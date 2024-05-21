#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <pico/i2c_slave.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

// use https://www.h-schmidt.net/FloatConverter/IEEE754.html to convert between float and binary/hex

// small class to store data from read buffer, it converts ints to floats mainly just here to abstract c code 
class Decoder {
  public:
    Decoder(int size){ // initialize class, set size, allocate storage
        this->size = size;
        float array[size];
        this->floats = array;
        for(int i=0;i<size;i++)
            array[i]=0;
    }
    void Store(uint8_t* binary_array) { // store buffer array of uint8 to n floats
        //bytes are stored in the wrong direction, flip and rewrite
        int bsize = sizeof(binary_array);
        uint8_t flipped_binary_array[bsize];
        for(int i=0;i<bsize;i++)
          flipped_binary_array[i]=binary_array[bsize-1-i];

        int size = this->size; 
        for(int i=0;i<size;i++) // write to floats (in reversed direction)
            *(this->floats+(i)) = *(((float*) flipped_binary_array)+(size-1-i));
    }
    float* GetValues() {
        return floats;
    }

  private:
    float* floats;
    int size;
};//put that into pico comms have pico comms d odecoding