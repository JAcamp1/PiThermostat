#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <time.h>

extern char *filename;
extern int file_i2c;
extern int addr;
extern unsigned char buffer[];
extern unsigned char numbers[];
extern unsigned char digits[];

void displaySetBit(unsigned char loc, unsigned char code);

void displayDoOp(unsigned char op);

int initializeDisplay();
