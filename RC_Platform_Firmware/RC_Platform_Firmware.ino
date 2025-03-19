// By Dakota Winslow

// Load Wi-Fi library
#include <WiFi.h>
#include <ESP32Servo.h>

// https://github.com/AyushMarsian/SR74HC595 <- this one sux don't use it
// #include <SR74HC595.h>
#include <ShiftRegister74HC595.h>
// https://github.com/TheDIYGuy999/SBUS
#include <SBUS.h>


// Replace with your network credentials
const char* ssid = "Olympus";
const char* password = "choponionseatrocks";

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

#define S1 22 //S1 PWM pin, Driver Board pin 9
#define S2 23 //S2 PWM pin, Driver Board pin 10

#define SERIAL2_RX 16
#define SERIAL2_TX 17
#define SBUS_INVERT false

#define SR_DATA 14 // Serial Register data pin, Driver Board pin 8
#define SR_CLK 33 // Serial Register clock pin, Driver Board pin 4
#define SR_LTCH 13 // Serial Register Latch Pin, Driver Board pin 12
#define SR_EN 27 // Serial Register Enable, Driver Board pin 7

#define OFF 0b00  
#define FWD 0b01 
#define REV 0b10

#define DEADZONE 2 // speed values smaller than this will be treated as "motor off"

// create a global shift register object
// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<1> SR(SR_DATA, SR_CLK, SR_LTCH);
// SR74HC595 SR(SR_DATA,SR_CLK,SR_LTCH); // Initialize Serial Register on Driver Board (Data, Clock, Latch)
SBUS SBUS_RX(Serial2);
WiFiServer server(80);

Servo tilt_servo;  // create Servo object to control a servo

// channel, fail safe, and lost frames data
uint16_t channels[16];
bool SBUS_failSafe;
bool SBUS_lostFrame;

// Variable to store the HTTP request
String header;

uint8_t motor_state = 0x00; // bitstring that defines the behavior of each motor driver via the Serial Register

void setMotor(uint8_t motor, int speed, uint8_t A, uint8_t B, bool rev)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  if (speed < -(DEADZONE))
  {
    SR.setNoUpdate(A, (HIGH^rev));
    SR.setNoUpdate(B, (LOW^rev));
  } else if (speed > DEADZONE)
  {
    SR.setNoUpdate(A, (LOW^rev));
    SR.setNoUpdate(B, (HIGH^rev));
  } else 
  {
    SR.setNoUpdate(A, (LOW));
    SR.setNoUpdate(B, (LOW));
  }
  // set the speed
  speed = abs(speed);
  analogWrite(motor, speed);
}

void setM1(int speed)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  setMotor(M1, speed, 2, 3, M1_REV);
}

void setM2(int speed)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  setMotor(M2, speed, 1, 4, M2_REV);
}

void setM3(int speed)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  setMotor(M3, speed, 7, 5, M3_REV);
}
void setM4(int speed)
// sets the speed and direction of a motor, given by the pin number of that motor.
{
  setMotor(M4, speed, 0, 6, M4_REV);
}

String latestDebugMessage = ""; // Store the most recent debug message

// Function to set the most recent debug message
void setDebugMessage(const String& message) {
  latestDebugMessage = message; // Overwrite the latest debug message
}

// Function to handle incoming client requests
void handleClient() {
  WiFiClient client = server.available(); // Check if a client has connected
  if (client) {
    // Wait for the client to send data
    while (client.connected() && !client.available()) {
      delay(1);
    }

    String request = client.readStringUntil('\r');
    client.flush();

    // Check if the request is for the debug message (AJAX request)
    if (request.indexOf("GET /debug") >= 0) {
      // Send only the most recent debug message
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.print(latestDebugMessage); // Send the most recent debug message
      client.stop();
      return;
    }

    // Serve the main HTML page
    String html = "<html><head><title>Debug Monitor</title></head><body>";
    html += "<h1>Debug Monitor</h1>";
    html += "<div id='debugOutput'>Loading...</div>";
    html += "<script>";
    html += "setInterval(() => {";
    html += "  fetch('/debug').then(response => response.text()).then(data => {";
    html += "    document.getElementById('debugOutput').innerText = data;";
    html += "  });";
    html += "}, 1000);"; // Fetch data every second
    html += "</script>";
    html += "</body></html>";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println("Content-Length: " + String(html.length()));
    client.println();
    client.print(html); // Send the HTML content
    client.stop();
  }
}

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// velocity values
int x_vel = 0;
int y_vel = 0;
int theta_vel = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting RASTICxHackH Spring 2025 Controller...");

  SBUS_RX.begin(SERIAL2_RX, SERIAL2_TX, SBUS_INVERT);
  
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

  setM1(0); // Set all the motor pins before attaching the servo, prevents double use of PWM generators
  setM2(0);
  setM3(0);
  setM4(0);

  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  tilt_servo.setPeriodHertz(50);    // standard 50 hz servo
	tilt_servo.attach(S1, 800, 2200);

  digitalWrite(SR_EN, LOW); // Activate the Serial register (Active Low)

  SR.setAllLow(); // Set all output pin of shift register to 0.

  // Connect to Wi-Fi network with SSID and password
  // Serial.print("Connecting to ");
  // Serial.println(ssid);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Print local IP address and start web server
  // Serial.println("");
  // Serial.println("WiFi connected.");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());

  // server.begin();
  // setDebugMessage("Starting loop...");
}

int16_t w_m1 = 0;
int16_t w_m2 = 0;
int16_t w_m3 = 0;
int16_t w_m4 = 0;
uint8_t tilt_pos = 90;    // variable to store the servo position

void loop()
{
  if(SBUS_RX.read(&channels[0], &SBUS_failSafe, &SBUS_lostFrame))
  {
    theta_vel = map(channels[3], 172, 1811, -255, 255);
    x_vel     = map(channels[2], 172, 1811, -255, 255);
    y_vel     = map(channels[1], 172, 1811, 255, -255);
    tilt_pos  = map(channels[0], 172, 1811, 30,   157);

    tilt_servo.write(tilt_pos);
    // Serial.println(tilt_pos);
    // delay(20);

    // inverse kinematics
    w_m1 = (x_vel + y_vel - theta_vel)/3;
    w_m2 = (x_vel - y_vel + theta_vel)/3;
    w_m3 = (x_vel + y_vel + theta_vel)/3;
    w_m4 = (x_vel - y_vel - theta_vel)/3;

    setM1(w_m1);
    setM2(w_m2);
    setM3(w_m3);
    setM4(w_m4);

    SR.updateRegisters();
  }
  
}