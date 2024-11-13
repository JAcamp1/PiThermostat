#include "dht22.h"
#include "7segment.h"
#include<wiringPi.h>

#define DIGIT0 0x00
#define DIGIT1 0x02
#define DIGIT2 0x06
#define DIGIT3 0x08
#define COLON  0x04

#define UP 25
#define DOWN 24
#define CONFIRM 23

#define CHANGEDUR 10

int currentTemp = 0;
int goalTemp = 74;
int currentSet = 0;
int goalSet = 3;
int needChange = 0;
int upPressed = 0;
int downPressed = 0;
int confirmPressed = 0;

int leds[] = {
	26,
	27,
	28,
	29
};

int deltaHistory[4][30] = {
	{1},
	{-2},
	{-1},
	{0}
};

int updateCurrentTemp() {
	int digit;
	digit = (int) (currentTemp / 10) % 10;
	displaySetBit(DIGIT0, numbers[digit]);
	digit = (int) (currentTemp) % 10;
	displaySetBit(DIGIT1, numbers[digit]);
	return 0;
}

int updateGoalTemp() {
	int digit;
	digit = (int) (goalTemp / 10) % 10;
	displaySetBit(DIGIT2, numbers[digit]);
	digit = (int) (goalTemp) % 10;
	displaySetBit(DIGIT3, numbers[digit]);
}

void pushBack(int* array, int value) {
	for (int i = 29; i > 0; i--) {
		array[i] = array[i-1];
	}
	array[0] = value;
}

float avg(int* array) {
	int total = 0;
	for (int i = 0; i < 30; i++) {
		total += array[i];
	}
	return (float) total / 30.0;
}

int calcSetting() {
	int bestSetting = 0;
	float bestError = 1000.0;
	float goalChange = (float) (goalTemp - currentTemp);
	for (int i = 0; i < 4; i++) {
		float error = abs((avg(deltaHistory[i]) * CHANGEDUR) - goalChange);
		printf("%d, %d\n", error, goalChange);
		if (error < bestError) {
			bestSetting = i;
			bestError = error;
		}
	}
	return bestSetting;
}

int loadFromFile(char *filename) {
	FILE *filePointer = fopen(filename, "r");
	if (filePointer == NULL) {
		return -1;
	}
	for (int i = 0; i < 4; i++) {
		fread(deltaHistory[i], sizeof(int), 30, filePointer);
	}
	fclose(filePointer);
	return 0;
}

int writeToFile(char *filename) {
	FILE *filePointer = fopen(filename, "w");
	for (int i = 0; i < 4; i++) {
		fwrite(deltaHistory[i], sizeof(int), 30, filePointer);
	}
	fclose(filePointer);
	return 0;
}

int main() {
	initializeDisplay();
	initializeSensor();
	for (int i = 0; i < 4; i++) {
		pinMode(leds[i], OUTPUT);
	}
	pinMode(UP, INPUT);
	pinMode(DOWN, INPUT);
	pinMode(CONFIRM, INPUT);
	int oldTemp = 0;
	int delta = 0;
	unsigned long int i = 0;
	if (loadFromFile("history.b") == -1) {
		printf("Failed to load from history file. Please check.\n");
	}
	updateGoalTemp();
	updateCurrentTemp();
	while(0==0) {
		if (upPressed) {
			if (digitalRead(UP) == 0) {
				upPressed = 0;
			}
		}
		if (downPressed) {
			if (digitalRead(DOWN) == 0) {
				downPressed = 0;
			}
		}
		if (confirmPressed) {
			if (digitalRead(CONFIRM) == 0) {
				confirmPressed = 0;
			}
		}
		if (upPressed == 0) {
			if (digitalRead(UP)) {
				upPressed = 1;
				goalTemp += 1;
				updateGoalTemp();
			}
		}
		if (downPressed == 0) {
			if (digitalRead(DOWN)) {
				downPressed = 1;
				goalTemp -= 1;
				updateGoalTemp();
			}
		}
		if (confirmPressed == 0) {
			if (digitalRead(CONFIRM)) {
				confirmPressed = 1;
				currentSet = goalSet;
			}
		}
		if (i % 500 == 0) {
			needChange = (goalSet != currentSet);
			if (needChange) {
				digitalWrite(leds[goalSet], HIGH);
			}
		}
		if (i % 1000 == 0) {
			currentTemp = (int) read_DHT_Data();
			updateCurrentTemp();
			for (int j = 0; j < 4; j++) {
				digitalWrite(leds[j], LOW);
			}
		}
		if (i % 60000 == 0) {
			i = 0;
			if (oldTemp != 0) {
				delta = currentTemp - oldTemp;
				pushBack(deltaHistory[currentSet], delta);
			}
			oldTemp = currentTemp;
			goalSet = calcSetting();
			writeToFile("history.b");
		}
		i += 1;
		delay(1);
	}
	return 0;
}
