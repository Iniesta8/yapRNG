/*
    This file is part of yapRNG library.
    Please check the README file and the notes inside the yapRNG.h file
*/

// include required files
#include "yapRNG.h"
#include <avr/wdt.h>

// global settings - modify them to change the yapRNG characteristics
volatile byte _prngPool[PRNG_POOL_SIZE]; // random pool
volatile byte _prngPointer = 0; // points to the current cell of the pool
volatile byte _prngBitPointer = 0; // points to the current bit of the cell being used

// class constructor
yapRNG::yapRNG(void) {
  // we ensure that the memory cells that will contain the pool are empty
  for (byte _tempI = 0; _tempI < PRNG_POOL_SIZE; _tempI++) {
    _prngPool[_tempI] = 0;
  }
  _setWDT(); // initialize the WDT
}

// return a random byte (8-bits) from the pool
uint8_t yapRNG::nextByte() {
  uint8_t _result;

  while (_prngPointer < 1) {
  } // value is not ready yet, wait...

  SREG &= ~(1 << SREG_I); // disable interrupts to access the pool securely
  _result = _prngPool[0]; // get the first byte
  // shift the bytes of the pool to the left to full the empty cell
  for (uint8_t _tempPointer = 0; _tempPointer < (PRNG_POOL_SIZE - 1);
       _tempPointer++) {
    _prngPool[_tempPointer] = _prngPool[_tempPointer + 1];
  }
  _prngPool[PRNG_POOL_SIZE - 1] = 0;
  // clear the last cell - we don't want to insert recursion into the pool
  _prngPool[_prngPointer] = 0;
  _prngPointer--;
  SREG |= (1 << SREG_I); // reactivate the interrupts
  return _result;
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
void yapRNG::_setWDT() {
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
  volatile static unsigned long _lfsr = 1; // LFSR starting value
  volatile static unsigned long _polynomial;

  /* taps: 32 30 29 23; feedback polynomial: x^32 + x^30 + x^29 + x^23 + 1 */
  _polynomial = ((_lfsr >> 0) ^ (_lfsr >> 2) ^ (_lfsr >> 3) ^ (_lfsr >> 9)) & 1;
  _lfsr = (_lfsr >> 1) | (_polynomial << 31);

// XOR between the 1st bit of the LFSR and the 1st bit of the TCNT1L register
// (or the TCNT0 register, if the MCU has just 1 timer), then put the result
// into a bit of the pool
#ifdef TCNT1L
  if ((_lfsr & 1) ^ (TCNT1L & 1)) {
#else
  if ((_lfsr & 1) ^ (TCNT0 & 1)) {
#endif
    _prngPool[_prngPointer] |= (1 << _prngBitPointer);
  } else {
    _prngPool[_prngPointer] &= ~(1 << _prngBitPointer);
  }
  // increment the bit's pointer to distribute the entropy inside the whole
  // pool
  if (++_prngBitPointer > 7) {
    _prngBitPointer = 0;
    if (++_prngPointer == PRNG_POOL_SIZE) {
      _prngPointer = 0;
    }
  }
}
