#include <nds.h>

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