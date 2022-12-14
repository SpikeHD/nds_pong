#include <nds.h>
#include <gl2d.h>

#include <stdio.h>
#include <string.h>

const int SCR_MAX_X = 256;
const int SCR_MAX_Y = 192;
const int PAD_SIZE_X = 5;
const int PAD_SIZE_Y = 60;
const int BALL_SIZE = 10;

volatile int frame = 0;

void Vblank() {
	frame++;
}

bool between(int num, int min, int max) {
	return min <= num && num <= max;
}

u32 kdr() {
	return keysHeld();
}

/* Controls section */
bool leftUp() {
	return kdr() & KEY_UP;
}

bool leftDown() {
	return kdr() & KEY_DOWN;
}

bool rightUp() {
	return kdr() & KEY_X;
}

bool rightDown() {
	return kdr() & KEY_Y;
}

// Game
int main(void) {
	int playPosX = 10, playPosY = 0;
	int opPosX = SCR_MAX_X - 10, opPosY = 0;
	int ballX = 0, ballY = 0;

	// Paddle movement speed
	int paddleVel = 2;

	// Core velocity is just the speed
	int coreVel = 2;

	// Velocity starts as down and towards the player
	int ballVelX = -coreVel, ballVelY = coreVel;

	// Store touch position
	// touchPosition touchXY;
	irqSet(IRQ_VBLANK, Vblank);

	videoSetMode(MODE_5_3D);

	// initialize gl2d
	glScreen2D();

	// Start the ball at the middle of the screen
	ballX = SCR_MAX_X / 2;
	ballY = SCR_MAX_Y / 2;

	// Game loop
	while(true) {
		glBegin2D();

		// touchRead(&touchXY);

		// Get inputs
		scanKeys();

		// Button inputs
		if (leftUp()) {
			playPosY -= paddleVel;
		}

		if (leftDown()) {
			playPosY += paddleVel;
		}

		if (rightUp()) {
			opPosY -= paddleVel;
		}

		if (rightDown()) {
			opPosY += paddleVel;
		}

		// // Prevent paddle resetting 
		// if (touchXY.py != 0)
		// 	playPosY = touchXY.py;

		// // Prevent paddle going offscreen
		// if (playPosY > SCR_MAX_Y - PAD_SIZE_Y)
		// 	playPosY = SCR_MAX_Y - PAD_SIZE_Y;

		// Draw player
		glBoxFilled(playPosX, 0 + playPosY, playPosX + PAD_SIZE_X, PAD_SIZE_Y + playPosY, 999999);

		// Draw opponent
		glBoxFilled(opPosX, 0 + opPosY, opPosX + PAD_SIZE_X, PAD_SIZE_Y + opPosY, 999999);

		// Draw ball
		glBoxFilled(ballX, ballY, ballX + BALL_SIZE, ballY + BALL_SIZE, 999999);

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

		if (ballY + BALL_SIZE >= SCR_MAX_Y) {
			// Positive coreVel, since we bounce off the bottom
			ballVelY = -coreVel;
		}

		// We technically don't need screen bound bouncing on the X axis, since the paddles will be doing that, but it's good for testing
		if (ballX <= 0 || (
			between(ballX, playPosX, playPosX + PAD_SIZE_X) &&
			between(ballY + BALL_SIZE, playPosY, playPosY + PAD_SIZE_Y)
		)) {
			ballVelX = coreVel;
		}

		if (ballX + BALL_SIZE >= SCR_MAX_X || (
			between(ballX + BALL_SIZE, opPosX, opPosX + PAD_SIZE_X) &&
			between(ballY + BALL_SIZE, opPosY, opPosY + PAD_SIZE_Y)
		)) {
			ballVelX = -coreVel;
		}

		glEnd2D();
		glFlush(0);

		swiWaitForVBlank();
	}
}
