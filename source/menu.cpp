#include <nds.h>
#include <stdio.h>
#include <string>
#include <sstream>

void drawScore(PrintConsole *scoreConsole, int leftScore, int rightScore, int spaces) {
  consoleSelect(scoreConsole);
	consoleClear();

	std::stringstream scorePrint;
	scorePrint << leftScore << std::string(spaces, ' ') << rightScore;

	iprintf(scorePrint.str().c_str());
}

void drawGameMenu(PrintConsole *menuConsole) {
	consoleSelect(menuConsole);
	consoleClear();
	iprintf("This is the menu console");
}