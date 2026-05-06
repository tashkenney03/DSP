const int buttonPins[] = {A0, A1, A2, A3, 2};
const int midiNotes[] = {60, 62, 64, 65, 67};
const int trigPin = 3;
const int echoPin = 4;
const int joyX = A4;
const int joyY = A5;
const int joySW = 5;

bool lastState[5];
bool lastSW = HIGH;
int octaveShift = 0;

void setup() {
  Serial.begin(31250);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(joySW, INPUT_PULLUP);
  for (int i = 0; i < 5; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastState[i] = HIGH;
  }
}

void noteOn(int note, int velocity) {
  Serial.write(0x90);
  Serial.write(note);
  Serial.write(velocity);
}

void noteOff(int note) {
  Serial.write(0x80);
  Serial.write(note);
  Serial.write(0);
}

void sendCC(int channel, int cc, int value) {
  Serial.write(0xB0 | channel);
  Serial.write(cc);
  Serial.write(value);
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration * 0.034 / 2;
}

void loop() {
  // Joystick click shift octave
  bool swPressed = digitalRead(joySW) == LOW;
  if (swPressed && lastSW == HIGH) {
    octaveShift += 12;
    if (octaveShift > 24) octaveShift = -24;
  }
  lastSW = swPressed ? LOW : HIGH;
  

  // Joystick X → CC73 (attack)
  int xVal = analogRead(joyX);
  int ccX = map(xVal, 0, 1023, 0, 127);
  static int lastCCX = -1;
  if (abs(ccX - lastCCX) > 2) {
    sendCC(0, 73, ccX);
    lastCCX = ccX;
  }

  // Joystick Y → CC71 (resonance)
  int yVal = analogRead(joyY);
  int ccY = map(yVal, 0, 1023, 0, 127);
  static int lastCCY = -1;
  if (abs(ccY - lastCCY) > 2) {
    sendCC(0, 71, ccY);
    lastCCY = ccY;
  }

  // Ultrasonic → CC74 (filter cutoff)
  int distance = getDistance();
  if (distance > 2 && distance < 43) {
    int vol = map(distance, 2, 43, 127, 0);
    vol = constrain(vol, 0, 127);
    sendCC(0, 74, vol);
  }

  // Buttons
  for (int i = 0; i < 5; i++) {
    bool pressed = digitalRead(buttonPins[i]) == LOW;
    if (pressed && lastState[i] == HIGH) {
      int note = constrain(midiNotes[i] + octaveShift, 0, 127);
      noteOn(note, 100);
    }
    if (!pressed && lastState[i] == LOW) {
      int note = constrain(midiNotes[i] + octaveShift, 0, 127);
      noteOff(note);
    }
    lastState[i] = pressed ? LOW : HIGH;
  }
}