/*
 * WMY_SCCB.c
 *
 *  Created on: 2018年4月18日
 *      Author: wmy
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/interrupt.h"
#include "driverlib/fpu.h"
#include "driverlib/qei.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "time.h"
#include "inc/hw_i2c.h"
#include "driverlib/rom.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "string.h"
#include "driverlib/timer.h"
#include "WMY_SCCB.h"

#define SCCB0_BASE GPIO_PORTE_BASE
#define SCCB0_SCL GPIO_PIN_2
#define SCCB0_SDA GPIO_PIN_3
#define SCCB0_ID 0x21//ov7725 id

void Delay_us(uint32_t x)
{
    ROM_SysCtlDelay((SysCtlClockGet()/3000)*x/1000);
}

void SCCB0_Init(void)
{
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL |SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);//80Mhz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    GPIOPinTypeGPIOOutput(SCCB0_BASE, SCCB0_SCL);//SCL Output
    GPIOPinTypeGPIOOutput(SCCB0_BASE, SCCB0_SDA);//SDA Output
    GPIOPadConfigSet(SCCB0_BASE,SCCB0_SCL,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);//上拉
    GPIOPadConfigSet(SCCB0_BASE,SCCB0_SDA,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);//上拉
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2,GPIO_PIN_2);//输出高
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_3,GPIO_PIN_3);//输出高
    GPIOPinTypeGPIOOutput(SCCB0_BASE, SCCB0_SDA);//SDA Output
}

void SCCB0_Start(void)
{
    GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,SCCB0_SCL);//输出高
    GPIOPinWrite(SCCB0_BASE,SCCB0_SDA,SCCB0_SDA);//输出高
    Delay_us(50);
    GPIOPinWrite(SCCB0_BASE,SCCB0_SDA,0x0);//输出低
    Delay_us(50);
    GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,0x0);//输出低
}

void SCCB0_Stop(void)
{
    GPIOPinWrite(SCCB0_BASE,SCCB0_SDA,0x0);//输出低
    Delay_us(50);
    GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,SCCB0_SCL);//输出高
    Delay_us(50);
    GPIOPinWrite(SCCB0_BASE,SCCB0_SDA,SCCB0_SDA);//输出高
    Delay_us(50);
}

void SCCB0_No_Ack(void)
{
    Delay_us(50);
    GPIOPinWrite(SCCB0_BASE,SCCB0_SDA,SCCB0_SDA);//输出高
    GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,SCCB0_SCL);//输出高
    Delay_us(50);
    GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,0x0);//输出低
    Delay_us(50);
    GPIOPinWrite(SCCB0_BASE,SCCB0_SDA,0x0);//输出低
    Delay_us(50);
}

uint8_t SCCB0_Write_Byte(uint8_t data)
{
    uint8_t i, Res;
    for(i=0; i<8; i++)
        {
            if(data&0x80)
                {
                    GPIOPinWrite(SCCB0_BASE,SCCB0_SDA,SCCB0_SDA);//输出高
                }
            else
                {
                    GPIOPinWrite(SCCB0_BASE,SCCB0_SDA,0x0);//输出低
                }
            data<<=1;
            Delay_us(50);
            GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,SCCB0_SCL);//输出高
            Delay_us(50);
            GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,0x0);//输出低
        }
    GPIOPinTypeGPIOInput(SCCB0_BASE, SCCB0_SDA);//SDA
    Delay_us(50);
    GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,SCCB0_SCL);//输出高
    Delay_us(50);
    if(GPIOPinRead(SCCB0_BASE,SCCB0_SDA))
    {
        Res=1;
    }
    else
    {
        Res=0;
    }
    GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,0x0);//输出低
    GPIOPinTypeGPIOOutput(SCCB0_BASE, SCCB0_SDA);//SDA
    return Res;
}

uint8_t SCCB0_Read_Byte(void)
{
    uint8_t temp=0,j;
    GPIOPinTypeGPIOInput(SCCB0_BASE, SCCB0_SDA);//SDA
    for(j=8;j>0;j--)
    {
        Delay_us(50);
        GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,SCCB0_SCL);//输出高
        temp=temp<<1;
        if(GPIOPinRead(SCCB0_BASE,SCCB0_SDA))
        {
            temp++;
        }
        Delay_us(50);
        GPIOPinWrite(SCCB0_BASE,SCCB0_SCL,0x0);//输出低
    }
    GPIOPinTypeGPIOOutput(SCCB0_BASE, SCCB0_SDA);//SDA
    return temp;
}

uint8_t SCCB0_Write_Reg(uint8_t reg,uint8_t data)
{
    uint8_t Res=0;
    SCCB0_Start();
    if(SCCB0_Write_Byte(SCCB0_ID))
    {
        Res=1;
    }
    Delay_us(100);
    if(SCCB0_Write_Byte(reg))
    {
        Res=1;
    }
    Delay_us(100);
    if(SCCB0_Write_Byte(data))
    {
        Res=1;
    }
    SCCB0_Stop();
    return  Res;
}

uint8_t SCCB0_Read_Reg(uint8_t reg)
{
    uint8_t val=0;
    SCCB0_Start();
    SCCB0_Write_Byte(SCCB0_ID);
    Delay_us(100);
    SCCB0_Write_Byte(reg);
    Delay_us(100);
    SCCB0_Stop();
    Delay_us(100);
    SCCB0_Start();
    SCCB0_Write_Byte(SCCB0_ID|0X01);
    Delay_us(100);
    val=SCCB0_Read_Byte();
    SCCB0_No_Ack();
    SCCB0_Stop();
    return val;
}
