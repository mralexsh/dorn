#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define CHANELLS_SIZE 58
#define START_24_FREQ 2412
#define START_5_FREQ 5180
#define PFD 20.0

typedef struct {
    uint16_t integer_ratio;
    uint16_t fraction_ratio;
} WI_FI_CHANNEL_STRUCTURE;

typedef uint32_t UFix26_16;

static uint16_t channel_frequencies[CHANELLS_SIZE] = {
		2412,   2417,   2422,   2427,   2432,   2437,   2442,   2447,   2452,   2457,   2462,   2467,   2472,   2484,

		5180, 5190, 5200, 5220, 5230, 5240, 5260, 5270, 5280, 5300,
		5310, 5320, 5500, 5510, 5520, 5540, 5550, 5560, 5580, 5590,
		5600, 5620, 5630, 5640, 5660, 5670, 5680, 5700, 5710, 5720,
		5745, 5755, 5765, 5785, 5795, 5805, 5825, 5860, 5870, 5875,
		5880, 5885, 5890, 5865
};


// PFD = 6.75
//#define DIV_RATIO_24_COEF 0.197530864197530864
//#define DIV_RATIO_5_COEF 0.118518518518518519

//#define DIV_RATIO_24_COEF
//#define DIV_RATIO_5_COEF 


// PFD = 9
//#define DIV_RATIO_24_COEF 0.148148148148148148
//#define DIV_RATIO_5_COEF 0.088888888888888889

//#define DIV_RATIO_24_COEF
//#define DIV_RATIO_5_COEF 




// PFD = 13.5
//#define DIV_RATIO_24_COEF 0.098765432098765427
//#define DIV_RATIO_5_COEF 0.059259259259259262

//#define DIV_RATIO_24_COEF 1657009
//#define DIV_RATIO_5_COEF  994205

// PFD = 20
//#define DIV_RATIO_24_COEF 0.06666666666666666666666666666667
//#define DIV_RATIO_5_COEF 0.04

//#define DIV_RATIO_24_COEF 1118481
//#define DIV_RATIO_5_COEF 671089

#define FIXED_POINT_FRACTIONAL_BITS 16

typedef struct {
	uint32_t div_ratio24_coef;
	uint32_t div_ratio5_coef;
} RATIOS;

RATIOS pfdRatios[4] = {
		{1118481, 671089},
		{1657009, 994205},
		{2485513, 1491308},
		{3314018, 1988411},
};

//----------------------------------


WI_FI_CHANNEL_STRUCTURE calcWifiDiv(uint16_t f, uint8_t is24, uint8_t ratio_index) {
    WI_FI_CHANNEL_STRUCTURE r = { 0 };
    UFix26_16 m = is24 == 1 ? f * pfdRatios[ratio_index].div_ratio24_coef : f * pfdRatios[ratio_index].div_ratio5_coef;
    uint16_t frac = (m >> 8) & 0xFFFF;        
    r.integer_ratio = (m >> 24) | (frac & 0x3000);
    r.fraction_ratio = frac >> 2;
    return r;
}


//----------------------------------
uint8_t* toBinary(const void* v, int size) {
    const uint8_t* bytes = (uint8_t *)v;
    const uint16_t mSize = size * 8;
    uint8_t* r = malloc(mSize + 1);
    for (int j = size - 1, k = 0; j >= 0; --j, ++k) {
        uint8_t b = bytes[j];
        for (int i = 7; i >= 0; --i, b >>= 1) {
            r[k * 8 + i] = (1 & b) + '0';
        }
    }
    r[mSize] = '\0';
    return r;
}


uint16_t getExp(double val) {
    const uint64_t *binR =  &val;
    return (uint16_t)((*binR >> 52) & 0x7FF);
}

uint64_t getMant(double val) {
    const uint64_t* binR = &val;
    return (uint64_t)(((*binR | 0x20000000000000) << 12)  >> 52);
}

UFix26_16 convertToFixedPoint(double val) {
   
    return (UFix26_16)(round(val * 0xFFFFFF));
    //return (UFix26_16)(round(val * 0xFFFFFFFF));
}

void printFreqPlan(uint8_t ratio_index) {

  printf("fRF (MHz)     |fRF x 4/3 20 MHz   |Integer Divider ratio     |Fraction Divider ratio     \n");
  for (int i = 0; i < 14; ++i) {
        WI_FI_CHANNEL_STRUCTURE channel = calcWifiDiv(channel_frequencies[i], 1, ratio_index);
        uint8_t* int_bin = toBinary(&channel.integer_ratio, sizeof(uint16_t));
        uint8_t* frac_bin = toBinary(&channel.fraction_ratio, sizeof(uint16_t));

        printf("%d             %d           %s              %0x%.4X          %s \n", channel_frequencies[i], channel.integer_ratio, int_bin, channel.fraction_ratio, frac_bin);
        free(int_bin);
        free(frac_bin);
    }

    printf("===================\n\n\n");

}


int main() {


  printFreqPlan(0);
  printFreqPlan(1);
  printFreqPlan(2);
  printFreqPlan(3);

	/*
    UFix26_16 c1 = convertToFixedPoint(DIV_RATIO_24_COEF);
    UFix26_16 c2 = convertToFixedPoint(DIV_RATIO_5_COEF);
    uint8_t* c1Bin = toBinary(&c1, sizeof(UFix26_16));
    
    printf("DIV_RATIO_24_COEF %ld  binary = %s\n", c1, c1Bin);
    printf("DIV_RATIO_5_COEF %ld \n", c2);
    free(c1Bin);
*/
    

    /*
    double c1 = DIV_RATIO_24_COEF;
    
    uint16_t t = TEST_VAL;

    uint8_t* t1 = toBinary(&t, sizeof(uint16_t));
    uint8_t* b1 = toBinary(&c1, sizeof(double));
    

    uint16_t exp16 = getExp(c1);    
    uint8_t* exp = toBinary(&exp16, sizeof(uint16_t));

    uint64_t mantissa64 = getMant(c1);
    uint8_t* mantissa = toBinary(&mantissa64, sizeof(uint64_t)); ;

    printf("%s\n\n", t1);
    printf("%s\n", b1);
    
    printf("Exponenta = %s\n", exp);
    printf("Mantissa = %s\n", mantissa);

    UFix26_16 cv = convertToFixedPoint(c1) * 2412;
    uint8_t* convertedValue = toBinary(&cv, sizeof(UFix26_16));

    printf("Converted value = %s\n", convertedValue);

    free(b1);
    free(exp);
    free(mantissa);
    free(convertedValue);
    */

    return 0;
}
