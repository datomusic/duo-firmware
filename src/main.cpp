#include "WProgram.h"

extern "C" 
int main(void) {
#ifdef USING_MAKEFILE



#else
	// Pretend to be an Arduino by calling setup() and loop()
	setup();
	while (1) {
		loop();
		yield();
	}

#endif
}

