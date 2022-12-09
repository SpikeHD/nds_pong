#include <nds.h>
#include <stdio.h>

volatile int frame = 0;

void Vblank() {
	frame++;
}

int main(void) {
	iprintf("This is a test!");

	consoleDemoInit();

	// Store touch position
	touchPosition touchXY;
	irqSet(IRQ_VBLANK, Vblank);

	while(true) {
		swiWaitForVBlank();

		touchRead(&touchXY);

		iprintf("Touch X: %04X\n", touchXY.rawx);
		iprintf("Touch Y: %04X\n", touchXY.rawy);
		
		iprintf("Frame: %d\n", frame);
	}
}