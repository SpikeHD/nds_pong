/**
 * Return -1 to move down, positive 1 to move up.
 * "main.cpp" will multiply the return value by the paddle velocity to properly move the paddle
 * 
 * @return int 
 */
int aiMove(int paddleMidY, int ballY) {
  return paddleMidY > ballY ? -1 : 1;
}