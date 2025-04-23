#include <iostream>
#include <wiringPi.h>

using namespace std;

void forkAndPlay();

int main() {

	wiringPiSetup();
	pinMode(26, INPUT);
	pinMode(25, OUTPUT);
	digitalWrite(25, HIGH);

	for (;;) {
		if (digitalRead(26)) {
			forkAndPlay();
			delay(1000);
		}
	}
}
