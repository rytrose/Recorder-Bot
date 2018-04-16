#include <Servo.h>

#define LIN_ACT_EN 13
#define LIN_ACT_IN1 12
#define LIN_ACT_IN2 8

#define FINGER_1 22
#define FINGER_2 23
#define FINGER_3 24
#define FINGER_4 25
#define FINGER_5 26
#define FINGER_6 27
#define FINGER_7 28
#define VALVE 29

#define VIBRATO 6
#define HEAD_SERVO 5

#define HEAD_RED 11
#define HEAD_GREEN 10
#define HEAD_BLUE 9

// Serial message variables
String msg_buffer;
String message_type;

// Servo objects
Servo vibrato_servo;
Servo head_servo;

// Valve open/closed
bool valve = false;

// U in/out or off
String u_state = "OFF";
bool u_in_1 = false;
bool u_in_2 = false;

// Color values
int red = 0;
int blue = 0;
int green = 0;

// Start time of current movement
unsigned long motorStartTime = 0;
// Time to wait for linear actuator to move
unsigned long motorWaitTime = 1200;

// Fingers open/closed
bool f_1 = false;
bool f_2 = false;
bool f_3 = false;
bool f_4 = false;
bool f_5 = false;
bool f_6 = false;
bool f_7 = false;

// Tempo (for vibrato purposes at the moment)
int mspb = 448;

// Vibrato state
bool vibrato = false;
// Start time of current articulation
unsigned long articulationStartTime = 0;
// Time to wait until vibrato starts
unsigned long vibratoWaitTime = mspb;

void setup() {
  Serial.begin(9600);

  // Solenoid
  pinMode(VALVE, OUTPUT);

  // Fingers
  pinMode(FINGER_1, OUTPUT);
  pinMode(FINGER_2, OUTPUT);
  pinMode(FINGER_3, OUTPUT);
  pinMode(FINGER_4, OUTPUT);
  pinMode(FINGER_5, OUTPUT);
  pinMode(FINGER_6, OUTPUT);
  pinMode(FINGER_7, OUTPUT);

  // Motor pins
  pinMode(LIN_ACT_EN, OUTPUT);
  pinMode(LIN_ACT_IN1, OUTPUT);
  pinMode(LIN_ACT_IN2, OUTPUT);

  // Head lights
  pinMode(HEAD_RED, OUTPUT);
  pinMode(HEAD_BLUE, OUTPUT);
  pinMode(HEAD_GREEN, OUTPUT);

  // For fun
  pinMode(LED_BUILTIN, OUTPUT);

  // Servos
  vibrato_servo.attach(VIBRATO);
  head_servo.attach(HEAD_SERVO);

  // Set motor in
  setU(false);
}

void loop() {
  // Read commands in from Max
  readFromMax();

  // Use vibrato
  vibratoHandler();

  // Write to all the fingers and valve
  digitalWrite(VALVE, valve);
  digitalWrite(FINGER_1, f_1);
  digitalWrite(FINGER_2, f_2);
  digitalWrite(FINGER_3, f_3);
  digitalWrite(FINGER_4, f_4);
  digitalWrite(FINGER_5, f_5);
  digitalWrite(FINGER_6, f_6);
  digitalWrite(FINGER_7, f_7);

  // Enable motor
  digitalWrite(LIN_ACT_EN, HIGH);
  // Write to motor
  digitalWrite(LIN_ACT_IN1, u_in_1);
  digitalWrite(LIN_ACT_IN2, u_in_2);

  // Write lights
  analogWrite(HEAD_RED, red);
  analogWrite(HEAD_GREEN, green);
  analogWrite(HEAD_BLUE, blue);

  // Handle motor on/off
  motorHandler();
}

// Read the serial port
void readFromMax() {
  while (Serial.available() > 0)
  {
    char inByte = Serial.read(); // Read the incoming byte.

    if (inByte != '\n') // If the byte is NOT an ASCII 10 \n character, add it to the buffer.
    {
      msg_buffer += inByte;
    }
    else // ... otherwise, if it is an ASCII 10 \n character, the buffer is full so decode it.
    {

      ///*** PUT NEW COMMANDS HERE ****///
      message_type = "valve"; // message type to test
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        int val = msg_buffer.toInt();
        // Update the valve state
        setValve(val);
        msg_buffer = ""; // Clear the buffer.
      }
      
      message_type = "fingers"; // message type to test
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        int val = msg_buffer.toInt();
        char valByte = (char) val;

        // Each bit in the byte represents a finger
        f_1 = (valByte & (1 << 7-1)) != 0;
        f_2 = (valByte & (1 << 6-1)) != 0;
        f_3 = (valByte & (1 << 5-1)) != 0;
        f_4 = (valByte & (1 << 4-1)) != 0;
        f_5 = (valByte & (1 << 3-1)) != 0;
        f_6 = (valByte & (1 << 2-1)) != 0;
        f_7 = (valByte & (1 << 1-1)) != 0; 
        
        msg_buffer = ""; // Clear the buffer.
      }

      message_type = "head";
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        int val = msg_buffer.toInt();
        if(val >= 0 && val <= 180){
          digitalWrite(LED_BUILTIN, HIGH);
          head_servo.write(val);
        }
        msg_buffer = ""; // Clear the buffer.
      }

      message_type = "motor"; 
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        int val = msg_buffer.toInt();
        setU(val);
        msg_buffer = ""; // Clear the buffer.
      }

      message_type = "red"; 
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        red = msg_buffer.toInt();
        msg_buffer = ""; // Clear the buffer.
      }

      message_type = "green"; 
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        green = msg_buffer.toInt();
        msg_buffer = ""; // Clear the buffer.
      }

      message_type = "blue";
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        blue = msg_buffer.toInt();
        msg_buffer = ""; // Clear the buffer.
      }
      
      msg_buffer = ""; // Clear the buffer.
    }
  }
}

void setValve(bool value) {
  // Mark 0->1 time for vibrato
  if(valve == 0 && value == 1) articulationStartTime = millis();

  // Clear vibrato and fingers
  if(valve == 1 && value == 0) {
    vibrato = false;
    f_1 = false;
    f_2 = false;
    f_3 = false;
    f_4 = false;
    f_5 = false;
    f_6 = false;
    f_7 = false;
  }

  valve = value;
}

int vibratoFreq = mspb / 3;
void vibratoHandler() {
  // If vibrato is off, check to see if it should be turned on
  if(valve && !vibrato && (millis() - articulationStartTime) > vibratoWaitTime) vibrato = true;

  if(vibrato) {
    vibrato_servo.write(90 + (90 * ((float)(millis() % vibratoFreq) / vibratoFreq)));
    //Serial.println(115 + (50 * ((float)(millis() % vibratoFreq) / vibratoFreq)));
  }
  else vibrato_servo.write(180);
}

void setU(bool value) {
  // Move motor out
  if(u_state != "OUT" && value == 1) {
    // Move the motor out
    u_in_1 = false;
    u_in_2 = true;
    u_state = "OUT";
    motorStartTime = millis();
  }

  // Move motor in
  if(u_state != "IN" && value == 0) {
    // Move the motors out
    u_in_1 = true;
    u_in_2 = false;
    u_state = "IN";
    motorStartTime = millis();
  }
}

void motorHandler() {
  if(u_state != "OFF" && (millis() - motorStartTime) > motorWaitTime) {
    u_in_1 = false;
    u_in_2 = false;
    u_state = "OFF";
  }
}

void demoOne()
{
  // this function will run the motors in both directions at a fixed speed
  // turn on motor A
  digitalWrite(LIN_ACT_IN1, HIGH);
  digitalWrite(LIN_ACT_IN2, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(LIN_ACT_EN, 255);
  delay(1200);
  // now change motor directions
  digitalWrite(LIN_ACT_IN1, LOW);
  digitalWrite(LIN_ACT_IN2, HIGH); 
  delay(1200);
  // now turn off motors
  digitalWrite(LIN_ACT_IN1, LOW);
  digitalWrite(LIN_ACT_IN2, LOW); 
}

void demoTwo()
{
  // this function will run the motors across the range of possible speeds
  // note that maximum speed is determined by the motor itself and the operating voltage
  // the PWM values sent by analogWrite() are fractions of the maximum speed possible 
  // by your hardware
  // turn on motors
  digitalWrite(LIN_ACT_IN1, LOW);
  digitalWrite(LIN_ACT_IN2, HIGH);  
  // accelerate from zero to maximum speed
  for (int i = 0; i < 256; i++)
  {
    analogWrite(LIN_ACT_EN, i);
    delay(20);
  } 
  // decelerate from maximum speed to zero
  for (int i = 255; i >= 0; --i)
  {
    analogWrite(LIN_ACT_EN, i);
    delay(20);
  } 
  // now turn off motors
  digitalWrite(LIN_ACT_IN1, LOW);
  digitalWrite(LIN_ACT_IN2, LOW);  
}

// Fires valve and the fingers twice each, sequentially
void demoThree()
{
   digitalWrite(VALVE, HIGH);
   delay(200);
   digitalWrite(VALVE, LOW);
   delay(200);
   digitalWrite(VALVE, HIGH);
   delay(200);
   digitalWrite(VALVE, LOW);
   delay(200);

   digitalWrite(FINGER_1, HIGH);
   delay(200);
   digitalWrite(FINGER_1, LOW);
   delay(200);
   digitalWrite(FINGER_1, HIGH);
   delay(200);
   digitalWrite(FINGER_1, LOW);
   delay(200);

   digitalWrite(FINGER_2, HIGH);
   delay(200);
   digitalWrite(FINGER_2, LOW);
   delay(200);
   digitalWrite(FINGER_2, HIGH);
   delay(200);
   digitalWrite(FINGER_2, LOW);
   delay(200);

   digitalWrite(FINGER_3, HIGH);
   delay(200);
   digitalWrite(FINGER_3, LOW);
   delay(200);
   digitalWrite(FINGER_3, HIGH);
   delay(200);
   digitalWrite(FINGER_3, LOW);
   delay(200);

   digitalWrite(FINGER_4, HIGH);
   delay(200);
   digitalWrite(FINGER_4, LOW);
   delay(200);
   digitalWrite(FINGER_4, HIGH);
   delay(200);
   digitalWrite(FINGER_4, LOW);
   delay(200);

   digitalWrite(FINGER_5, HIGH);
   delay(200);
   digitalWrite(FINGER_5, LOW);
   delay(200);
   digitalWrite(FINGER_5, HIGH);
   delay(200);
   digitalWrite(FINGER_5, LOW);
   delay(200);

   digitalWrite(FINGER_6, HIGH);
   delay(200);
   digitalWrite(FINGER_6, LOW);
   delay(200);
   digitalWrite(FINGER_6, HIGH);
   delay(200);
   digitalWrite(FINGER_6, LOW);
   delay(200);

   digitalWrite(FINGER_7, HIGH);
   delay(200);
   digitalWrite(FINGER_7, LOW);
   delay(200);
   digitalWrite(FINGER_7, HIGH);
   delay(200);
   digitalWrite(FINGER_7, LOW);
   delay(200);
}

// Fires all fingers and the valve
void demoFour() 
{
  digitalWrite(FINGER_1, HIGH);
  digitalWrite(FINGER_2, HIGH);
  digitalWrite(FINGER_3, HIGH);
  digitalWrite(FINGER_4, HIGH);
  digitalWrite(FINGER_5, HIGH);
  digitalWrite(FINGER_6, HIGH);
  digitalWrite(FINGER_7, HIGH);
  digitalWrite(VALVE, HIGH);
  delay(1000);
  
  digitalWrite(FINGER_1, LOW);
  digitalWrite(FINGER_2, LOW);
  digitalWrite(FINGER_3, LOW);
  digitalWrite(FINGER_4, LOW);
  digitalWrite(FINGER_5, LOW);
  digitalWrite(FINGER_6, LOW);
  digitalWrite(FINGER_7, LOW);
  digitalWrite(VALVE, LOW);
  delay(1000);
}
