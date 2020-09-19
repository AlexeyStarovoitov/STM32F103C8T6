#ifndef TIMS_H_
#define TIMS_H_


#define TIM2_PSC ((uint16_t)9999)
#define TIM3_PSC ((uint16_t)35)
#define TIM2_ARR ((uint16_t)1799)
#define TIM2_PRIOR	((uint32_t)0)

void tim2_init(void);
void tim3_init(void);
void tim2_start(void);
void tim3_start(void);
void tim3_stop(void);
void delay_us(uint32_t t_us);

#endif
