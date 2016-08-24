//header files
#include <stdio.h>





#include <stdint.h>
#include <string.h>
#include "stm32f3_discovery.h"
#include "stm32f303xc.h"
#include "common.h"



// FUNCTION		: pwm1()
// DESCRIPTION		: The function configures pin pc8 connected to channel 3 of timer 3
//                 		   to output pulse width modualated signals
// PARAMETERS    	: uint32_t dutycycle
// RETURNS       	: void
void pwm1(uint32_t dutyCycle)
{
  	RCC->AHBENR  |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock
  	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Enable Timer 3 clock
  
  	// PC8 configuration
  	GPIOC->MODER   |= 2 << (8*2);		// Alternate function mode 
  	GPIOC->OTYPER  |= 0 << 8;		// Output push-pull (reset state)
  	GPIOC->OSPEEDR |= 0 << (8*2);		// 2 MHz High speed
  	GPIOC->AFR[1]  |= 2 << ((8-8)*4);	// Select AF2 for PC8: TIM3_CH3
  	TIM3->PSC = 5999;			// Set prescaler to 6000 (PSC + 1)
  	TIM3->ARR = 600;			// Auto reload value 600
  	TIM3->CCR3 = dutyCycle;			// Start PWM duty for channel 3 on timer 3

        // PWM mode 2 on channel 3
  	TIM3->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1|TIM_CCMR2_OC3M_0; 

  	TIM3->CCER |= TIM_CCER_CC3E;		// Enable compare on channel 3
  	TIM3->CR1 |= TIM_CR1_CEN;		// Enable timer
} 



// FUNCTION		: pwm2()
// DESCRIPTION		: The function configures pin PB1 connected to channel 4 of timer 3
//                  		   to output pulse width modualated signals
// PARAMETERS		: uint32_t dutycycle
// RETURNS       	: void
void pwm2(uint32_t dutyCycle)
{  
  	RCC->AHBENR  |= RCC_AHBENR_GPIOBEN; 	// Enable GPIOB clock
  	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; 	// Enable Timer 3 clock
  
  	// PB1 configuration
  	GPIOB->MODER   |= 0x00000008;		//Alternate function mode
  	GPIOB->OTYPER  |= 0x00000000;		//Output push-pull (reset state)
  	GPIOB->OSPEEDR |= 0x00000000;		//configure for 
  	GPIOB->AFR[0]  |= 0x00000020;		//Select AF2 for PB1: TIM3_CH3

  	TIM3->PSC = 5999;			//Set prescaler to 6000 (PSC + 1)
  	TIM3->ARR = 600;			//Auto reload value 600
  	TIM3->CCR4 = dutyCycle;			//Start PWM duty for channel 4 on timer 3


        // PWM mode 2 on channel 3
        TIM3->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0;  

  	TIM3->CCER |= TIM_CCER_CC4E;		// Enable compare on channel 4
  	TIM3->CR1 |= TIM_CR1_CEN;		// Enable timer
} 



// FUNCTION		: CmdDCmotor_antiClock
// DESCRIPTION		: The function generates PWM signals on PC8 to drive the DC motor 
//                 	  in the anticlockwise direction depending on input duty cycle
// PARAMETERS		: int mode
// RETURNS		: void
void CmdDCmotor_antiClock(int mode){	
        uint32_t dutyCycle;  
        int rc;
	if(mode != CMD_INTERACTIVE) {
    	 return;
  	}
        rc = fetch_uint32_arg(&dutyCycle);	//gets value of Duty cycle form terminal
        if(rc) 
         {
           printf("Please enter the duty cycle\n");
           return;
         }	
        pwm1(dutyCycle);			//calls pwm1 function			
}
ADD_CMD("dcmotora", CmdDCmotor_antiClock, "Run the DC Motor anti-clockwise direction ");



// FUNCTION		: CmdDCmotor_antiClock
// DESCRIPTION		: The function generates PWM signals on PB1 to drive the DC motor 
//			   in the clockwise direction depending on input duty cycle from user
// PARAMETERS		: int mode
// RETURNS		: void
void CmdDCmotor_Clock(int mode){

        uint32_t dutyCycle;     
	int rc;   
	if(mode != CMD_INTERACTIVE) {
    	 return;
  	}

        rc = fetch_uint32_arg(&dutyCycle);	//gets value of Duty cycle from terminal
        if(rc) 
         {
           printf("Please enter the duty cycle\n");
           return;
         }
	pwm2(dutyCycle); 
}
ADD_CMD("dcmotorc", CmdDCmotor_Clock, "Runs the DC Motor clockwise direction");



void stopDC()
{

	TIM3->CCR3 = 0;	//stops the motor form rotating anticlockwise by setting
	TIM3->CCR4 = 0;	//stops the motor form rotating clockwise 
}



void CmdStop_DCmotor(int mode){

	if(mode != CMD_INTERACTIVE) {
    	 return;
  	}
	TIM3->CCR3 = 0;	//stops the motor form rotating anticlockwise by setting
	TIM3->CCR4 = 0;	//stops the motor form rotating clockwise 
}
ADD_CMD("stopdc", CmdStop_DCmotor, "		Stop the Motor clockwise");



