/*
        yapRNG.h - <y>et <a>nother <p>seudo <R>andom <N>umber <G>enerator

    yapRNG is a simple pseudo-random number generator. It uses
    a mechanism based on an interrupt raised by the WatchDog
    Timer of the microcontroller to collect entropy and a
    Fibonacci 32 bit LFSR (Linear Feedback Shift Register) to
    distribuite it into a pool.

    ** IMPORTANT: yapRNG is NOT intended for use in critical applications!! **

    For more infos, please read the README.md file.

    Written by Andi Schnebinger.
    Inspired by Leonardo Miliani <www DOT leonardomiliani DOT com>.
    He wrote a library using a Galois LFSR to generate random bits.
    The latest version of his library can be found at:
    https://github.com/leomil72

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU General Public
        License as published by the Free Software Foundation; either
        version 3.0 of the License, or (at your option) any later version.

    You should have received a copy of the GNU General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*/

#ifndef yapRNG_H
#define yapRNG_H

// library version
#define yapRNG_VERSION 100

// this library is compatible both with Arduino <=0023 and Arduino >=100
#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// check MCU compatibility - yapRNG won't work on Atmega8
#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__)
#error Sorry, this MCU is not supported (lack of interrupt vector for WDT)!
#endif

// set the size of the pool depending on the amount of SRAM
#ifdef RAMEND
#if (RAMEND < 512)
#define PRNG_POOL_SIZE 8
#elif (RAMEND < 1024)
#define PRNG_POOL_SIZE 12
#else
#define PRNG_POOL_SIZE 16
#endif
#else
#define PRNG_POOL_SIZE 8
#endif

class yapRNG {
public:
  yapRNG();
  uint8_t nextByte(void);
  uint16_t nextInt(void);
  uint32_t nextLong(void);

private:
  void setWDT();
};

#endif
