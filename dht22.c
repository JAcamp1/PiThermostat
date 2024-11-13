#include "./dht22.h"

int data[5];
float cachedH;
float cachedC;

float read_DHT_Data() {
    uint8_t lastState     = HIGH;
    uint8_t stateDuration = 0;
    uint8_t stateChanges  = 0;
    uint8_t bitsRead      = 0;
    float   h             = 0.0;
    float   c             = 0.0;
    float   f             = 0.0;
    char *  dataType;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    /* Signal Sensor we're ready to read by pulling pin UP for 10 milliseconds,
       pulling pin down for 18 milliseconds and then back up for 40 microseconds. */

    pinMode( DHT_PIN, OUTPUT );
    digitalWrite( DHT_PIN, HIGH );
    delay(10);
    digitalWrite( DHT_PIN, LOW );
    delay(18);
    digitalWrite( DHT_PIN, HIGH );
    delayMicroseconds(40);

    /* Read data from pin.  Look for a change in state. */

    pinMode( DHT_PIN, INPUT );

    for ( (stateChanges=0), (stateDuration=0); (stateChanges < MAX_TIMINGS) && (stateDuration < 255) ; stateChanges++) {
        stateDuration  = 0;
        while( (digitalRead( DHT_PIN ) == lastState) && (stateDuration < 255) ) {
            stateDuration++;
            delayMicroseconds( 1 );
        };

        lastState = digitalRead( DHT_PIN );

        // First 2 state changes are sensor signaling ready to send, ignore them.
        // Each bit is preceeded by a state change to mark its beginning, ignore it too.
        if ( (stateChanges > 2) && (stateChanges % 2 == 0) ) {
            data[ bitsRead / 8 ] <<= 1; // Each array element has 8 bits.  Shift Left 1 bit.
            if ( stateDuration > 16 )   // A State Change > 16 microseconds is a '1'.
                data[ bitsRead / 8 ] |= 0x00000001;
            bitsRead++;
        }
    }

    /**
     * Read 40 bits. (Five elements of 8 bits each)  Last element is a checksum.
     */

    if ( (bitsRead >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
        h = (float)((data[0] << 8) + data[1]) / 10.0;
        c = (float)((data[2] << 8) + data[3]) / 10.0;
        if ( data[2] & 0x80 )           // Negative Sign Bit on.
            c *= -1;
        cachedH  = h;
        cachedC  = c;
        dataType = "Temperature";
    }
    else {                              // Data Bad, use cached values.
        h = cachedH;
        c = cachedC;
        dataType = "Cached Temp";
    }

    if ( (h == BAD_VALUE) || (c == BAD_VALUE) )
        return BAD_VALUE;
    else {
        f = c * 1.8f + 32.0f;
        return f;
    }
}

void initializeSensor() {
	if ( wiringPiSetup() == -1 )
		exit( 1 );
}
