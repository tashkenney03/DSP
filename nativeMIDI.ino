// === 8-PAD MIDI CONTROLLER - NATIVE MIDI ===
// Sends actual MIDI bytes (no bridge software needed!)

const int NUM_PADS = 8;

// Button pins (D2-D9)
const int buttonPins[NUM_PADS] = {2, 3, 4, 5, 6, 7, 8, 9};

// LED pins
const int ledPins[NUM_PADS] = {13, 12, 11, 10, A5, A4, A3, A2};

// MIDI notes - C Major scale
const int midiNotes[NUM_PADS] = {60, 62, 64, 65, 67, 69, 71, 72};

const int POT_PIN = A0;
const int MIDI_CHANNEL = 1;  // MIDI channel 1

bool lastButtonState[NUM_PADS];
bool buttonPressed[NUM_PADS];

const unsigned long LED_ON_MS = 80;

void setup() {
  for (int i = 0; i < NUM_PADS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonState[i] = HIGH;
    buttonPressed[i] = false;
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
 
  Serial.begin(31250);  // MIDI baud rate
}

void sendMIDI(byte command, byte data1, byte data2) {
  Serial.write(command);
  Serial.write(data1);
  Serial.write(data2);
}

void loop() {
  int pot = analogRead(POT_PIN);
  int velocity = map(pot, 0, 1023, 30, 127);
 
  for (int i = 0; i < NUM_PADS; i++) {
    bool currentState = digitalRead(buttonPins[i]);
   
    if (currentState == LOW && lastButtonState[i] == HIGH && !buttonPressed[i]) {
      buttonPressed[i] = true;
     
      digitalWrite(ledPins[i], HIGH);
     
      // Send MIDI Note On (0x90 = Note On, channel 1)
      sendMIDI(0x90, midiNotes[i], velocity);
     
      delay(LED_ON_MS);
      digitalWrite(ledPins[i], LOW);
     
      // Send MIDI Note Off (0x80 = Note Off)
      sendMIDI(0x80, midiNotes[i], 0);
    }
   
    if (currentState == HIGH) {
      buttonPressed[i] = false;
    }
   
    lastButtonState[i] = currentState;
  }
 
  delay(5);
}