#ifndef MAIN_H_
#define MAIN_H_

#define DIG_NUM ((uint16_t)0x000A)

#define NUM_RATE_MASK_0 ((uint16_t)0x0001)
#define NUM_RATE_MASK_1 ((uint16_t)0x0002)
#define NUM_RATE_MASK_2 ((uint16_t)0x0004)
#define NUM_RATE_MASK_3 ((uint16_t)0x0008)
#define NUM_RATE_MASK_4 ((uint16_t)0x0010)
#define NUM_RATE_MASK_5 ((uint16_t)0x0020)
#define NUM_RATE_MASK_6 ((uint16_t)0x0040)
#define NUM_RATE_MASK_7 ((uint16_t)0x0080)
#define NUM_RATE_MASK_8 ((uint16_t)0x0100)
#define NUM_RATE_MASK_9 ((uint16_t)0x0200)

#define PI 3.141592653589793
#define LEN ((uint16_t)1024)
#define fd ((float)5120)
#define OMEGA ((float)fd/LEN)
	

uint16_t next_bin_inv_num_fun(uint16_t bin_inv_num, uint16_t n);
void arr_inv_trans(float complex arr[], uint16_t len);
float complex wn1_calc(uint16_t n);
float complex wnk_calc(uint16_t k, float complex wn1);
void fft_user(float complex arr[],  float arr2[], uint16_t n);


#endif
