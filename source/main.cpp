#include <nds.h>
#include <gl2d.h>

#include <stdio.h>
#include <string.h>

// SimpleNFL namespace
using namespace SNF;

#define SCR_MAX_X 250
#define SCR_MAX_Y 190

volatile int frame = 0;

void Vblank() {
	frame++;
}

bool between(int num, int min, int max) {
	return min <= num && num <= max;
}

bool leftUp() {
	return getKeys(KEY_UP, KeyPhase::held);
}

bool leftDown() {
	return getKeys(KEY_DOWN, KeyPhase::held);
}

bool rightUp() {
	return getKeys(KEY_X, KeyPhase::held);
}

bool rightDown() {
	return getKeys(KEY_B, KeyPhase::held);
}

int main(void) {
	int playPosX = 10, playPosY = 0;
	int opPosX = SCR_MAX_X - 10, opPosY = 0;
	int padSizeX = 5, padSizeY = 60;
	int ballX = 0, ballY = 0;
	int ballSizeX = 10, ballSizeY = 10;

	// Core velocity is just the speed
	int coreVel = 2;
	// Velocity starts as down and towards the player
	int ballVelX = -coreVel, ballVelY = coreVel;

	// Store touch position
	touchPosition touchXY;
	irqSet(IRQ_VBLANK, Vblank);

	videoSetMode(MODE_5_3D);

	// initialize gl2d
	glScreen2D();

	// Start the ball at the middle of the screen
	ballX = SCR_MAX_X / 2;
	ballY = SCR_MAX_Y / 2;

	while(true) {
		glBegin2D();

		touchRead(&touchXY);

		// Button inputs
		if (leftUp()) {
			playPosY -= 5;
		}

		if (leftDown()) {
			playPosY += 5;
		}

		if (rightUp()) {
			opPosY -= 5;
		}

		if (rightDown()) {
			opPosY += 5;
		}

		// Prevent paddle resetting 
		if (touchXY.py != 0)
			playPosY = touchXY.py;

		// Prevent paddle going offscreen
		if (playPosY > SCR_MAX_Y - padSizeY)
			playPosY = SCR_MAX_Y - padSizeY;

		// Draw player
		glBoxFilled(playPosX, 0 + playPosY, playPosX + padSizeX, padSizeY + playPosY, 999999);

		// Draw opponent
		glBoxFilled(opPosX, 0 + opPosY, opPosX + padSizeX, padSizeY + opPosY, 999999);

		// Draw ball
		glBoxFilled(ballX, ballY, ballX + ballSizeX, ballY + ballSizeY, 999999);

		// Update ball position
		ballX += ballVelX;
		ballY += ballVelY;

		// Win conditions
		if (ballX <= 0) {
			// Player loses
		} else if (ballX >= SCR_MAX_X) {
			// Player wins!!
		}

		// Ball bouncing
		if (ballY <= 0) {
			// Positive coreVel, since we bounce off the top
			ballVelY = coreVel;
		}

		if (ballY + ballSizeY >= SCR_MAX_Y) {
			// Positive coreVel, since we bounce off the bottom
			ballVelY = -coreVel;
		}

		// We technically don't need screen bound bouncing on the X axis, since the paddles will be doing that, but it's good for testing
		if (ballX <= 0 || (
			between(ballX, playPosX, playPosX + padSizeX) &&
			between(ballY + ballSizeY, playPosY, playPosY + padSizeY)
		)) {
			ballVelX = coreVel;
		}

		if (ballX + ballSizeX >= SCR_MAX_X || (
			between(ballX + ballSizeX, opPosX, opPosX + padSizeX) &&
			between(ballY + ballSizeY, opPosY, opPosY + padSizeY)
		)) {
			ballVelX = -coreVel;
		}

		glEnd2D();
		glFlush(0);

		swiWaitForVBlank();
	}
}
