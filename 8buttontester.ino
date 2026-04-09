const int dataPin = 11;
const int latchPin = 12;
const int clockPin = 13;
const int buttonPins[] = {A0, A1, A2, A3, A4, A5, 0, 1};

byte ledState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  for (int i = 0; i < 8; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void updateLEDs() {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, ledState);
  digitalWrite(latchPin, HIGH);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      bitSet(ledState, i);
      Serial.print("Button ");
      Serial.print(i + 1);
      Serial.println(" pressed");
    } else {
      bitClear(ledState, i);
    }
  }
  updateLEDs();
  delay(10);
}