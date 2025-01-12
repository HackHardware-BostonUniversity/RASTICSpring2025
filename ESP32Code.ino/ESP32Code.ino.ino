/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Variables to store the current output state
bool isMoveLeftOn = 0;
bool isMoveRightOn = 0;
bool isMoveForwardOn = 0;
bool isMoveBackwardOn = 0;
bool isRotateLeftOn = 0;
bool isRotateRightOn = 0;

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Motion Functions
void MoveLeft(){

  return;
}
void MoveRight(){

  return;
}
void MoveForward(){

  return;
}
void MoveBackward(){

  return;
}
void RotateLeft(){

  return;
}
void RotateRight(){

  return;
}
void Stationary(){

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
  Serial.println("Starting RASTICxHackH Spring 2025 Controller...")
  
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  

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
