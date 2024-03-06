# ESP32 30x30 LED Arcade Board with WIFI and Custom Games

University Project in Embedded Programming. Task: Wire 30 strips of 30 LEDs in series connected to an ESP32 Microcontroller at a single port. Create a multiplayer arcade machine with 3 games, which can be connected to with WIFI.

My focus was on the machine's physical layer. My tasks:
1) Provide the game programmers with an easy to use framework for manipulating the 30x30 matrix of LEDs.
2) Provide a playable refresh rate of 20H or 50ms. As the LED chain works in series, the longer the chain, the longer it takes for data to reach the last LED in the chain.

Reaching that goal:
1) The chosen LEDs..

6) The ESP32 is equipped a Remote Control Transmitter (RMT)provides an RMT unit
1) Find a reliable, fast and efficient method for  
Controlling the LEDs. Ensuring the Microcontroller can update the LEDs fast enough to create a real gaming experience.
Find a realiable method for lighting the LED strip, while meeting timing closure.
