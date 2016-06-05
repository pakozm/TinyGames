#ifndef SPRITES_H
#define SPRITES_H

#include "byte_array.h"

#define G 9.8f
#define SCREEN_W 128
#define SCREEN_H 64

class Sprite {
public:
  Sprite(int w, int h, const ByteArray *data) : width(w), height(h), data(data) {}
  virtual ~Sprite() {}

  bool getPixel(int x, int y) const {
    return data->getBit(y*width + x);
  }

  int getWidth() const { return width; }
  int getHeight() const { return height; }

private:
  int width, height;
  const ByteArray *data;
};

class DummySprite : public Sprite{
public:
  DummySprite() : Sprite(0, 0, NULL) {}
  virtual ~DummySprite() {}
};

class SpriteInScreen {
public:
  SpriteInScreen(Sprite &s=dummy, int x=-1, int y=-1) : sprite(s), x_pos(x), y_pos(y) {}

  void init(Sprite &s, int x, int y) {
    sprite = s;
    x_pos = x;
    y_pos = y;
  }

  void clear() { sprite = dummy; x_pos=-1; y_pos=-1; }
  bool valid() const { return &sprite == &dummy; }

  /**
   * Translates the screen position to sprite position values, and returns
   * true/false depending on pixel color.
   */
  bool getScreenPixel(int screen_x, int screen_y) const {
    int x = x_pos - screen_x;
    int y = y_pos - screen_y;
    if (x >= 0 && x < getWidth() && y >= 0 && y < getHeight()) {
      return sprite.getPixel(x, y);
    }
    return false;
  }

  /**
   * Returns collision at bounding box level between two Sprites.
   */
  static bool collision(const SpriteInScreen &a, const SpriteInScreen &b) {
    return  (a.x_pos < b.x_pos + b.getWidth()&&
             a.x_pos + a.getWidth()> b.x_pos &&
             a.y_pos < b.y_pos + b.getHeight()&&
             a.y_pos + a.getHeight()> b.y_pos);
  }

  int getWidth() const { return sprite.getWidth(); }
  int getHeight() const { return sprite.getHeight(); }

  int x_pos, y_pos;

private:
  Sprite &sprite;
  static DummySprite dummy;
};

class UpdatableObject {
public:
  virtual ~UpdatableObject() {}
  virtual bool update(float dt) = 0;
};

template<typename T>
class RandomXObject : public UpdatableObject {
public:
  RandomXObject(T *o, int y, int x) : object(o) {
    o->x_pos = x;
    o->y_pos = y;
  }
  virtual ~RandomXObject() {}
  void update(float dt) {
    // apply random movement
    x += max(0, min(SCREEN_W - object->getWidth(), random(0, 2) - 1));
    //
    object->x_pos = x;
    object->y_pos = y;
    //
    return true;
  }

private:
  T *object;
  const int y;
  int x;
};


template<typename T>
class ParabolicObject : public UpdatableObject {
public:
  ParabolicObject(T *o, float vx, float vy, float x, float y) : object(o), vx(vx), vy(vy), x(x), y(y) {
    object->x_pos = x;
    object->y_pos = y;
  }
  virtual ~ParabolicObject() {}

  void update(float dt) {
    // projectile trajectory update (http://www.physics.buffalo.edu/phy410-505-2008/chapter2/ch2-lec1.pdf)
    x  += vx*dt;
    y  += vy*dt;
    vy -= G*dt;
    //
    object->x_pos = (int)x;
    object->y_pos = (int)y;
    // returns false when the object is out of the screen
    return (object->x_pos >= 0 && object->x_pos + object->getWidth() < SCREEN_W &&
            object->y_pos >= 0 && object->y_pos + object->getHeight() < SCREEN_H);
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
