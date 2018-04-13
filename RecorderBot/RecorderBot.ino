#include <Servo.h>

#define VIBRATO 3
#define FINGER_3 4
#define HEAD_SERVO 9
#define FINGER_2 6
#define BUTTON_2 7
#define FINGER_1 8
#define BUTTON_1 5
#define VALVE 10
#define FLOW_BUTTON 11
#define SWITCH 12
#define BUTTON_3 13

String msg_buffer;
String message_type;
Servo servo;
Servo head_servo;

void setup() {
  Serial.begin(9600);

  // Solenoid
  pinMode(VALVE, OUTPUT);
  pinMode(FLOW_BUTTON, INPUT);
  
  // Manual vs. Sequenced Mode
  pinMode(SWITCH, INPUT);

  // Fingers
  pinMode(BUTTON_1, INPUT);
  pinMode(FINGER_1, OUTPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(FINGER_2, OUTPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(FINGER_3, OUTPUT);

  // For fun
  pinMode(LED_BUILTIN, OUTPUT);


  // Servos
  servo.attach(VIBRATO);
  head_servo.attach(HEAD_SERVO);
}

// Button pressing (0) or reading from Max (1) states
bool state = false;

// Valve open/closed
bool valve = false;

// Fingers open/closed
bool f_1 = false;
bool f_2 = false;
bool f_3 = false;

// Tempo (for vibrato purposes at the moment)
int mspb = 448;

// Vibrato state
bool vibrato = false;
// Start time of current articulation
unsigned long articulationStartTime = 0;
// Time to wait until vibrato starts
unsigned long vibratoWaitTime = mspb;

void loop() {
  state = digitalRead(SWITCH);
  //digitalWrite(LED_BUILTIN, vibrato);
  
  if(true) readFromMax();
  else {
    // Read/write fingers
    digitalWrite(FINGER_1, !digitalRead(BUTTON_1));
    digitalWrite(FINGER_2, !digitalRead(BUTTON_2));
    digitalWrite(FINGER_3, !digitalRead(BUTTON_3));
    
    // Read/write valve
    setValve(digitalRead(FLOW_BUTTON));
    digitalWrite(VALVE, valve);
  }

  // Use vibrato
  vibratoHandler();
}

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
      message_type = "finger"; // message type to test
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        int val = msg_buffer.toInt();
        msg_buffer = ""; // Clear the buffer.
      }

      message_type = "head"; // message type to test
      if(msg_buffer.startsWith(message_type)){
        msg_buffer.replace((message_type + " "), ""); // get rid of message prefix
        int val = msg_buffer.toInt();
        //Serial.println(val);
        if(val >= 0 && val <= 180){
          digitalWrite(LED_BUILTIN, HIGH);
          head_servo.write(val);
        }
        msg_buffer = ""; // Clear the buffer.
      }
      
      msg_buffer = ""; // Clear the buffer.
    }
  }
  /*while(Serial.available() > 0) {
    int val = Serial.parseInt();
    char b = (char) val;
    
    if (Serial.read() == '\n') {
      if(val < 2 && val > -1) {
        setValve(!val);
        digitalWrite(VALVE, valve);
      }
      else {     
        // Set fingers
        f_1 = (b & (1 << 7-1)) != 0;
        f_2 = (b & (1 << 6-1)) != 0;
        f_3 = (b & (1 << 5-1)) != 0;
        digitalWrite(FINGER_1, f_1);
        digitalWrite(FINGER_2, f_2);
        digitalWrite(FINGER_3, f_3); 
      }
    } 
  }*/
}

void setValve(bool value) {
  // Mark 0->1 time for vibrato
  if(valve == 0 && value == 1) articulationStartTime = millis();

  // Clear vibrato
  if(valve == 1 && value == 0) vibrato = false;

  valve = value;
}

int vibratoFreq = mspb / 3;
void vibratoHandler() {
  // If vibrato is off, check to see if it should be turned on
  if(valve && !vibrato && (millis() - articulationStartTime) > vibratoWaitTime) vibrato = true;

  if(vibrato) {
    servo.write(115 + (50 * ((float)(millis() % vibratoFreq) / vibratoFreq)));
    //Serial.println(115 + (50 * ((float)(millis() % vibratoFreq) / vibratoFreq)));
  }
  else servo.write(90);
}

