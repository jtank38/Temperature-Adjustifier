/* Assignment 5 - Interrupt */

// ------- Preamble -------- //
#include <stdio.h>              //* Import needed for sprintf *//
#include <stdlib.h>
#include <avr/io.h>             //* AVR library *//
#include "USART.h"              //* import for USART *//
#include <util/setbaud.h>       //* import for baudrates *//
#include <util/delay.h>         //* import for delays *//
#include <stdbool.h>            //* import for boolean *//
#include <avr/interrupt.h>      //* import for interrupts *//
#include <string.h>             //* import for string manipulation *//
#include <float.h>
#define BIT_IS_SET(byte,bit) ((byte &(1 << bit)))
#define BIT_IS_CLEAR(byte,bit) (!(byte &(1 << bit)))   //* function for checking if a pin in byte is clear *//


static inline void initUSART(void){                   // initialization function for USART and LEDs

  UBRR0H = UBRRH_VALUE;                        // set baud rate
  UBRR0L = UBRRL_VALUE;                        // set baud rate

  #if USE_2X                                   // to adjust settings for baud rate depending on CPU clock speed
  UCSR0A |= (1 << U2X0);        
  #else
  UCSR0A &= ~(1 << U2X0);
  #endif

  UCSR0B = (1 << TXEN0) | (1 << RXEN0);        // enable USART transmitter and receive
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);      // set packet size (8-bits)

  UCSR0C &= ~(1 << USBS0);                     // stop bit amount (1 stop bit)
}

static inline void initADC(){

  ADMUX &= ~(1<<REFS1);               //CLEAR REFS1 (AVCC) 
  ADMUX |= (1<<REFS0);                //SET REFS0 (AVCC)



  //Config for division factor 8   

  ADCSRA &= ~(1<<ADPS2);              //Enable ADC division factor 8
  ADCSRA |= (1<<ADPS1);               //Enable ADC division factor 8
  ADCSRA |= (1<<ADPS0);               //Enable ADC division factor 8
  ADCSRA |= (1<<ADEN);                //ADEN Set to start conversion

  
  DDRB |= (1<< PB1);                  //red LED
  DDRB |= (1<< PB0);                  //Green LED   //motion sensor

}

double getTemp(uint16_t adc_value){                  // Receive ADC Step Value and returns temperature
  float sensorVolt;                                        // initialize variable to store volatge
  float temp;                                              // initialize variable to store temperature

  sensorVolt = (float)((adc_value * 5000.0)/1024.0);       // convert step value to volatage in mV
  
  temp = (float)(sensorVolt/10.0);                         // temperature scale is 10mV/Degee Celsius

  return (temp);                                            // Return temperature

}

void printFloat(float temperature){               // Prints Float/Double Values to the serial Terminal

    char wholePart[16];                             // Char variale to store Whole part
    char DecPart[16];                               // Char variale to store Decimal part

    int intpart = (int)temperature;                 // Take the Whole part
    float decpart = temperature - intpart;         // Take the decimal part
    int int_dec = decpart * 1000;                   // Decimal part converted to integer (1000 precision)
    int_dec =abs(int_dec);
    sprintf(wholePart,"%d",intpart);                //Convert integer to string
    sprintf(DecPart,"%d",int_dec);                  //Convert integer to string
    printString(wholePart);                         //Print the string converted whole/integer part
    printString(".");
    printString(DecPart);                           //Print the string converted decimal part
    printString("   ");
}


uint16_t ReadADC(uint8_t __channel)              // Initalize the ADC channel, read values and return them
  {
    ADMUX = (ADMUX & 0xf0) | __channel;           // Channel selection: Preverse the first 4 bits and update the last 4 bits 
    ADCSRA |= (1<<ADSC);                          // Start conversion
   
    while(ADCSRA & (1 << ADSC));                  // Loop until conversion is complete
    return(ADC);                                  // Return the ADC value
  }


int main(void) {                                 // main function


  initADC();                                     // Call initialization function for ADC. 
  initUSART();                                   // Call initialization function for USART. 
  uint16_t stepValue = 0;                        //Assign Variable to get 16 bit ADC step value
  float sensA_temp1;                             // Assign Variable to store temperature from Temperature sensor A
  float sensB_temp2;                             // Assign Variable to store temperature from Temperature sensor B
  float temp_diff;                               // Assign Variable to store temperature difference 
  // float sum_;
  int count = 0;
  // _delay_ms(60000);                             // Delay kept to initialize the Motion Sensor
  
  while(1)
  {
    DDRC &= ~(1 << PC0);                         // Clear bit for Motion Sensor input
    DDRC &= ~(1 << PC1);
    if BIT_IS_SET(PINC,PC0)                      // Conditon to only switch on when the motion sensor detects something
    {
      while(1)                                   // Inner Main Loop
      {

      if BIT_IS_SET(PINC,PC1){                   //  If the Motion sensor 2 is triggered break from loop
        PORTB &= ~(1<<PB0);                      //  Green LED is Off 
        PORTB |= (1<<PB1);                       // Red LED is turned on
        break;
      }
      PORTB &= ~(1<<PB1);                        // Blue LED off when motion sensor 1 is triggered
      PORTB |= (1<<PB0);                         // Green LED to indicate Motion Sesor(1) reading 
      
      stepValue = ReadADC(5);                    // Read ADC5  
      printString("\r"); 
      sensA_temp1 = getTemp(stepValue);          // Get temperature for Sensor A

      
      stepValue = ReadADC(4);                    // Read ADC4 
      sensB_temp2 = getTemp(stepValue);          // Get temperature for Sensor B

      temp_diff = sensB_temp2 - sensA_temp1 + 5;     // Get the difference in temperature


      float sum_ = sum_ + temp_diff ;             // Offset to get correct temperature
      
      if (count==500){                           // Conditon for if and when we get 500 sensor readings calculate mean   
        printString("\n \r");

        float mean = sum_ /(float) count;         // take average to smooth the values
        
        if (mean < 1.0 && mean > -1.0){          // if average is between 1.0 and -1.0 indicate no change
          printString("\n \r");
          printString("NODATA");
          }
        else {                                   // Else transmit the average value
        printFloat(mean);
        printString("\n \r");
        printString("END");

          }
        sum_ = 0.0;                              // reset sum_ to 0 once we hit 500 values
        count = 0;                               // reset count to 0 once we hit 500 values
      }
      count = count+1;                           // Increment count   
      }
    } 
  }                  
  return 0;                                      // Never reaches here        
}





   

  


