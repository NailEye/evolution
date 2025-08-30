/********************************************
 Odotukseen liittyvät määritykset
*********************************************/
#pragma once

#include "common.h"

#define NOP     asm volatile("nop");

static inline void cycle_wait(uint32_t count) {
    // Odotetaan countin verran syklejä
    // TODO: Siirretään arkkitehturin alle, koska on nyt ARM specifi
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : "=r"(count): [count]"0"(count) : "cc");
}

