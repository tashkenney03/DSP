#!/usr/bin/env python3
import serial
import rtmidi
import time

# Configuration
SERIAL_PORT = '/dev/cu.usbmodem101'
BAUD_RATE = 31250

print("Connecting to Arduino...")

# Open serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
time.sleep(2)

# Flush any existing data
ser.reset_input_buffer()

# Create MIDI output
midiout = rtmidi.MidiOut()
available_ports = midiout.get_ports()

print("\nAvailable MIDI ports:")
for i, port in enumerate(available_ports):
    print(f"{i}: {port}")

# Try to open IAC Driver
iac_port = None
for i, port in enumerate(available_ports):
    if 'IAC' in port:
        iac_port = i
        break

if iac_port is not None:
    midiout.open_port(iac_port)
    print(f"\n✓ Connected to: {available_ports[iac_port]}")
else:
    midiout.open_virtual_port("Arduino MIDI")
    print("\n✓ Created virtual port: Arduino MIDI")

print(f"✓ Listening on {SERIAL_PORT}...")
print("Press your pads! (Ctrl+C to stop)\n")

# Forward MIDI messages with proper synchronization
try:
    while True:
        # Wait for a status byte (0x80-0xFF)
        byte = ser.read(1)
        if len(byte) == 0:
            continue
            
        status = byte[0]
        
        # Check if it's a valid MIDI status byte
        if status >= 0x80:
            # Read next 2 data bytes
            data1 = ser.read(1)
            data2 = ser.read(1)
            
            if len(data1) > 0 and len(data2) > 0:
                midi_msg = [status, data1[0], data2[0]]
                midiout.send_message(midi_msg)
                
                note_type = "ON " if status == 0x90 else "OFF"
                print(f"MIDI {note_type}: Note {data1[0]}, Velocity {data2[0]}")
                
except KeyboardInterrupt:
    print("\n\nStopping...")
    ser.close()
    del midiout
    print("Disconnected!")
