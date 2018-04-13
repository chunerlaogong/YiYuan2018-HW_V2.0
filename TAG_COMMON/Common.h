#ifndef _COMMON__H__
#define _COMMON__H__

#include "stm8l15x_conf.h"
#include "Type.h"



#define _EINT() asm("rim")
#define _DINT()	asm("sim")
#define _WAIT()	asm("wfi")
#define _HALT()	asm("halt")

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define COAP_TEST 1
void minidelay(uint8_t cc);
void delay(uint32_t nn);
char *itoa(int num, char *str, int radix);
#endif