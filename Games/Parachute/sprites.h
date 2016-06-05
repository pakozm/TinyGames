#ifndef SPRITES_H
#define SPRITES_H

#include "byte_array.h"

#define G 9.8f

class Sprite {
public:
  Sprite(int w, int h, const ByteArray *data) : width(w), heigth(h), data(data) {}
  virtual ~Sprite() {}

  bool getPixel(int x, int y) const {
    return data->getBit(y*width + x);
  }
 
  int width, heigth;
  const ByteArray *data;
};

class SpriteInScreen {
public:
  SpriteInScreen(Sprite *s, int x, int y) : sprite(s), x_pos(x), y_pos(y) {}

  /**
   * Translates the screen position to sprite position values, and returns
   * true/false depending on pixel color.
   */
  bool getScreenPixel(int screen_x, int screen_y) const {
    int x = x_pos - screen_x;
    int y = y_pos - screen_y;
    if (x >= 0 && x < sprite->width && y >= 0 && y < sprite->heigth) {
      return sprite->getPixel(x, y);
    }
    return false;
  }

  /**
   * Returns collision at bounding box level between two Sprites.
   */
  static bool collision(const SpriteInScreen &a, const SpriteInScreen &b) {
    return  (a.x_pos < b.x_pos + b.sprite->width &&
             a.x_pos + a.sprite->width > b.x_pos &&
             a.y_pos < b.y_pos + b.sprite->heigth &&
             a.y_pos + a.sprite->heigth > b.y_pos);
  }

  int x_pos, y_pos;
  Sprite *sprite;
};

template<typename T>
class ParabolicObject {
public:
  ParabolicObject(T *o, float vx, float vy, float x, float y) : object(o), vx(vx), vy(vy), x(x), y(y) {
    object->x_pos = x;
    object->y_pos = y;
  }

  void update(float dt=1.0f) {
    // projectile trajectory update (http://www.physics.buffalo.edu/phy410-505-2008/chapter2/ch2-lec1.pdf)
    x  += vx*dt;
    y  += vy*dt;
    vy -= G*dt;
    //
    object->x_pos = (int)x;
    object->y_pos = (int)y;
  }
private:
  SpriteInScreen *object;
  float vx, vy, x, y;
};

class ProgMemArrays {
public:
  PROGMEM static const byte boat[18];    // W=20, h=7 
  PROGMEM static const byte chopper[25]; // w=20, h=10
  PROGMEM static const byte coco[2];     // w=3, h=4
  PROGMEM static const byte shark[15];   // w=15, h=8
  PROGMEM static const byte soldier[13]; // w=9, h=11
};

class ProgMemByteArrays {
public:
  static ByteArrayInProgMem<18> boat;
  static ByteArrayInProgMem<25> chopper;
  static ByteArrayInProgMem<2>  coco;
  static ByteArrayInProgMem<15> shark;
  static ByteArrayInProgMem<13> soldier;
};

#endif // SPRITES_H
