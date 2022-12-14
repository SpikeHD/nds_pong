#include <nds.h>
#include <gl2d.h>

#include <stdio.h>
#include <string>
#include <sstream>

// Custom stuff
#include "consts.h"
#include "menu.h"
#include "controls.h"
#include "number_util.h"
#include "ai.h"

volatile int frame = 0;

void Vblank() {
	frame++;
}

// Game
int main(void) {
	bool ai = false;
	int playPosX = 10, playPosY = 0;
	int opPosX = SCR_MAX_X - 10, opPosY = 0;
	int ballX = 0, ballY = 0;
	int leftScore = 0;
	int rightScore = 0;

	// This is used to store what frame we changed an option, so that we can wait some amount of frames before
	// allowing another option change
	int changeFrm = 0;

	// Spacing between score prints
	int scrSpc = CONSOLE_WIDTH - 2;

	// Paddle movement speed
	int paddleVel = 2;

	// Core velocity is just the speed
	int coreVel = 3;

	// Velocity starts as down and towards the player
	int ballVelX = -coreVel, ballVelY = coreVel;

	// Score and menu change flags, used to reprint console
	bool scrChange = true, menuChange = true;

	// Store touch position
	touchPosition touchXY;
	irqSet(IRQ_VBLANK, Vblank);

	videoSetMode(MODE_5_3D);
	videoSetModeSub(MODE_0_2D);

	// Init consoles
	PrintConsole scoreConsole;
	consoleInit(&scoreConsole, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

	PrintConsole menuConsole;
	consoleInit(&menuConsole, 1, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
	
	// Draws right at the top of the bottom screen
	consoleSetWindow(&scoreConsole, CONSOLE_T_WIDTH / 6, 0, CONSOLE_T_WIDTH / 1.5, 1);

	// Draws in the middle of the bottom screen
	consoleSetWindow(&menuConsole, 0, CONSOLE_T_HEIGHT / 2, CONSOLE_T_WIDTH, 4);

	// initialize gl2d
	glScreen2D();

	// Start the ball at the middle of the screen
	ballX = SCR_MAX_X / 2;
	ballY = SCR_MAX_Y / 2;

	// Game loop
	while(true) {
		int dig = digits(leftScore) + digits(rightScore);
		scrSpc = (CONSOLE_T_WIDTH / 1.5) - dig;

		glBegin2D();

		// If the score has changed, trigger a referesh
		if (scrChange) {
			drawScore(&scoreConsole, leftScore, rightScore, scrSpc);

			scrChange = false;
		}

		if (menuChange) {
			drawGameMenu(&menuConsole, coreVel, ai);
			menuChange = false;

			changeFrm = frame;
		}

		// Reset the frame we got our last option change from, after a certain amount hafe passed
		if (frame >= changeFrm + 20) {
			changeFrm = 0;
		}

		// If we don't have a menu change, and it's been enough frames, we can wait for an input
		if (!menuChange && changeFrm == 0) {
			touchRead(&touchXY);
			menuChange = menuActions(touchXY, coreVel, ai);

			if (coreVel < 0) coreVel = 0;

			// Only allow up to 9
			if (coreVel > 9) coreVel = 9;

			bool xVelNeg = ballVelX < 0;
			bool yVelNeg = ballVelY < 0;

			// Reassign the coreVel in case of change within menu
			ballVelX = xVelNeg ? -coreVel : coreVel;
			ballVelY = yVelNeg ? -coreVel : coreVel;
		}

		// Get inputs
		scanKeys();

		// Button inputs
		if (leftUp()) {
			playPosY -= paddleVel;
		}

		if (leftDown()) {
			playPosY += paddleVel;
		}

		// Control AI if AI is enabled, otherwise enable second player
		if (!ai) {
			if (rightUp()) {
				opPosY -= paddleVel;
			}

			if (rightDown()) {
				opPosY += paddleVel;
			}
		} else {
			// aiMove() returns either 1 or -1, so we know what direction to move by multiplying paddle velocity by the return value
			opPosY = opPosY + (paddleVel * aiMove(opPosY, ballY));
		}


		// Prevent left paddle going offscreen
		if (playPosY <= 0)
			playPosY = 0;

		if (playPosY + PAD_SIZE_Y >= SCR_MAX_Y)
			playPosY = SCR_MAX_Y - PAD_SIZE_Y;

		// Prevent right paddle going offscreen
		if (opPosY <= 0)
			opPosY = 0;

		if (opPosY + PAD_SIZE_Y >= SCR_MAX_Y)
			opPosY = SCR_MAX_Y - PAD_SIZE_Y;

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
			rightScore++;

			// Reset ball
			ballX = SCR_MAX_X / 2;
			ballY = SCR_MAX_Y / 2;

			scrChange = true;
		} else if (ballX + BALL_SIZE >= SCR_MAX_X) {
			leftScore++;
			
			// Reset ball
			ballX = SCR_MAX_X / 2;
			ballY = SCR_MAX_Y / 2;

			scrChange = true;
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
