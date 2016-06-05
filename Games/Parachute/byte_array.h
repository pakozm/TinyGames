#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include <avr/pgmspace.h>

class ByteArray {
public:
  virtual ~ByteArray() {}
  virtual bool getBit(int p) const = 0;
};


template<int N>
class ByteArrayInProgMem {
public:
  ByteArrayInProgMem(PROGMEM const byte * const data) : data(data) {
    bits_len = N<<3;
  }
  virtual ~ByteArrayInProgMem() {}
  bool getBit(int p) const {
    int b = p / bits_len;
    int i = p % bits_len;
    return bitRead(pgm_read_byte_near(data + b), i);
  }
private:
  size_t bits_len;
  PROGMEM const byte * const data;
};

#endif // BYTE_ARRAY_H

