//header files
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f3_discovery.h"
#include "stm32f3xx_hal.h"
#include "stm32f303xc.h"
#include "common.h"
#include "IR_sensor.h"
#include "buzzer.h"
#include "pwm.h"



//function proto-types
void blinkLed(uint8_t ledVal);
void TIM_DELAY(uint16_t value);
void configure_DCmotor();
void stopMotor(void);
void cmdForward(int mode);
void cmdReverse(int mode);
void cmdRight(int mode);
void cmdLeft(int mode);
void cmdStop(int mode);
void reset_1(void);
void reset_2(void);



//globals
uint32_t dutyCycle = 0;
uint32_t button;



void blinkLed(uint8_t i)
{
  for(int x = 0; x<20; x++)
   {
     BSP_LED_Toggle(i);
     TIM_DELAY(100);
   }
}




void TIM_DELAY(uint16_t value)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;	  // Enable Clock APB1(36Mhz), value is 0x10
  TIM6->ARR = value;      		  // delay in ms
  TIM6->PSC = 36000-1;  		  // APB1 = 36Mhz so eqn :  CLK_CNT = (36000000)/36000,i.e. CLK_CNT = 1Khz
  TIM6->CR1 = 0x01;         		  // Enable TIMER 
  while(!((TIM6->SR & 0x01) == 0x01));    // Check for Update Event
  TIM6->CR1 &= ~(0x01);                   // Disable timer
  TIM6->SR &= ~(0x01);                    // Reset the update event
}



void reset_1(void)
{
  GPIOC->MODER   &= 0X00000000;		 
  GPIOC->OTYPER  &= 0X00000000;		
  GPIOC->OSPEEDR &= 0X00000000;
  GPIOC->AFR[1]  &= 0X00000000;

  GPIOB->MODER   &= 0X00000000;		 
  GPIOB->OTYPER  &= 0X00000000;		
  GPIOB->OSPEEDR &= 0X00000000;
  GPIOB->AFR[1]  &= 0X00000000;
}



void configure_DCmotor()
{

  /* Initialize the pushbutton */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);


  //initialize the LEDs for indication
   for(int i=0; i<8; i++) 
    {
      BSP_LED_Init(i);
    }

  //enable GPIOC clock
  RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;  
  RCC->AHBENR  |= RCC_AHBENR_GPIOBEN;
   
  reset_1(); //reset alternate functions on PWM pins PC8, PB1

  //PC0,PC3,PC8
  GPIOC->MODER 	 |= 0X00010041;          //GPIO Mode: output 
  GPIOC->OTYPER  |= 0X00000109;          //Output Type: Open Drain
  GPIOC->OSPEEDR |= 0X000300C3;          //GPIO Speed: 50MHz    
  GPIOC->PUPDR 	 |= 0X00010041;          //PULL UP
 
  //PB1
  GPIOB->MODER 	 |= 0X00000004;          //GPIO Mode: output 
  GPIOB->OTYPER  |= 0X00000002;          //Output Type: Open Drain
  GPIOB->OSPEEDR |= 0X0000000C;          //GPIO Speed: 50MHz    
  GPIOB->PUPDR 	 |= 0X00000004;          //PULL UP	
	
}




void stopMotor(void)
{

  if(button == 1) printf("Motor stopped...\n");
  else printf("Obstacle detected...\n");
  GPIOC->ODR &= 0X00000000;
  GPIOB->ODR &= 0X00000000; 
  stopDC();

}




void cmdForward(int mode)
{

  configure_DCmotor();

  if(mode != CMD_INTERACTIVE)
   {
     return;
   }
  
  while(readSensorVal() != 1)
   {  
    button = BSP_PB_GetState(BUTTON_USER);
    if(button ==1) break;
    GPIOC->ODR |= 0X00000001; //set bits in ODR for PC0
    GPIOB->ODR |= 0X00000002; //set bits in ODR for PB1 
    blinkLed(0);
   }

  stopMotor();
  buzzerOn();
  TIM_DELAY(3000);
  buzzerOff();
  

}
ADD_CMD("forward", cmdForward, "		Run the DC Motor in Forward direction");




void cmdReverse(int mode)
{

  configure_DCmotor();

  if(mode != CMD_INTERACTIVE) 
   {
     return;
   }
   
  
  while(readSensorVal() != 1)
   {  
    button = BSP_PB_GetState(BUTTON_USER);
    if(button ==1) break;
    GPIOC->ODR |= 0X00000108; //set bits in ODR for PC8 and PC3
    GPIOB->ODR &= 0X00000000;
    blinkLed(7);
   }

  stopMotor();     
  buzzerOn();
  TIM_DELAY(3000);
  buzzerOff();

}
ADD_CMD("reverse", cmdReverse, "		Run the DC Motor in Backward direction");




void reset_2(void)
{
  GPIOC->MODER 	 &= 0X00000000;          
  GPIOC->OTYPER  &= 0X00000000;          
  GPIOC->OSPEEDR &= 0X00000000;              
  GPIOC->PUPDR 	 &= 0X00000000;          
 
  GPIOB->MODER 	 &= 0X00000000;          
  GPIOB->OTYPER  &= 0X00000000;          
  GPIOB->OSPEEDR &= 0X00000000;              
  GPIOB->PUPDR 	 &= 0X00000000;
}




void cmdRight(int mode)
{
  int rc;

  if(mode != CMD_INTERACTIVE) 
   {
     return;
   }
  
  /* Initialize the pushbutton */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

  rc = fetch_uint32_arg(&dutyCycle);
  if(rc) 
  {
    printf("Please enter the duty cycle\n");
    return;
  }	
  

  reset_2();         
  
  while(readSensorVal() != 1)
   {
    button = BSP_PB_GetState(BUTTON_USER);
    if(button ==1) break;  
    pwm1(dutyCycle);
    blinkLed(4);
   }

  stopMotor(); 
  buzzerOn();
  TIM_DELAY(3000);
  buzzerOff();
}
ADD_CMD("right", cmdRight, "			Run the DC Motor Rightwards");		




void cmdLeft(int mode)
{
  int rc;

  if(mode != CMD_INTERACTIVE) 
   {
     return;
   }

  /* Initialize the pushbutton */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

  rc = fetch_uint32_arg(&dutyCycle);
  if(rc) 
  {
    printf("Please enter the duty cycle\n");
    return;
  }
     
  reset_2();

  while(readSensorVal() != 1)
   {  
    button = BSP_PB_GetState(BUTTON_USER);
    if(button ==1) break;
    pwm2(dutyCycle);	
    blinkLed(3);
   }

  stopMotor();
  buzzerOn();
  TIM_DELAY(3000);
  buzzerOff();

}
ADD_CMD("left", cmdLeft, "			Run the DC Motor Leftwards");





