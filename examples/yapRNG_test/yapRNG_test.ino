/*
This is a simple sketch to demonstrate the use of yapRNG, the
<y>et <a>nother <p>seudo <R>andom <N>umber <G>enerator

yapRNG is a simple pseudo-random number generator. It uses a mechanism based
on an interrupt raised by the WatchDog Timer of the microcontroller to
collect entropy and a Fibonacci 32 bit LFSR (Linear Feedback Shift Register)
to distribuite it into a pool.

More info on the mechanism of the entropy collector and the methods
and their usage can be found into the README file.

Written by Andi Schnebinger.
Inspired by Leonardo Miliani <leonardo AT leonardomiliani DOT com> and his
Galois LFSR based pRNG library.

This code and the yapRNG library are free software; you can redistribute
and/or modify them under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3.0 of the License,
or (at your option) any later version.

The yapRNG library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

WARNING - IMPORTANT ADVICE FOR ALL THE USERS OF yapRNG

yapRNG is NOT intended to be used in critical applications where a REAL
random number generator is required because the mechanism used by
this library does NOT guarantee that the sequence won't contain repeating
patterns. If you need a more secure algorithm, try looking something else.

*/

// library inclusion
#include "yapRNG.h"
yapRNG prng; // new instance of the class pRNG

// setup
void setup() {
  Serial.begin(9600); // let's open the serial
  delay(2000);
}

// main loop
void loop() {
  Serial.println(prng.nextLong()); // print an unsigned long random number
  delay(1000);
  Serial.println(prng.nextInt()); // print an unsigned int random number
  delay(1000);
  Serial.println(prng.nextByte()); // print a byte random number
  delay(1000);
}
