#include <math.h> 
#include <complex.h>
#include <stdio.h>
#include <stdint.h>

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
#define LEN 1024
#define fd ((float)5120)
#define OMEGA ((float)fd/LEN)


uint16_t next_bin_inv_num_fun(uint16_t bin_inv_num, uint16_t n);
void arr_inv_trans(float complex arr[], uint16_t len);
float complex wn1_calc(uint16_t n);
float complex wnk_calc(uint16_t k, float complex wn1);
void fft_user(float complex arr[],  float arr2[], uint16_t n);

//float complex datas[LEN] = {0,0.40698,0.80542,1.187,1.5437,1.8682,2.1539,2.3949,2.5867,2.7255,2.809,2.8361,2.8069,2.7229,2.5866,2.4019,2.1736,1.9075,1.6101,1.2888,0.95106,0.60489,0.25827,-0.0809,-0.40506,-0.70711,-0.98061,-1.2199,-1.4204,-1.5782,-1.691,-1.7572,-1.7768,-1.7506,-1.6808,-1.5706,-1.4243,-1.2467,-1.0438,-0.82191,-0.58779,-0.34844,-0.11093,0.11783,0.33124,0.52321,0.68826,0.82172,0.91983,0.97981,1,0.97981,0.91983,0.82172,0.68826,0.52321,0.33124,0.11783,-0.11093,-0.34844,-0.58779,-0.82191,-1.0438,-1.2467,-1.4243,-1.5706,-1.6808,-1.7506,-1.7768,-1.7572,-1.691,-1.5782,-1.4204,-1.2199,-0.98061,-0.70711,-0.40506,-0.0809,0.25827,0.60489,0.95106,1.2888,1.6101,1.9075,2.1736,2.4019,2.5866,2.7229,2.8069,2.8361,2.809,2.7255,2.5867,2.3949,2.1539,1.8682,1.5437,1.187,0.80542,0.40698,1.592e-15,-0.40698,-0.80542,-1.187,-1.5437,-1.8682,-2.1539,-2.3949,-2.5867,-2.7255,-2.809,-2.8361,-2.8069,-2.7229,-2.5866,-2.4019,-2.1736,-1.9075,-1.6101,-1.2888,-0.95106,-0.60489,-0.25827,0.0809,0.40506,0.70711,0.98061,1.2199,1.4204,1.5782,1.691,1.7572,1.7768,1.7506,1.6808,1.5706,1.4243,1.2467,1.0438,0.82191,0.58779,0.34844,0.11093,-0.11783,-0.33124,-0.52321,-0.68826,-0.82172,-0.91983,-0.97981,-1,-0.97981,-0.91983,-0.82172,-0.68826,-0.52321,-0.33124,-0.11783,0.11093,0.34844,0.58779,0.82191,1.0438,1.2467,1.4243,1.5706,1.6808,1.7506,1.7768,1.7572,1.691,1.5782,1.4204,1.2199,0.98061,0.70711,0.40506,0.0809,-0.25827,-0.60489,-0.95106,-1.2888,-1.6101,-1.9075,-2.1736,-2.4019,-2.5866,-2.7229,-2.8069,-2.8361,-2.809,-2.7255,-2.5867,-2.3949,-2.1539,-1.8682,-1.5437,-1.187,-0.80542,-0.40698,-3.1841e-15,0.40698,0.80542,1.187,1.5437,1.8682,2.1539,2.3949,2.5867,2.7255,2.809,2.8361,2.8069,2.7229,2.5866,2.4019,2.1736,1.9075,1.6101,1.2888,0.95106,0.60489,0.25827,-0.0809,-0.40506,-0.70711,-0.98061,-1.2199,-1.4204,-1.5782,-1.691,-1.7572,-1.7768,-1.7506,-1.6808,-1.5706,-1.4243,-1.2467,-1.0438,-0.82191,-0.58779,-0.34844,-0.11093,0.11783,0.33124,0.52321,0.68826,0.82172,0.91983,0.97981,1,0.97981,0.91983,0.82172,0.68826,0.52321,0.33124,0.11783,-0.11093,-0.34844,-0.58779,-0.82191,-1.0438,-1.2467,-1.4243,-1.5706,-1.6808,-1.7506,-1.7768,-1.7572,-1.691,-1.5782,-1.4204,-1.2199,-0.98061,-0.70711,-0.40506,-0.0809,0.25827,0.60489,0.95106,1.2888,1.6101,1.9075,2.1736,2.4019,2.5866,2.7229,2.8069,2.8361,2.809,2.7255,2.5867,2.3949,2.1539,1.8682,1.5437,1.187,0.80542,0.40698,-2.3293e-15,-0.40698,-0.80542,-1.187,-1.5437,-1.8682,-2.1539,-2.3949,-2.5867,-2.7255,-2.809,-2.8361,-2.8069,-2.7229,-2.5866,-2.4019,-2.1736,-1.9075,-1.6101,-1.2888,-0.95106,-0.60489,-0.25827,0.0809,0.40506,0.70711,0.98061,1.2199,1.4204,1.5782,1.691,1.7572,1.7768,1.7506,1.6808,1.5706,1.4243,1.2467,1.0438,0.82191,0.58779,0.34844,0.11093,-0.11783,-0.33124,-0.52321,-0.68826,-0.82172,-0.91983,-0.97981,-1,-0.97981,-0.91983,-0.82172,-0.68826,-0.52321,-0.33124,-0.11783,0.11093,0.34844,0.58779,0.82191,1.0438,1.2467,1.4243,1.5706,1.6808,1.7506,1.7768,1.7572,1.691,1.5782,1.4204,1.2199,0.98061,0.70711,0.40506,0.0809,-0.25827,-0.60489,-0.95106,-1.2888,-1.6101,-1.9075,-2.1736,-2.4019,-2.5866,-2.7229,-2.8069,-2.8361,-2.809,-2.7255,-2.5867,-2.3949,-2.1539,-1.8682,-1.5437,-1.187,-0.80542,-0.40698,-6.3682e-15,0.40698,0.80542,1.187,1.5437,1.8682,2.1539,2.3949,2.5867,2.7255,2.809,2.8361,2.8069,2.7229,2.5866,2.4019,2.1736,1.9075,1.6101,1.2888,0.95106,0.60489,0.25827,-0.0809,-0.40506,-0.70711,-0.98061,-1.2199,-1.4204,-1.5782,-1.691,-1.7572,-1.7768,-1.7506,-1.6808,-1.5706,-1.4243,-1.2467,-1.0438,-0.82191,-0.58779,-0.34844,-0.11093,0.11783,0.33124,0.52321,0.68826,0.82172,0.91983,0.97981,1,0.97981,0.91983,0.82172,0.68826,0.52321,0.33124,0.11783,-0.11093,-0.34844,-0.58779,-0.82191,-1.0438,-1.2467,-1.4243,-1.5706,-1.6808,-1.7506,-1.7768,-1.7572,-1.691,-1.5782,-1.4204,-1.2199,-0.98061,-0.70711,-0.40506,-0.0809,0.25827,0.60489,0.95106,1.2888,1.6101,1.9075,2.1736,2.4019,2.5866,2.7229,2.8069,2.8361,2.809,2.7255,2.5867,2.3949,2.1539,1.8682,1.5437,1.187,0.80542,0.40698,-2.4014e-14,-0.40698,-0.80542,-1.187,-1.5437,-1.8682,-2.1539,-2.3949,-2.5867,-2.7255,-2.809,-2.8361,-2.8069,-2.7229,-2.5866,-2.4019,-2.1736,-1.9075,-1.6101,-1.2888,-0.95106,-0.60489,-0.25827,0.0809,0.40506,0.70711,0.98061,1.2199,1.4204,1.5782,1.691,1.7572,1.7768,1.7506,1.6808,1.5706,1.4243,1.2467,1.0438,0.82191,0.58779,0.34844,0.11093,-0.11783,-0.33124,-0.52321,-0.68826,-0.82172,-0.91983,-0.97981,-1,-0.97981,-0.91983,-0.82172,-0.68826,-0.52321,-0.33124,-0.11783,0.11093,0.34844,0.58779,0.82191,1.0438,1.2467,1.4243,1.5706,1.6808,1.7506,1.7768,1.7572,1.691,1.5782,1.4204,1.2199,0.98061,0.70711,0.40506,0.0809,-0.25827,-0.60489,-0.95106,-1.2888,-1.6101,-1.9075,-2.1736,-2.4019,-2.5866,-2.7229,-2.8069,-2.8361,-2.809,-2.7255,-2.5867,-2.3949,-2.1539,-1.8682,-1.5437,-1.187,-0.80542,-0.40698,4.6586e-15,0.40698,0.80542,1.187,1.5437,1.8682,2.1539,2.3949,2.5867,2.7255,2.809,2.8361,2.8069,2.7229,2.5866,2.4019,2.1736,1.9075,1.6101,1.2888,0.95106,0.60489,0.25827,-0.0809,-0.40506,-0.70711,-0.98061,-1.2199,-1.4204,-1.5782,-1.691,-1.7572,-1.7768,-1.7506,-1.6808,-1.5706,-1.4243,-1.2467,-1.0438,-0.82191,-0.58779,-0.34844,-0.11093,0.11783,0.33124,0.52321,0.68826,0.82172,0.91983,0.97981,1,0.97981,0.91983,0.82172,0.68826,0.52321,0.33124,0.11783,-0.11093,-0.34844,-0.58779,-0.82191,-1.0438,-1.2467,-1.4243,-1.5706,-1.6808,-1.7506,-1.7768,-1.7572,-1.691,-1.5782,-1.4204,-1.2199,-0.98061,-0.70711,-0.40506,-0.0809,0.25827,0.60489,0.95106,1.2888,1.6101,1.9075,2.1736,2.4019,2.5866,2.7229,2.8069,2.8361,2.809,2.7255,2.5867,2.3949,2.1539,1.8682,1.5437,1.187,0.80542,0.40698,-1.3725e-14,-0.40698,-0.80542,-1.187,-1.5437,-1.8682,-2.1539,-2.3949,-2.5867,-2.7255,-2.809,-2.8361,-2.8069,-2.7229,-2.5866,-2.4019,-2.1736,-1.9075,-1.6101,-1.2888,-0.95106,-0.60489,-0.25827,0.0809,0.40506,0.70711,0.98061,1.2199,1.4204,1.5782,1.691,1.7572,1.7768,1.7506,1.6808,1.5706,1.4243,1.2467,1.0438,0.82191,0.58779,0.34844,0.11093,-0.11783,-0.33124,-0.52321,-0.68826,-0.82172,-0.91983,-0.97981,-1,-0.97981,-0.91983,-0.82172,-0.68826,-0.52321,-0.33124,-0.11783,0.11093,0.34844,0.58779,0.82191,1.0438,1.2467,1.4243,1.5706,1.6808,1.7506,1.7768,1.7572,1.691,1.5782,1.4204,1.2199,0.98061,0.70711,0.40506,0.0809,-0.25827,-0.60489,-0.95106,-1.2888,-1.6101,-1.9075,-2.1736,-2.4019,-2.5866,-2.7229,-2.8069,-2.8361,-2.809,-2.7255,-2.5867,-2.3949,-2.1539,-1.8682,-1.5437,-1.187,-0.80542,-0.40698,-1.2736e-14,0.40698,0.80542,1.187,1.5437,1.8682,2.1539,2.3949,2.5867,2.7255,2.809,2.8361,2.8069,2.7229,2.5866,2.4019,2.1736,1.9075,1.6101,1.2888,0.95106,0.60489,0.25827,-0.0809,-0.40506,-0.70711,-0.98061,-1.2199,-1.4204,-1.5782,-1.691,-1.7572,-1.7768,-1.7506,-1.6808,-1.5706,-1.4243,-1.2467,-1.0438,-0.82191,-0.58779,-0.34844,-0.11093,0.11783,0.33124,0.52321,0.68826,0.82172,0.91983,0.97981,1,0.97981,0.91983,0.82172,0.68826,0.52321,0.33124,0.11783,-0.11093,-0.34844,-0.58779,-0.82191,-1.0438,-1.2467,-1.4243,-1.5706,-1.6808,-1.7506,-1.7768,-1.7572,-1.691,-1.5782,-1.4204,-1.2199,-0.98061,-0.70711,-0.40506,-0.0809,0.25827,0.60489,0.95106,1.2888,1.6101,1.9075,2.1736,2.4019,2.5866,2.7229,2.8069,2.8361,2.809,2.7255,2.5867,2.3949,2.1539,1.8682,1.5437,1.187,0.80542,0.40698,1.0776e-14,-0.40698,-0.80542,-1.187,-1.5437,-1.8682,-2.1539,-2.3949,-2.5867,-2.7255,-2.809,-2.8361,-2.8069,-2.7229,-2.5866,-2.4019,-2.1736,-1.9075,-1.6101,-1.2888,-0.95106,-0.60489,-0.25827,0.0809,0.40506,0.70711,0.98061,1.2199,1.4204,1.5782,1.691,1.7572,1.7768,1.7506,1.6808,1.5706,1.4243,1.2467,1.0438,0.82191,0.58779,0.34844,0.11093,-0.11783,-0.33124,-0.52321,-0.68826,-0.82172,-0.91983,-0.97981,-1,-0.97981,-0.91983,-0.82172,-0.68826,-0.52321,-0.33124,-0.11783,0.11093,0.34844,0.58779,0.82191,1.0438,1.2467,1.4243,1.5706,1.6808,1.7506,1.7768,1.7572,1.691,1.5782,1.4204,1.2199,0.98061,0.70711,0.40506,0.0809,-0.25827,-0.60489,-0.95106,-1.2888,-1.6101,-1.9075,-2.1736,-2.4019,-2.5866,-2.7229,-2.8069,-2.8361,-2.809,-2.7255,-2.5867,-2.3949,-2.1539,-1.8682,-1.5437,-1.187,-0.80542,-0.40698, 0,0.40698,0.80542,1.187,1.5437,1.8682,2.1539,2.3949,2.5867,2.7255,2.809,2.8361,2.8069,2.7229,2.5866,2.4019,2.1736,1.9075,1.6101,1.2888,0.95106,0.60489,0.25827,-0.0809};
float complex datas[LEN] = {0,0.079756,0.15945,0.23901,0.31839,0.3975,0.4763,0.55472,0.63269,0.71016,0.78705,0.86332,0.93889,1.0137,1.0877,1.1609,1.2331,1.3043,1.3745,1.4436,1.5115,1.5782,1.6437,1.7079,1.7707,1.8321,1.892,1.9504,2.0073,2.0626,2.1163,2.1682,2.2185,2.267,2.3137,2.3586,2.4017,2.4428,2.4821,2.5193,2.5546,2.588,2.6192,2.6485,2.6757,2.7007,2.7237,2.7446,2.7634,2.78,2.7945,2.8068,2.817,2.825,2.8308,2.8345,2.8361,2.8355,2.8327,2.8278,2.8208,2.8116,2.8004,2.787,2.7716,2.7542,2.7347,2.7132,2.6897,2.6643,2.6369,2.6076,2.5764,2.5435,2.5087,2.4721,2.4338,2.3938,2.3521,2.3088,2.264,2.2176,2.1697,2.1204,2.0697,2.0176,1.9643,1.9097,1.8539,1.797,1.7389,1.6799,1.6198,1.5589,1.497,1.4344,1.371,1.3069,1.2421,1.1768,1.1109,1.0446,0.97785,0.91077,0.84341,0.77582,0.70806,0.6402,0.57229,0.50439,0.43656,0.36886,0.30135,0.23409,0.16714,0.10054,0.034371,-0.031325,-0.096488,-0.16106,-0.225,-0.28824,-0.35073,-0.41242,-0.47325,-0.53319,-0.59218,-0.65016,-0.70711,-0.76296,-0.81767,-0.87121,-0.92352,-0.97457,-1.0243,-1.0727,-1.1198,-1.1654,-1.2095,-1.2522,-1.2934,-1.3331,-1.3711,-1.4076,-1.4425,-1.4757,-1.5073,-1.5371,-1.5653,-1.5918,-1.6165,-1.6395,-1.6607,-1.6802,-1.6978,-1.7137,-1.7279,-1.7402,-1.7507,-1.7595,-1.7665,-1.7717,-1.7751,-1.7767,-1.7766,-1.7748,-1.7712,-1.7659,-1.7589,-1.7502,-1.7399,-1.7279,-1.7142,-1.699,-1.6822,-1.6639,-1.644,-1.6227,-1.5999,-1.5757,-1.55,-1.5231,-1.4948,-1.4652,-1.4344,-1.4024,-1.3692,-1.3349,-1.2995,-1.2631,-1.2257,-1.1873,-1.1481,-1.108,-1.0671,-1.0254,-0.98304,-0.94002,-0.89641,-0.85224,-0.80756,-0.76244,-0.71692,-0.67105,-0.62489,-0.57849,-0.5319,-0.48517,-0.43836,-0.39152,-0.3447,-0.29796,-0.25134,-0.2049,-0.15869,-0.11277,-0.067171,-0.021961,0.022817,0.067112,0.11088,0.15406,0.19662,0.2385,0.27967,0.32007,0.35967,0.39841,0.43626,0.47317,0.50911,0.54404,0.57791,0.6107,0.64236,0.67286,0.70218,0.73027,0.7571,0.78266,0.8069,0.82981,0.85136,0.87152,0.89028,0.90762,0.92351,0.93793,0.95089,0.96235,0.9723,0.98075,0.98767,0.99306,0.99691,0.99923,1,0.99923,0.99691,0.99306,0.98767,0.98075,0.9723,0.96235,0.95089,0.93793,0.92351,0.90762,0.89028,0.87152,0.85136,0.82981,0.8069,0.78266,0.7571,0.73027,0.70218,0.67286,0.64236,0.6107,0.57791,0.54404,0.50911,0.47317,0.43626,0.39841,0.35967,0.32007,0.27967,0.2385,0.19662,0.15406,0.11088,0.067112,0.022817,-0.021961,-0.067171,-0.11277,-0.15869,-0.2049,-0.25134,-0.29796,-0.3447,-0.39152,-0.43836,-0.48517,-0.5319,-0.57849,-0.62489,-0.67105,-0.71692,-0.76244,-0.80756,-0.85224,-0.89641,-0.94002,-0.98304,-1.0254,-1.0671,-1.108,-1.1481,-1.1873,-1.2257,-1.2631,-1.2995,-1.3349,-1.3692,-1.4024,-1.4344,-1.4652,-1.4948,-1.5231,-1.55,-1.5757,-1.5999,-1.6227,-1.644,-1.6639,-1.6822,-1.699,-1.7142,-1.7279,-1.7399,-1.7502,-1.7589,-1.7659,-1.7712,-1.7748,-1.7766,-1.7767,-1.7751,-1.7717,-1.7665,-1.7595,-1.7507,-1.7402,-1.7279,-1.7137,-1.6978,-1.6802,-1.6607,-1.6395,-1.6165,-1.5918,-1.5653,-1.5371,-1.5073,-1.4757,-1.4425,-1.4076,-1.3711,-1.3331,-1.2934,-1.2522,-1.2095,-1.1654,-1.1198,-1.0727,-1.0243,-0.97457,-0.92352,-0.87121,-0.81767,-0.76296,-0.70711,-0.65016,-0.59218,-0.53319,-0.47325,-0.41242,-0.35073,-0.28824,-0.225,-0.16106,-0.096488,-0.031325,0.034371,0.10054,0.16714,0.23409,0.30135,0.36886,0.43656,0.50439,0.57229,0.6402,0.70806,0.77582,0.84341,0.91077,0.97785,1.0446,1.1109,1.1768,1.2421,1.3069,1.371,1.4344,1.497,1.5589,1.6198,1.6799,1.7389,1.797,1.8539,1.9097,1.9643,2.0176,2.0697,2.1204,2.1697,2.2176,2.264,2.3088,2.3521,2.3938,2.4338,2.4721,2.5087,2.5435,2.5764,2.6076,2.6369,2.6643,2.6897,2.7132,2.7347,2.7542,2.7716,2.787,2.8004,2.8116,2.8208,2.8278,2.8327,2.8355,2.8361,2.8345,2.8308,2.825,2.817,2.8068,2.7945,2.78,2.7634,2.7446,2.7237,2.7007,2.6757,2.6485,2.6192,2.588,2.5546,2.5193,2.4821,2.4428,2.4017,2.3586,2.3137,2.267,2.2185,2.1682,2.1163,2.0626,2.0073,1.9504,1.892,1.8321,1.7707,1.7079,1.6437,1.5782,1.5115,1.4436,1.3745,1.3043,1.2331,1.1609,1.0877,1.0137,0.93889,0.86332,0.78705,0.71016,0.63269,0.55472,0.4763,0.3975,0.31839,0.23901,0.15945,0.079756,-1.9607e-15,-0.079756,-0.15945,-0.23901,-0.31839,-0.3975,-0.4763,-0.55472,-0.63269,-0.71016,-0.78705,-0.86332,-0.93889,-1.0137,-1.0877,-1.1609,-1.2331,-1.3043,-1.3745,-1.4436,-1.5115,-1.5782,-1.6437,-1.7079,-1.7707,-1.8321,-1.892,-1.9504,-2.0073,-2.0626,-2.1163,-2.1682,-2.2185,-2.267,-2.3137,-2.3586,-2.4017,-2.4428,-2.4821,-2.5193,-2.5546,-2.588,-2.6192,-2.6485,-2.6757,-2.7007,-2.7237,-2.7446,-2.7634,-2.78,-2.7945,-2.8068,-2.817,-2.825,-2.8308,-2.8345,-2.8361,-2.8355,-2.8327,-2.8278,-2.8208,-2.8116,-2.8004,-2.787,-2.7716,-2.7542,-2.7347,-2.7132,-2.6897,-2.6643,-2.6369,-2.6076,-2.5764,-2.5435,-2.5087,-2.4721,-2.4338,-2.3938,-2.3521,-2.3088,-2.264,-2.2176,-2.1697,-2.1204,-2.0697,-2.0176,-1.9643,-1.9097,-1.8539,-1.797,-1.7389,-1.6799,-1.6198,-1.5589,-1.497,-1.4344,-1.371,-1.3069,-1.2421,-1.1768,-1.1109,-1.0446,-0.97785,-0.91077,-0.84341,-0.77582,-0.70806,-0.6402,-0.57229,-0.50439,-0.43656,-0.36886,-0.30135,-0.23409,-0.16714,-0.10054,-0.034371,0.031325,0.096488,0.16106,0.225,0.28824,0.35073,0.41242,0.47325,0.53319,0.59218,0.65016,0.70711,0.76296,0.81767,0.87121,0.92352,0.97457,1.0243,1.0727,1.1198,1.1654,1.2095,1.2522,1.2934,1.3331,1.3711,1.4076,1.4425,1.4757,1.5073,1.5371,1.5653,1.5918,1.6165,1.6395,1.6607,1.6802,1.6978,1.7137,1.7279,1.7402,1.7507,1.7595,1.7665,1.7717,1.7751,1.7767,1.7766,1.7748,1.7712,1.7659,1.7589,1.7502,1.7399,1.7279,1.7142,1.699,1.6822,1.6639,1.644,1.6227,1.5999,1.5757,1.55,1.5231,1.4948,1.4652,1.4344,1.4024,1.3692,1.3349,1.2995,1.2631,1.2257,1.1873,1.1481,1.108,1.0671,1.0254,0.98304,0.94002,0.89641,0.85224,0.80756,0.76244,0.71692,0.67105,0.62489,0.57849,0.5319,0.48517,0.43836,0.39152,0.3447,0.29796,0.25134,0.2049,0.15869,0.11277,0.067171,0.021961,-0.022817,-0.067112,-0.11088,-0.15406,-0.19662,-0.2385,-0.27967,-0.32007,-0.35967,-0.39841,-0.43626,-0.47317,-0.50911,-0.54404,-0.57791,-0.6107,-0.64236,-0.67286,-0.70218,-0.73027,-0.7571,-0.78266,-0.8069,-0.82981,-0.85136,-0.87152,-0.89028,-0.90762,-0.92351,-0.93793,-0.95089,-0.96235,-0.9723,-0.98075,-0.98767,-0.99306,-0.99691,-0.99923,-1,-0.99923,-0.99691,-0.99306,-0.98767,-0.98075,-0.9723,-0.96235,-0.95089,-0.93793,-0.92351,-0.90762,-0.89028,-0.87152,-0.85136,-0.82981,-0.8069,-0.78266,-0.7571,-0.73027,-0.70218,-0.67286,-0.64236,-0.6107,-0.57791,-0.54404,-0.50911,-0.47317,-0.43626,-0.39841,-0.35967,-0.32007,-0.27967,-0.2385,-0.19662,-0.15406,-0.11088,-0.067112,-0.022817,0.021961,0.067171,0.11277,0.15869,0.2049,0.25134,0.29796,0.3447,0.39152,0.43836,0.48517,0.5319,0.57849,0.62489,0.67105,0.71692,0.76244,0.80756,0.85224,0.89641,0.94002,0.98304,1.0254,1.0671,1.108,1.1481,1.1873,1.2257,1.2631,1.2995,1.3349,1.3692,1.4024,1.4344,1.4652,1.4948,1.5231,1.55,1.5757,1.5999,1.6227,1.644,1.6639,1.6822,1.699,1.7142,1.7279,1.7399,1.7502,1.7589,1.7659,1.7712,1.7748,1.7766,1.7767,1.7751,1.7717,1.7665,1.7595,1.7507,1.7402,1.7279,1.7137,1.6978,1.6802,1.6607,1.6395,1.6165,1.5918,1.5653,1.5371,1.5073,1.4757,1.4425,1.4076,1.3711,1.3331,1.2934,1.2522,1.2095,1.1654,1.1198,1.0727,1.0243,0.97457,0.92352,0.87121,0.81767,0.76296,0.70711,0.65016,0.59218,0.53319,0.47325,0.41242,0.35073,0.28824,0.225,0.16106,0.096488,0.031325,-0.034371,-0.10054,-0.16714,-0.23409,-0.30135,-0.36886,-0.43656,-0.50439,-0.57229,-0.6402,-0.70806,-0.77582,-0.84341,-0.91077,-0.97785,-1.0446,-1.1109,-1.1768,-1.2421,-1.3069,-1.371,-1.4344,-1.497,-1.5589,-1.6198,-1.6799,-1.7389,-1.797,-1.8539,-1.9097,-1.9643,-2.0176,-2.0697,-2.1204,-2.1697,-2.2176,-2.264,-2.3088,-2.3521,-2.3938,-2.4338,-2.4721,-2.5087,-2.5435,-2.5764,-2.6076,-2.6369,-2.6643,-2.6897,-2.7132,-2.7347,-2.7542,-2.7716,-2.787,-2.8004,-2.8116,-2.8208,-2.8278,-2.8327,-2.8355,-2.8361,-2.8345,-2.8308,-2.825,-2.817,-2.8068,-2.7945,-2.78,-2.7634,-2.7446,-2.7237,-2.7007,-2.6757,-2.6485,-2.6192,-2.588,-2.5546,-2.5193,-2.4821,-2.4428,-2.4017,-2.3586,-2.3137,-2.267,-2.2185,-2.1682,-2.1163,-2.0626,-2.0073,-1.9504,-1.892,-1.8321,-1.7707,-1.7079,-1.6437,-1.5782,-1.5115,-1.4436,-1.3745,-1.3043,-1.2331,-1.1609,-1.0877,-1.0137,-0.93889,-0.86332,-0.78705,-0.71016,-0.63269,-0.55472,-0.4763,-0.3975,-0.31839,-0.23901,-0.15945,-0.079756};
float datas2[LEN/2+1];
uint16_t bin_masks[DIG_NUM] = {NUM_RATE_MASK_9, NUM_RATE_MASK_8, NUM_RATE_MASK_7, NUM_RATE_MASK_6, NUM_RATE_MASK_5, NUM_RATE_MASK_4, NUM_RATE_MASK_3, NUM_RATE_MASK_2, NUM_RATE_MASK_1, NUM_RATE_MASK_0};


int main(void)
{

	uint16_t i;
	
		
	fft_user(datas, datas2, LEN);
	
	/* printf("%.3f, %.3f \n", creal(wn1_calc(LEN)), cimag(wn1_calc(LEN))); */
	
	printf("f: amps \n");
	for(i = 0; i <= LEN/2; i++)
		printf("%.3f %.3f\n", OMEGA*i, datas2[i]);
	

	/* while(1)
	{
	} */
}


uint16_t next_bin_inv_num_fun(uint16_t bin_inv_num, uint16_t n)
{
	uint16_t next_bin_inv_num = 0;
	uint16_t i, j;
	
	for(i = 0; i < DIG_NUM; i++)
	{
		if(!(bin_inv_num & bin_masks[i]))
		{
			next_bin_inv_num = bin_inv_num + n/((uint16_t)pow(2, i+1));
			for(j = i; j > 0; j--)
				next_bin_inv_num -= n/((uint16_t)pow(2, j));
			break;
		}
	}
	if (!next_bin_inv_num)
		next_bin_inv_num = 0;
	
	return next_bin_inv_num;
}

void arr_inv_trans(float complex arr[], uint16_t len)
{
	uint16_t i;
	uint16_t inv_index = 0;
	
	for(i = 1; i < len; i++)
	{
		inv_index = next_bin_inv_num_fun(inv_index, len);
		
		if(inv_index >= i)
		{
			float complex temp = arr[i];
			arr[i] = arr[inv_index];
			arr[inv_index] = temp;
		}
	}
}

float complex wn1_calc(uint16_t n)
{
	return (cos(2*PI/n) - sin(2*PI/n)*I);
}

float complex wnk_calc(uint16_t k, float complex wn1)
{
	if (k > 1)
		return wnk_calc(k-1, wn1)*wn1;
	else
		return wn1;
	
}

void fft_user(float complex arr[], float arr2[], uint16_t n)
{
	uint16_t i, j, k;
	uint16_t len = 1;
	uint16_t trans_num;
	float complex mul, s1;
	float complex w1 = wn1_calc(n);
	arr_inv_trans(arr, n);
	trans_num = (uint16_t)log2(n);
	
	
	for(i = 1; i<= trans_num; i++)
	{
		len *= 2;
		for(j = 0; j <= (n - len); j = j+len)
		{
			for(k = j; k < j+len/2; k++)
			{
				mul =  wnk_calc(n/len*(k-j), w1)*arr[k+len/2];
				s1 = arr[k];
				arr[k] = s1 + mul;
				arr[k+len/2] = s1 - mul;
			}
		}
	}
	for(i = 0; i <= n/2; i++)
	{
		if(!i)
			arr2[i] = cabsf(arr[i])/n;
		else
			arr2[i] = cabsf(arr[i])*2.0/n;
	}
	
}


