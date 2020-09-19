#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_OUT_50_MHz GPIO_CRL_MODE0
#define GPIO_OUT_PP ((uint32_t)0x00000000)
#define GPIO_IN ((uint32_t)0x00000000)
#define GPIO_AF_PP GPIO_CRL_CNF0_1
#define GPIO_IN_PU_PD GPIO_CRL_CNF0_1
#define GPIO_IN ((uint32_t)0x00000000)
#define GPIO_IN_FLT GPIO_CRL_CNF0_0

#define GPIO_S ((uint16_t)0x0001)
#define GPIO_R ((uint16_t)0x0001)


#define GPIO0	((uint8_t)0)
#define GPIO1	((uint8_t)1)
#define GPIO2	((uint8_t)2)
#define GPIO3	((uint8_t)3)
#define GPIO4	((uint8_t)4)
#define GPIO5	((uint8_t)5)
#define GPIO6	((uint8_t)6)
#define GPIO7	((uint8_t)7)
#define GPIO8	((uint8_t)8)
#define GPIO9	((uint8_t)9)
#define GPIO10	((uint8_t)10)
#define GPIO11	((uint8_t)11)
#define GPIO12 ((uint8_t)12)
#define GPIO13 ((uint8_t)13)
#define GPIO14 ((uint8_t)14)
#define GPIO15 ((uint8_t)15)


#define GPIO0_MODE	((uint8_t)0)
#define GPIO1_MODE	((uint8_t)1)
#define GPIO2_MODE	((uint8_t)2)
#define GPIO3_MODE	((uint8_t)3)
#define GPIO4_MODE	((uint8_t)4)
#define GPIO5_MODE	((uint8_t)5)
#define GPIO6_MODE	((uint8_t)6)
#define GPIO7_MODE	((uint8_t)7)
#define GPIO8_MODE	((uint8_t)0)
#define GPIO9_MODE	((uint8_t)1)
#define GPIO10_MODE	((uint8_t)2)
#define GPIO11_MODE	((uint8_t)3)
#define GPIO12_MODE ((uint8_t)4)
#define GPIO13_MODE ((uint8_t)5)
#define GPIO14_MODE ((uint8_t)6)
#define GPIO15_MODE ((uint8_t)7)

#define EXTI_GPIO_MASK (GPIO_CRL_MODE0 | GPIO_CRL_CNF0)
#define EXTI0_PRIOR ((uint32_t)0)

void exti_gpio_init(void);

#endif
