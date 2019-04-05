# enterCode

This is a small toy for my children:
- they want a small keyboard to "simulate" a code to enter their room
- a green LED will light on when the code is correct (red if not)
- they will also be able to use a RFID key

It is powered by three AA battery, and should not consume too much (sleep mode)

![enterCode box](photos/IMG4238.JPG)

# Principle






# Bill of Material
- an AVR micro-controller (ATtiny 2313)
- a small 4x4 matrix keyboard (
- 3 AA 1.5v battery (and their holder)
- two LEDs (green and red)
- a plastic box
- a RC522 RFID reader and some RFID key
- four 7-segment displays, with TM1637 driver

# Some links
- [doc for the RC522 board](https://www.sunrom.com/p/mifare-rfid-readerwriter-1356mhz-rc522)

# To Do (not yet done, but will be done... one day in the future)
- go to sleep mode after 20 seconds (when not used)
- RFID part

