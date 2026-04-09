// === 8-PAD MIDI CONTROLLER - OPTIMIZED ===

const int NUM_PADS = 8;
const int buttonPins[NUM_PADS] = {2, 3, 4, 5, 6, 7, 8, 9};
const int ledPins[NUM_PADS] = {13, 12, 11, 10, A5, A4, A3, A2};
const int midiNotes[NUM_PADS] = {60, 62, 64, 65, 67, 69, 71, 72};
const int POT_PIN = A0;
const int MIDI_CHANNEL = 0;  // Channel 1 (0-indexed)

bool lastButtonState[NUM_PADS];
bool noteOn[NUM_PADS];
const unsigned long LED_ON_MS = 80;

void setup() {
  for (int i = 0; i < NUM_PADS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    lastButtonState[i] = HIGH;
    noteOn[i] = false;
  }
  
  Serial.begin(31250);
  delay(100);  // Give serial time to initialize
}

void sendMIDI(byte cmd, byte data1, byte data2) {
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
  Serial.flush();  // Make sure data is sent immediately
}

void loop() {
  int pot = analogRead(POT_PIN);
  int velocity = constrain(map(pot, 0, 1023, 30, 127), 1, 127);
  
  for (int i = 0; i < NUM_PADS; i++) {
    bool currentState = digitalRead(buttonPins[i]);
    
    // Button pressed (HIGH to LOW transition)
    if (currentState == LOW && lastButtonState[i] == HIGH && !noteOn[i]) {
      noteOn[i] = true;
      digitalWrite(ledPins[i], HIGH);
      
      // Send Note ON
      sendMIDI(0x90 | MIDI_CHANNEL, midiNotes[i], velocity);
      
      delay(10);  // Small delay for debounce
    }
    
    // Button released (LOW to HIGH transition)
    if (currentState == HIGH && lastButtonState[i] == LOW && noteOn[i]) {
      noteOn[i] = false;
      digitalWrite(ledPins[i], LOW);
      
      // Send Note OFF
      sendMIDI(0x80 | MIDI_CHANNEL, midiNotes[i], 0);
      
      delay(10);  // Small delay for debounce
    }
    
    lastButtonState[i] = currentState;
  }
  
  delay(1);  // Small loop delay
}