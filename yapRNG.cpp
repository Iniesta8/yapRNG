/*
    This file is part of yapRNG library.
    Please check the README file and the notes inside the yapRNG.h file
*/

// include required files
#include "yapRNG.h"
#include <avr/wdt.h>

// global settings - modify them to change the yapRNG characteristics
volatile byte prngPool[PRNG_POOL_SIZE]; // random pool
volatile byte prngPointer = 0; // points to the current cell of the pool
volatile byte prngBitPointer = 0; // points to the current bit of the cell being used

// class constructor
yapRNG::yapRNG() {
  // we ensure that the memory cells that will contain the pool are empty
  for (byte i = 0; i < PRNG_POOL_SIZE; ++i) {
    prngPool[i] = 0;
  }
  setWDT(); // initialize the WDT
}

// return a random byte (8-bits) from the pool
uint8_t yapRNG::nextByte() {
  uint8_t result;

  while (prngPointer < 1) {
  } // value is not ready yet, wait...

  SREG &= ~(1 << SREG_I); // disable interrupts to access the pool securely
  result = prngPool[0]; // get the first byte
  // shift the bytes of the pool to the left to full the empty cell
  for (uint8_t tempPointer = 0; tempPointer < (PRNG_POOL_SIZE - 1);
       tempPointer++) {
    prngPool[tempPointer] = prngPool[tempPointer + 1];
  }
  prngPool[PRNG_POOL_SIZE - 1] = 0;
  // clear the last cell - we don't want to insert recursion into the pool
  prngPool[prngPointer] = 0;
  prngPointer--;
  SREG |= (1 << SREG_I); // reactivate the interrupts
  return result;
}

// return an unsinged int (16-bits) from the pool
uint16_t yapRNG::nextInt() {
  return (((uint16_t)(nextByte()) << 8) | nextByte());
}

// return an unsinged long (32-bits) from the pool
uint32_t yapRNG::nextLong() {
  return (((uint32_t)(nextInt()) << 16) | nextInt());
}

/*
************************************************************
WARNING!! DO NOT MODIFY THE FOLLOWING CODE IF YOU DON'T KNOW
WHAT YOU'RE DOING! YOU COULD PUT YOUR MICROCONTROLLER IN A
NEVERENDING RESET!!
************************************************************
*/

// set the WatchDog Timer
void yapRNG::setWDT() {
  MCUSR = 0;              // ensure that reset vectors are off
  wdt_disable();          // disable WDT
  SREG &= ~(1 << SREG_I); // disable all interrupts

  // set WDT to raise an interrupt every 16 ms
  _WD_CONTROL_REG = ((1 << _WD_CHANGE_BIT) | (1 << WDE));
  _WD_CONTROL_REG = (1 << WDIE);

  SREG |= (1 << SREG_I); // re-enable interrupts
}

// main core of the algorithm - this function collects entropy
// from timer 1 and redistribuite it using a Fibonacci 32-bits LFSR
// into a pool
ISR(WDT_vect) {
  volatile static unsigned long lfsr = 1; // LFSR starting value
  volatile static unsigned long polynomial;

  /* taps: 32 30 29 23; feedback polynomial: x^32 + x^30 + x^29 + x^23 + 1 */
  polynomial = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 9)) & 1;
  lfsr = (lfsr >> 1) | (polynomial << 31);

// XOR between the 1st bit of the LFSR and the 1st bit of the TCNT1L register
// (or the TCNT0 register, if the MCU has just 1 timer), then put the result
// into a bit of the pool
#ifdef TCNT1L
  if ((lfsr & 1) ^ (TCNT1L & 1)) {
#else
  if ((lfsr & 1) ^ (TCNT0 & 1)) {
#endif
    prngPool[prngPointer] |= (1 << prngBitPointer);
  } else {
    prngPool[prngPointer] &= ~(1 << prngBitPointer);
  }
  // increment the bit's pointer to distribute the entropy inside the whole
  // pool
  if (++prngBitPointer > 7) {
    prngBitPointer = 0;
    if (++prngPointer == PRNG_POOL_SIZE) {
      prngPointer = 0;
    }
  }
}
