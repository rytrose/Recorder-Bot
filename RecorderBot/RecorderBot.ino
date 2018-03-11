#define SWITCH 12
#define FLOW_BUTTON 11
#define VALVE 10
#define BUTTON_1 9
#define FINGER_1 8
#define BUTTON_2 7
#define FINGER_2 6
#define BUTTON_3 5
#define FINGER_3 4

void setup() {

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

  Serial.begin(9600);
}

bool state = false;
int mspb = 448;

void loop() {
  state = digitalRead(SWITCH);
  digitalWrite(LED_BUILTIN, state);

  if(state) playSandstorm();
  else {
    // Read/write fingers
    digitalWrite(FINGER_1, digitalRead(BUTTON_1));
    digitalWrite(FINGER_2, digitalRead(BUTTON_2));
    digitalWrite(FINGER_3, digitalRead(BUTTON_3));
    // Read/write valve
    digitalWrite(VALVE, digitalRead(FLOW_BUTTON));\
  }
  
}

void playSandstorm() {
  for(int i = 0; i < 2; i++) if(digitalRead(SWITCH)) part1(); else state = false;
  for(int i = 0; i < 4; i++) if(digitalRead(SWITCH)) part2(); else state = false;
  for(int i = 0; i < 8; i++) if(digitalRead(SWITCH)) part3(); else state = false;
  for(int i = 0; i < 16; i++) if(digitalRead(SWITCH)) part4(); else state = false;
}

void part1() {
  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb * 0.5);
  digitalWrite(VALVE, LOW);
  delay(mspb * 0.5);
  delay(mspb * 6);

  digitalWrite(VALVE, HIGH);
  delay(mspb * 0.8);
  digitalWrite(VALVE, LOW);
  delay(mspb * 0.2);
}

void part2() {
  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb * 0.5);
  digitalWrite(VALVE, LOW);
  delay(mspb * 0.5);
}

void part3() {
  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb / 8);
  digitalWrite(VALVE, LOW);
  delay(mspb / 8);

  digitalWrite(VALVE, HIGH);
  delay(mspb * 0.25);
  digitalWrite(VALVE, LOW);
  delay(mspb * 0.25);
}

void part4() {
  digitalWrite(VALVE, HIGH);
  delay(mspb * 0.5);
  digitalWrite(VALVE, LOW);
  delay(mspb * 0.25);

  digitalWrite(VALVE, HIGH);
  delay(mspb * 0.5);
  digitalWrite(VALVE, LOW);
  delay(mspb * 0.25);

  digitalWrite(VALVE, HIGH);
  delay(mspb * 0.25);
  digitalWrite(VALVE, LOW);
  delay(mspb * 0.25);
}

