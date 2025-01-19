/*  Copyright to AyushMarsian(Ankit Ghevariya)
 *  Licence GNU GPL V3
 *  
 *  Example for use shift register with Arduino UNO
 *  Written by Ankit Ghevariya, public domain
 *  
 *  You have to connect pin as written below 
 *  (74HC595) --------- (Arduino)
 *   Vcc      --------> 5v 
 *   MR       --------> 5v
 *   DS       --------> Digital 5
 *   SH_CP    --------> Digital 6
 *   ST_CP    --------> Digital 7
 *   OE       --------> Gnd
 *   Gnd      --------> Gnd
 */

// https://github.com/AyushMarsian/SR74HC595
#include <SR74HC595.h>

#define LED_BUILTIN 2

#define M1 25 //M1 PWM pin, Driver Board pin 11
#define M2 23 //M2 PWM pin, Driver Board pin 3
#define M3 19 //M3 PWM pin, Driver Board pin 6
#define M4 21 //M4 PWM pin, Driver Board pin 5

#define M1_REV false // set to true to reverse M1 direction
#define M2_REV false // set to true to reverse M2 direction
#define M3_REV false // set to true to reverse M3 direction
#define M4_REV false // set to true to reverse M4 direction

#define S1 32 //S1 PWM pin, Driver Board pin 9
#define S2 33 //S2 PWM pin, Driver Board pin 10

#define SR_DATA 4 // Serial Register data pin, Driver Board pin 8
#define SR_CLK 22 // Serial Register clock pin, Driver Board pin 4
#define SR_LTCH 26 // Serial Register Latch Pin, Driver Board pin 12
#define SR_EN 18 // Serial Register Enable, Driver Board pin 7

#define OFF 0b00  
#define FWD 0b01 
#define REV 0b10

SR74HC595 SR(SR_DATA,SR_CLK,SR_LTCH); // Initialize Serial Register on Driver Board (Data, Clock, Latch)

uint8_t motor_state = 0x00; // bitstring that defines the behavior of each motor driver via the Serial Register

void setM1(uint8_t speed, uint8_t dir)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  if (M1_REV) dir = ~dir; // Swaps FWD and REV if motor turns the wrong way
  // Clear bits 2 and 3 in SR_state
  motor_state &= ~(0b00001100); // Mask to clear bits 2 and 3
  // Set bits 2 and 3 with the value of dir
  motor_state |= (dir << 2);    // Shift dir to align with bits 2 and 3, then OR
  // Update the SR
  SR.sendToShiftRegister(motor_state);
  // set the speed
  analogWrite(M1, speed);
}

void setM2(uint8_t speed, uint8_t dir)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  if (M2_REV) dir = ~dir; // Swaps FWD and REV if motor turns the wrong way
  // Clear bits 1 and 4 in SR_state
  motor_state &= ~(0b00010010); 
  // Set bit 1 
  motor_state |= (dir & 0b10);
  // Set bit 4
  motor_state |= (dir & 0b01) << 4; // Shift left 4
  // Update the SR
  SR.sendToShiftRegister(motor_state);
  // set the speed
  analogWrite(M2, speed);
}

void setM3(uint8_t speed, uint8_t dir)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  if (M3_REV) dir = ~dir; // Swaps FWD and REV if motor turns the wrong way
  // Clear bits 0 and 6 in SR_state
  motor_state &= ~(0b01000001);
  // Set bit 0
  motor_state |= (dir & 0b10) >> 1;
  // Set bit 6
  motor_state |= (dir & 0b01) << 6; 
  // Update the SR
  SR.sendToShiftRegister(motor_state);
  // set the speed
  analogWrite(M3, speed);
}
void setM4(uint8_t speed, uint8_t dir)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  if (M4_REV) dir = ~dir; // Swaps FWD and REV if motor turns the wrong way
  // Clear bits 5 and 7 in SR_state
  motor_state &= ~(0b10100000);
  // Set bit 5
  motor_state |= (dir & 0b10) << 4;
  // Set bit 7
  motor_state |= (dir & 0b01) << 7; 
  // Update the SR
  SR.sendToShiftRegister(motor_state);
  // set the speed
  analogWrite(M4, speed);
}
void setup() 
{
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(SR_DATA, OUTPUT);
  pinMode(SR_CLK, OUTPUT);
  pinMode(SR_LTCH, OUTPUT);
  pinMode(SR_EN, OUTPUT);

  digitalWrite(SR_EN, LOW); // Activate the Serial register (Active Low)

  SR.sendToShiftRegister(0x00); // Set all output pin of shift register to 0.

}

void loop() 
{
  Serial.println("beginning sequence...");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("Activating M1");
  setM1(100, FWD);
  Serial.println(motor_state, BIN);
  delay(2000);
  setM1(0, OFF);
  Serial.println(motor_state, BIN);
  delay(500);
  
  Serial.println("Activating M2");
  setM2(100, FWD);
  Serial.println(motor_state, BIN);
  delay(2000);
  setM2(0, OFF);
  Serial.println(motor_state, BIN);
  delay(500);
  
  Serial.println("Activating M3");
  setM3(100, FWD);
  Serial.println(motor_state, BIN);
  delay(2000);
  setM3(0, OFF);
  Serial.println(motor_state, BIN);
  delay(500);

  Serial.println("Activating M4");
  setM4(100, FWD);
  Serial.println(motor_state, BIN);
  delay(2000);
  setM4(0, OFF);
  Serial.println(motor_state, BIN);
  delay(500);
}
