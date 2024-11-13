/**
 * DHT22.h
 * Read data (Temperature & Relative Humidity) from DHT22 sensor.
 * Data is transmitted bit by bit.  Start of bit is signaled by line going LOW,
 * then value of bit is determined by how long the line stays HIGH.
 * @author Chris Wolcott.  Based on code found at http://www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/
 * Compile with: cc -Wall DHT22.c -o DHT22 -lwiringPi
 * Run with: ./DHT22
 */
#pragma once
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TIMINGS 85                  // Takes 84 state changes to transmit data 
#define DHT_PIN     1                   // BCM16  GPIO-16  Phy 36 
#define BAD_VALUE   999.9f

extern int     data[5];
extern float   cachedH;
extern float   cachedC;

float read_DHT_Data();

void initializeSensor();
