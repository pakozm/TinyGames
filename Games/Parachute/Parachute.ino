#include "sprites.h"

#define MAX_CHOPPERS 1
#define MAX_COCOS 4
#define MAX_SHARKS 10
#define MAX_SOLDIERS 10

Sprite boat_sprite(20,7, &ProgMemByteArrays::boat);
Sprite chopper_sprite(20,10, &ProgMemByteArrays::chopper);
Sprite coco_sprite(3,4, &ProgMemByteArrays::coco);
Sprite shark_sprite(15,8, &ProgMemByteArrays::shark);
Sprite soldier_sprite(9,11, &ProgMemByteArrays::soldier);

int n_choppers, n_cocos, n_sharks, n_soldiers;
SpriteInScreen choppers[MAX_CHOPPERS];
SpriteInScreen cocos[MAX_COCOS];
SpriteInScreen sharks[MAX_SHARKS];
SpriteInScreen soldiers[MAX_SOLDIERS];

SpriteInScreen boat(boat_sprite, (SCREEN_W - boat_sprite.getWidth())/2,
                    SCREEN_H - boat_sprite.getHeight());

void init() {
  for (int i=0; i<MAX_CHOPPERS; ++i) choppers[i].clear();
  for (int i=0; i<MAX_COCOS; ++i) cocos[i].clear();
  for (int i=0; i<MAX_SHARKS; ++i) sharks[i].clear();
  for (int i=0; i<MAX_SOLDIERS; ++i) soldiers[i].clear();

  boat.init(boat_sprite,
            (SCREEN_W - boat_sprite.getWidth())/2,
            SCREEN_H - boat_sprite.getHeight());
}

void shutdown() {
}

void setup() {
}

void loop() {
  init();
  shutdown();
}

