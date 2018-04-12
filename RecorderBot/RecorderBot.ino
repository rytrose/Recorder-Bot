#include <Servo.h>

#define SWITCH 12
#define FLOW_BUTTON 11
#define VALVE 10
#define BUTTON_1 9
#define FINGER_1 8
#define BUTTON_2 7
#define FINGER_2 6
#define BUTTON_3 5
#define FINGER_3 4
#define VIBRATO 3

Servo servo;

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

  servo.attach(3);
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
  digitalWrite(LED_BUILTIN, vibrato);
  
  if(state) readFromMax();
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
  while(Serial.available() > 0) {
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
  }
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
    Serial.println(115 + (50 * ((float)(millis() % vibratoFreq) / vibratoFreq)));
  }
  else servo.write(90);
}

