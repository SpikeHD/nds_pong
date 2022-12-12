#include <nds.h>
#include <gl2d.h>

#include <stdio.h>
#include <string.h>

volatile int frame = 0;

void Vblank() {
	frame++;
}

int main(void) {
	int playPosX = 0, playPosY = 0;
	int padSizeX = 5, padSizeY = 60;

	// Store touch position
	touchPosition touchXY;
	irqSet(IRQ_VBLANK, Vblank);

	videoSetMode(MODE_5_3D);

	// initialize gl2d
	glScreen2D();

	while(true) {
		glBegin2D();

		touchRead(&touchXY);

		if (touchXY.py != 0)
			playPosY = touchXY.py;

		glBoxFilled(10, 0 + playPosY, 10 + padSizeX, padSizeY + playPosY, 999999);

		glEnd2D();
		glFlush(0);

		swiWaitForVBlank();
	}
}