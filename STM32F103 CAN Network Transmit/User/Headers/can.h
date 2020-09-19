#ifndef CAN_H_
#define CAN_H_

#define CAN1_BRP ((uint32_t)7)
#define CAN1_TS1 ((uint32_t)(4 << 16))
#define CAN1_TS2 ((uint32_t)(2 << 20))
#define CAN1_SJW ((uint32_t)(0 << 24))

#define CAN1_PRIOR	((uint32_t)1)

#define CAN_MCR_DBF	((uint32_t)0x00010000)
#define CAN_TX_CMP_MASK (CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2)

#define RX_ID ((uint16_t)321)
#define TX_ID1 ((uint16_t)123)
#define TX_ID2 ((uint16_t)124)
#define TX_ID3 ((uint16_t)125)


void can_init(void);

#endif
