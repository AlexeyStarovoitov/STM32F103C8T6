#ifndef CAN_H_
#define CAN_H_

#define CAN1_BRP ((uint32_t)7)
#define CAN1_TS1 ((uint32_t)(4 << 16))
#define CAN1_TS2 ((uint32_t)(2 << 20))
#define CAN1_SJW ((uint32_t)(0 << 24))

#define CAN1_TX_PRIOR	((uint32_t)3)
#define CAN1_RX0_PRIOR	((uint32_t)1)
#define CAN1_RX1_PRIOR	((uint32_t)2)

#define CAN_MCR_DBF	((uint32_t)0x00010000)

#define RX_F0_ID ((uint16_t)125)
#define RX_F1_ID ((uint16_t)126)

#define TX_F0_ID ((uint16_t)123)
#define TX_F1_ID ((uint16_t)124)

void can_init(void);
void can_start(void);

#endif
