/*
 * globalDefines.h
 *
 * Created: 22.09.2019 15:19:57
 *  Author: Michael
 */ 


#ifndef GLOBALDEFINES_H_
#define GLOBALDEFINES_H_

#define F_CPU 16000000L

// helper functions (macros) to support bit operations
#define sbi(PORT, bit) (PORT |= (1 << bit))  // set bit in PORT
#define cbi(PORT, bit) (PORT &= ~(1 << bit)) // clear bit in PORT
#define tgl(PORT, bit) (PORT ^= (1 << bit))  // switch bit in PORT

#endif /* GLOBALDEFINES_H_ */