import serial
import rtmidi

SERIAL_PORT = '/dev/cu.usbmodem101'
BAUD_RATE = 31250

midiout = rtmidi.MidiOut()
available_ports = midiout.get_ports()
print(available_ports)

for i, port in enumerate(available_ports):
    if 'IAC' in port:
        midiout.open_port(i)
        print(f"Opened: {port}")
        break

ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
print("Bridge running")

while True:
    if ser.in_waiting >= 3:
        msg = ser.read(3)
        midiout.send_message(list(msg))