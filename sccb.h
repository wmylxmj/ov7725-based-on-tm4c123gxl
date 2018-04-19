/*
 * WMY_SCCB.h
 *
 *  Created on: 2018年4月18日
 *      Author: wmy
 */

#ifndef SCCB_WMY_SCCB_H_
#define SCCB_WMY_SCCB_H_

#define SCCB0_BASE GPIO_PORTE_BASE
#define SCCB0_SCL GPIO_PIN_2
#define SCCB0_SDA GPIO_PIN_3
#define SCCB0_ID 0x21

extern void Delay_us(uint32_t x);
extern void SCCB0_Init(void);
extern void SCCB0_Start(void);
extern void SCCB0_Stop(void);
extern void SCCB0_No_Ack(void);
extern uint8_t SCCB0_Write_Byte(uint8_t data);
extern uint8_t SCCB0_Read_Byte(void);
extern uint8_t SCCB0_Write_Reg(uint8_t reg,uint8_t data);
extern uint8_t SCCB0_Read_Reg(uint8_t reg);

#endif /* SCCB_WMY_SCCB_H_ */
