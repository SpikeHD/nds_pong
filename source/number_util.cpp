bool between(int num, int min, int max) {
	return min <= num && num <= max;
}

// https://www.tutorialspoint.com/determining-how-many-digits-there-are-in-an-integer-in-cplusplus
int digits(int i) {
	int count = 1;
   while(i > 9) {
      i = i / 10;
      count++;
   }
   return count;
}
