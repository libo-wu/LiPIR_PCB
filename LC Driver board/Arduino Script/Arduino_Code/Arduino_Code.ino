/*
	Description:
	Generate out-of-phase PWM wave on Arduino Nano Board from Pin D3 and D11.
	Connect D3 and D11 to IN1 and IN2 of the PCB board, as well as the GND.
	Insert 9V battery.
	Test the output on OUT1 and OUT2
	*/
/*
 * Output A freq is 16MHz/256/256/2=122Hz
 * Output A duty cycle : 
 * Output B freq is 16MHz/256/256/2=122Hz, prescalar is 256, CS22:CS20=110
 * Output B duty cycle : 
 */

#include <SoftwareSerial.h>

// attach softwareserial of BT
// SoftwareSerial BTserial(0,1); //set Bluetooth Rx=D0,Tx=D1

// set the dutycycle of the PWM
byte dutyCycleA = 110;
byte dutyCycleB = 256- dutyCycleA;


int PWM_state=0;
int  value;

// inttrupt information
int INT_number=1;   // A counter to compare with second_to_toggle
int seconds_to_toggle = 6; // the seconds needed to toggle PWM state. 
void setup() 
 {
  pinMode (3, OUTPUT);    // Timer 2 "B" output: OC2B, pin number is D3 on Arduino Nano
  pinMode (11, OUTPUT);   // Timer 2 "A" output: OC2A
  pinMode(PIR_pin, INPUT);

  cli();
  // Set OC2A on Compare Match when up-counting.
  // Clear OC2B on Compare Match when down-counting.
  //TCCR2A = _BV (WGM20) | _BV (COM2B1) | _BV (COM2A1) | _BV (COM2A0);    
  TCCR2B = _BV (CS22) | _BV (CS21) ;         // phase correct PWM, prescaler of 1024, Fc=16000000/256/256/2=122Hz
  OCR2A = 256 - dutyCycleA;           // duty cycle out of 255. The actual duty cycle is (dutycycleA/256)= 45.3%, count from 256-dutyCycle to 256, length is dutycycle
  OCR2B = dutyCycleA;     // duty cycle out of 255. dutycycle = (256-dutycycleA/256)= , count down from dutycycle to zero

  //set timer1 interrupt every 1 seconds
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1 seconds 
  OCR1A = 15624;// = [(16*10^6) / (1*1024) - 1] (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  Serial.begin(38400);
  //BTserial.println("Connected to BT.");

  sei();
  
  }  // end of setup

void loop() {
  }
  
void PWM_off(){
  TCCR2A = 0x00;  //disable PWM
  PWM_state = 0;
}

void PWM_on(){
  TCCR2A = _BV (WGM20) | _BV (COM2B1) | _BV (COM2A1) | _BV (COM2A0);  //enable pwm
  PWM_state = 100;
}

ISR(TIMER1_COMPA_vect){//timer1 interrupts every 3 seconds then toggle PWM state
  if (INT_number % seconds_to_toggle ==0){
      if (PWM_state==100){
        PWM_off();
       }
      else{
        PWM_on();
      }
      INT_number=1; //return to initial value 1 and start counting again
  }
  else {
    INT_number++;
  }

}
