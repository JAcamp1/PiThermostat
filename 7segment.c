#include "./7segment.h"

char *filename = "/dev/i2c-1";
int file_i2c;
int addr = 0x70;
unsigned char buffer[2] = {0};
unsigned char numbers[10] = {
        0x3F,
        0x06,
        0x5B,
        0x4F,
        0x66,
        0x6D,
        0x7D,
        0x07,
        0x7F,
        0x6F
};
unsigned char digits[4] = {
        0x00,
        0x02,
        0x06,
        0x08
};

void displaySetBit(unsigned char loc, unsigned char code) {
	buffer[0] = loc;
	buffer[1] = code;
	write(file_i2c, buffer, 2);
}

void displayDoOp(unsigned char op) {
	buffer[0] = op;
	write(file_i2c, buffer, 1);
}

int initializeDisplay(){
	if ((file_i2c = open(filename, O_RDWR)) < 0) {
		printf("Failed to open communication");
		return -1;
	}


	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0) {
		printf("Connection failure with display");
		return -1;
	}

	displayDoOp(0x21); //start oscillator
	displayDoOp(0x81); //set blinking to off
	displayDoOp(0xE0); //set brightness to 0
	return 0;
}
