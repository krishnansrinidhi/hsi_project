/*
   FILE          : IR_sensor.c
   PROJECT       : CNTR8000-16S - Assignment #2
   PROGRAMMER    : Krishnan-Bengaluru,Srinidhi  Student
   FIRST VERSION : June 9, 2016
   DESCRIPTION   : This program configures and reads the ADC on STM32F303.
		   Displays readinngs on terminal through the simulated
		   serial port(minicom). A 10k Pot is connected on PA.0
                   of STM32F3 to read the analog value.
*/



//header files
#include <stdio.h>
#include <stdint.h>
#include "stm32f303xc.h"
#include "common.h"



// FUNCTION      : configureADC3()
// DESCRIPTION   : The function configures the ADC1 and GPIOA and reads the
//                 ADC Value 
// PARAMETERS    : The function doesn't accept any input parameters
// RETURNS       : returns an uint32_t
uint32_t configureADC3(void) {


 /* ADC clock configuration */
 RCC->CFGR2  |= RCC_CFGR2_ADCPRE34_DIV2;     //(0x00000110) ADC34 PLL clock divided by 2
 RCC->AHBENR |= RCC_AHBENR_ADC34EN;	     //(0x10000000) ADC3/ADC4 clock enable
 RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	     //(0x00020000) GPIOB Periph clock enable
 GPIOB->MODER|= 0x00000003;		     //Configure PB0 as analog input 

 
 /* Calibration procedure */
 ADC3->CR &= ~ADC_CR_ADVREGEN;		     //Reset ADC Control Register
 ADC3->CR |=  ADC_CR_ADVREGEN_0;	     //ADC Voltage regulator enabled
 for(int8_t i = 0; i<10; i++);               //approximate delay of 10us
 ADC3->CR &= ~ADC_CR_ADCALDIF;               //ADC Differential Mode for calibration
 ADC3->CR |=  ADC_CR_ADCAL; 		     //Start ADC calibration 
 while (ADC3->CR & ADC_CR_ADCAL); 	     //wait until calibration done
 //uint32_t calibration_value = ADC1->CALFACT; //Get Calibration Value ADC1 


 /* ADC configuration */ 
 ADC3->CFGR &= ~ADC_CFGR_CONT ; //ADC Single Conversion Mode Enable
 ADC3->CFGR &= ~ADC_CFGR_RES;   //12-bit data resolution
 ADC3->CFGR &= ~ADC_CFGR_ALIGN; //Right data alignment


 /* ADC3 regular channel12 configuration for PB0 */
 ADC3->SQR1  |= 0x00000300;    //start converting ch12 
 ADC3->SQR1  &= ~ADC_SQR1_L;   //ADC regular channel sequence length=0, that is 1 conversion/sequence
 ADC3->SMPR1 |= (ADC_SMPR1_SMP7_1 | ADC_SMPR1_SMP7_0); // = 0x03 => sampling time 7.5 ADC clock cycles


 /* read value from ADC */
 ADC3->CR |= ADC_CR_ADEN;                //Enable ADC3 
 while(!ADC3->ISR & ADC_ISR_ADRD);       //wait for ADRDY 
 ADC3->CR |= ADC_CR_ADSTART;             //Start ADC1 Software Conversion
 while(!(ADC3->ISR & ADC_ISR_EOC));      //Test EOC flag
 uint32_t ADC3ConvertedValue = ADC3->DR; //Get ADC3 converted data
 return ADC3ConvertedValue;

}



// FUNCTION      : CmdADC3()
// DESCRIPTION   : The function prints the ADC value
//		   continuously on the terminal 
// PARAMETERS    : accepts an int
// RETURNS       : returns nothing
void CmdADC3(int mode)
{
	
  if(mode != CMD_INTERACTIVE) 
   {
     return;
   }
	
  //while(1)
   //{
     uint32_t val = configureADC3();
     printf("The Val is %d\n", (unsigned int)val);        
   //}
}
ADD_CMD("adc3", CmdADC3, "		The ADC1 value");




uint8_t readSensorVal(void)
{
  
  uint32_t val = configureADC3();
  
  if(val == 4095) return 0; 
 
  else return 1; 

} 

