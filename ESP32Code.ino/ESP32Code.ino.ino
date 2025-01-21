/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

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

// Load Wi-Fi library
#include <WiFi.h>
// Load Dakota library
// https://github.com/AyushMarsian/SR74HC595
#include <SR74HC595.h>
// https://github.com/bolderflight/sbus
#include <sbus.h>


// Replace with your network credentials
const char* ssid = "Olympus";
const char* password = "choponionseatrocks";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Pin Definitions
#define LED_BUILTIN 2

#define M1 12 //M1 PWM pin, Driver Board pin 11
#define M2 32 //M2 PWM pin, Driver Board pin 3
#define M3 26 //M3 PWM pin, Driver Board pin 6
#define M4 25 //M4 PWM pin, Driver Board pin 5

#define M1_REV false // set to true to reverse M1 direction
#define M2_REV false // set to true to reverse M2 direction
#define M3_REV false // set to true to reverse M3 direction
#define M4_REV true // set to true to reverse M4 direction

#define S1 32 //S1 PWM pin, Driver Board pin 9
#define S2 33 //S2 PWM pin, Driver Board pin 10

#define SR_DATA 14 // Serial Register data pin, Driver Board pin 8
#define SR_CLK 33 // Serial Register clock pin, Driver Board pin 4
#define SR_LTCH 13 // Serial Register Latch Pin, Driver Board pin 12
#define SR_EN 27 // Serial Register Enable, Driver Board pin 7

#define SBUS_RX 16 // Serial port for SBUS commands
#define SBUS_TX 17 // Serial port for Telem return

#define OFF 0b00  
#define FWD 0b01 
#define REV 0b10

#define MOVESPEED 130 // default speed for movements

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
  // Clear bits 7 and 5 in SR_state
  motor_state &= ~(0b10100000);
  // Set bit 5
  motor_state |= (dir & 0b10) << 4;
  // Set bit 7
  motor_state |= (dir & 0b01) << 7; 
  // Update the SR
  SR.sendToShiftRegister(motor_state);
  // set the speed
  analogWrite(M3, speed);
}
void setM4(uint8_t speed, uint8_t dir)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  if (M4_REV) dir = ~dir; // Swaps FWD and REV if motor turns the wrong way
  // Clear bits 0 and 6 in SR_state
  motor_state &= ~(0b01000001);
  // Set bit 6
  motor_state |= (dir & 0b10) << 5;
  // Set bit 0
  motor_state |= (dir & 0b01); 
  // Update the SR
  SR.sendToShiftRegister(motor_state);
  // set the speed
  analogWrite(M4, speed);
}

// Variables to store the current output state
bool isMoveLeftOn = 0;
bool isMoveRightOn = 0;
bool isMoveForwardOn = 0;
bool isMoveBackwardOn = 0;
bool isRotateLeftOn = 0;
bool isRotateRightOn = 0;


// Motion Functions
void MoveLeft(){
  setM1(MOVESPEED, FWD);
  setM2(MOVESPEED, REV);
  setM3(MOVESPEED, FWD);
  setM4(MOVESPEED, REV);
  return;
}
void MoveRight(){
  setM1(MOVESPEED, REV);
  setM2(MOVESPEED, FWD);
  setM3(MOVESPEED, REV);
  setM4(MOVESPEED, FWD);
  return;
}
void MoveForward(){
  setM1(MOVESPEED, FWD);
  setM2(MOVESPEED, FWD);
  setM3(MOVESPEED, FWD);
  setM4(MOVESPEED, FWD);
  return;
}
void MoveBackward(){
  setM1(MOVESPEED, REV);
  setM2(MOVESPEED, REV);
  setM3(MOVESPEED, REV);
  setM4(MOVESPEED, REV);
  return;
}
void RotateLeft(){
  setM1(MOVESPEED, FWD);
  setM2(MOVESPEED, REV);
  setM3(MOVESPEED, REV);
  setM4(MOVESPEED, FWD);
  return;
}
void RotateRight(){
  setM1(MOVESPEED, REV);
  setM2(MOVESPEED, FWD);
  setM3(MOVESPEED, FWD);
  setM4(MOVESPEED, REV);
  return;
}
void Stationary(){
  setM1(0, OFF);
  setM2(0, OFF);
  setM3(0, OFF);
  setM4(0, OFF);
  return;
}

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Sends a Hello World Debug message
  Serial.println("Starting RASTICxHackH Spring 2025 Controller...");
  
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

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
// Move Left
            // turns the GPIOs on and off
            if (header.indexOf("GET /MoveLeftOn") >= 0) {
              // Prints that we're moving left
              Serial.println("MoveLeft");

              // Sets flags that we're moving left
              isMoveLeftOn = 1;
              isMoveRightOn = 0;
              isMoveForwardOn = 0;
              isMoveBackwardOn = 0;
              isRotateLeftOn = 0;
              isRotateRightOn = 0;
              
              // Turns on pins to move left
              MoveLeft();
// Move Right
            } else if (header.indexOf("GET /MoveRightOn") >= 0) {
              // Prints that we're moving right
              Serial.println("MoveRight");

              // Sets flags that we're moving right
              isMoveLeftOn = 0;
              isMoveRightOn = 1;
              isMoveForwardOn = 0;
              isMoveBackwardOn = 0;
              isRotateLeftOn = 0;
              isRotateRightOn = 0;

              // Turns on pins to move right
              MoveRight();
// Move Forward
            } else if (header.indexOf("GET /MoveForwardOn") >= 0) {
              // Prints that we're moving forward
              Serial.println("MoveForward");

              // Sets flags that we're moving forward
              isMoveLeftOn = 0;
              isMoveRightOn = 0;
              isMoveForwardOn = 1;
              isMoveBackwardOn = 0;
              isRotateLeftOn = 0;
              isRotateRightOn = 0;

              // Turns on pins to move forward
              MoveForward();
// Move Backward
            } else if (header.indexOf("GET /MoveBackwardOn") >= 0) {
              // Prints that we're moving backward
              Serial.println("MoveBackward");

              // Sets flags that we're moving backward
              isMoveLeftOn = 0;
              isMoveRightOn = 0;
              isMoveForwardOn = 0;
              isMoveBackwardOn = 1;
              isRotateLeftOn = 0;
              isRotateRightOn = 0;

              // Turns on pins to move backward
              MoveBackward();
// Rotate Left
            } else if (header.indexOf("GET /RotateLeftOn") >= 0) {
              // Prints that we're rotating left
              Serial.println("RotateLeft");

              // Sets flags that we're rotating left
              isMoveLeftOn = 0;
              isMoveRightOn = 0;
              isMoveForwardOn = 0;
              isMoveBackwardOn = 0;
              isRotateLeftOn = 1;
              isRotateRightOn = 0;

              // Turns on pins to rotate left
              RotateLeft();
// Rotate Right
            } else if (header.indexOf("GET /RotateRightOn") >= 0) {
              // Prints that we're rotating right
              Serial.println("RotateRight");

              // Sets flags that we're rotating right
              isMoveLeftOn = 0;
              isMoveRightOn = 0;
              isMoveForwardOn = 0;
              isMoveBackwardOn = 0;
              isRotateLeftOn = 0;
              isRotateRightOn = 1;

              // Turns on pins to rotate right
              RotateRight();
// Stationary
            } else{
              // Prints that the robot is stationary
              Serial.println("Stationary");

              // Set all other motion flags to 0
              isMoveLeftOn = 0;
              isMoveRightOn = 0;
              isMoveForwardOn = 0;
              isMoveBackwardOn = 0;
              isRotateLeftOn = 0;
              isRotateRightOn = 0;

              // Turns off all pins
              Stationary();

            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            


            // Changes UI based on which button is pressed.

            // Display Move Left Button
            client.println("Move Left");
            // If the MoveLeft is off, it displays the MoveLeft button       
            if (!isMoveLeftOn) {
              client.println("<p><a href=\"/MoveLeftOn\"><button class=\"button\">MoveLeft</button></a></p>");
            } else {
              client.println("<p><a href=\"/Stationary\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display Move Right Button
            client.println("Move Right");
            // If the MoveRight is off, it displays the MoveRight button       
            if (!isMoveRightOn) {
              client.println("<p><a href=\"/MoveRightOn\"><button class=\"button\">MoveRight</button></a></p>");
            } else {
              client.println("<p><a href=\"/Stationary\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display Move Forward Button
            client.println("Move Forward");
            // If the MoveForward is off, it displays the MoveForward button       
            if (!isMoveForwardOn) {
              client.println("<p><a href=\"/MoveForwardOn\"><button class=\"button\">MoveForward</button></a></p>");
            } else {
              client.println("<p><a href=\"/Stationary\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display Move Backward Button
            client.println("Move Backward");
            // If the MoveBackward is off, it displays the MoveBackward button       
            if (!isMoveBackwardOn) {
              client.println("<p><a href=\"/MoveBackwardOn\"><button class=\"button\">MoveBackward</button></a></p>");
            } else {
              client.println("<p><a href=\"/Stationary\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display Rotate Left Button
            client.println("Rotate Left");
            // If the RotateLeft is off, it displays the RotateLeft button       
            if (!isRotateLeftOn) {
              client.println("<p><a href=\"/RotateLeftOn\"><button class=\"button\">RotateLeft</button></a></p>");
            } else {
              client.println("<p><a href=\"/Stationary\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display Rotate Right Button
            client.println("Rotate Right");
            // If the RotateRight is off, it displays the RotateRight button       
            if (!isRotateRightOn) {
              client.println("<p><a href=\"/RotateRightOn\"><button class=\"button\">RotateRight</button></a></p>");
            } else {
              client.println("<p><a href=\"/Stationary\"><button class=\"button button2\">OFF</button></a></p>");
            }
               

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
