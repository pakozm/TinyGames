/* 2015
 * UFO Escape game by Ilya Titov. Find building instructions on http://webboggles.com/
 * The code that does not fall under the licenses of sources listed below can be used non commercially with attribution.
 *
 * If you have problems uploading this sketch, this is probably due to sketch size - you need to update ld.exe in arduino\hardware\tools\avr\avr\bin
 * https://github.com/TCWORLD/ATTinyCore/tree/master/PCREL%20Patch%20for%20GCC
 *
 * This sketch is using the screen control and font functions written by Neven Boyanov for the http://tinusaur.wordpress.com/ project
 * Source code and font files available at: https://bitbucket.org/tinusaur/ssd1306xled
 * 
 * Sleep code is based on this blog post by Matthew Little:
 * http://www.re-innovation.co.uk/web12/index.php/en/blog-75/306-sleep-modes-on-attiny85
 */
/* 2016
 *
 * Updated by pakozm (Paco Zamora Martinez):
 *
 *   - code refactoring
 *
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2) RIGHT
 *   SDA (4) -|    |- (1) BUZZER
 *   GND     -|____|- (0) LEFT
 */
#include <EEPROM.h>
#include <font6x8.h>
#include <ssd1306.h>
#include <avr/sleep.h>
#include <avr/interrupt.h> // needed for the additional interrupt

uint16_t *EEPROM_ADDR = (uint16_t*)0;

const int LEFT_BTN  = 0;
const int BUZZER    = 1;
const int RIGHT_BTN = 2;

const int SCREEN_WIDTH   = 128;
const int SCREEN_HEIGHT  = 64;
const int NUM_ROWS       = 8;

boolean stopAnimate = 0; // this is set to 1 when a collision is detected
int maxObstacles = 1; // this defines the max number of in game obstacles
int obstacleStep = 1; // pixel step of obstacles per frame
int obstacle[9] = {-50,-50,-50,-50,-50,-50,-50,-50,-50}; // x offset of the obstacle default position, out of view
int gapOffset[9] = {0,0,0,0,0,0,0,0,0}; // y offset of the fly-through gap
int gapSize[9]; // y height of the gap
byte maxGap = 60; // max height of the gap
int stepsSinceLastObstacle = 0; // so obstacles are not too close
byte gapBlock[9] = {0,0,0,0,0,0,0,0,0}; // if the fly-through gap is closed
byte blockChance = 0; // this higher value decreases the likelihood of gap being closed 
boolean fire = 0; // set to 1 when the fire interrupt is triggered
byte fireCount = 0; // the shot is persistent for several frames
byte playerOffset = 0; // y offset of the top of the player
byte flames = 0; // this is set to 1 when the move up interrupt is triggered

byte flameMask[2]={B00111111,B11111111}; // this is used to only show the flame part of the icon when moving up


int score = 0; // score - this affects the difficulty of the game
ISR(PCINT0_vect){ // PB0 pin button interrupt			     
  //if (playerOffset >1&&stopAnimate==0){playerOffset-=1;} // for debounce, the movement is in the main loop//
  return;
}
void playerInc(){ // PB2 pin button interrupt
  fire = 1;
  fireCount = 5; // number of frames the shot will persist
}

void setup() {
  DDRB = 0b00000010;  	// set PB1 as output (for the speaker)
  PCMSK = 0b00000001;	// pin change mask: listen to portb bit 1
  GIMSK |= 0b00100000;	// enable PCINT interrupt 
  sei();	        // enable all interrupts
  attachInterrupt(0,playerInc,RISING);
  resetGame();
}
void loop() { 
  //update game vars to make it harder to play
  if (score < 500){blockChance = 11-score/50;  maxObstacles=score/70+1;}
  if (score > 130){obstacleStep = 2;}
  if (score < 2000){maxGap = 60-score/100;}
  if (fire == 1){score--;}
  if (fireCount>0){fireCount--;}
              
  if (digitalRead(LEFT_BTN)==1){if (playerOffset >0 && stopAnimate==0){playerOffset--; flames = 1; // move player up
      for (int i = 0; i<2; i++){
        beep(1,random(0,i*2));
      }
    }} 
  if (digitalRead(LEFT_BTN)==1){if (playerOffset >0 && stopAnimate==0){playerOffset--; flames = 1; // move player up
      for (int i = 0; i<2; i++){
        beep(1,random(0,i*2));
      }
    }}
  if (digitalRead(LEFT_BTN)==1){if (playerOffset >0 && stopAnimate==0){playerOffset--; flames = 1; // move player up
      for (int i = 0; i<2; i++){
        beep(1,random(0,i*2));
      }
    }}
  stepsSinceLastObstacle += obstacleStep;
  for (byte i = 0; i<maxObstacles;i++){ // fly obstacles
    if (obstacle[i] >= 0 && obstacle[i] <= SCREEN_WIDTH && stopAnimate==0){
      obstacle[i] -= obstacleStep;
      if (gapBlock[i]>0 && obstacle[i] < 36  && playerOffset>gapOffset[i] && playerOffset+5<gapOffset[i]+gapSize[i] && fireCount > 0){//
        gapBlock[i] = 0;
        score += 5; 
        for (byte cp = 400; cp>0; cp--){
          beep(1,cp);
        }
      }
    } 
                  
    if (obstacle[i]<=4 && stepsSinceLastObstacle>=random(30,100)){ // generate new obstacles
      obstacle[i] = 123;
      gapSize[i] = random(25,maxGap);
      gapOffset[i] = random(0,SCREEN_HEIGHT-gapSize[i]);
      if (random(0,blockChance)==0){gapBlock[i] = 1;}else {gapBlock[i] = 0;}
      stepsSinceLastObstacle = 0;
      score+=1;
    }
  }
                
  if (playerOffset < 56 && stopAnimate==0){playerOffset++;} // player gravity
                
  delay(20/maxObstacles); // controls game speed, more obstacles take longer to be sent to the screen
  if (stopAnimate==0){ssd1306_clearscreen();}
              

  // update whats on the screen
                 
  noInterrupts();
  // Send Obstacle
  for (byte i = 0; i<maxObstacles;i++){
    if (obstacle[i] >= -5 && obstacle[i] <= SCREEN_WIDTH){ // only deal with visible obstacles
      if (obstacle[i] > 8 && obstacle[i] <16){ // look for collision if obstacle is near the player
        if (playerOffset < gapOffset[i] || playerOffset+5 > gapOffset[i]+gapSize[i] || gapBlock[i] != 0){
          // collision!
          stopAnimate = 1; 
          int topScore = EEPROM.read(0);
          topScore = topScore << 8;
          topScore = topScore |  EEPROM.read(1);
          if (score>topScore){topScore = score; EEPROM.write(1,topScore & 0xFF); EEPROM.write(0,(topScore>>8) & 0xFF); }

          ssd1306_char_f6x8(32, 3, "Game Over");
          ssd1306_char_f6x8(32, 5, "score:");
          char temp[10] = {0,0,0,0,0,0,0,0,0,0};
          itoa(score,temp,10);
          ssd1306_char_f6x8(70, 5, temp);
          ssd1306_char_f6x8(32, 6, "top score:");
          itoa(topScore,temp,10);
          ssd1306_char_f6x8(90, 6, temp);
          for (int i = 0; i<1000; i++){
            beep(1,random(0,i*2));
          }
          delay(2000);
          interrupts();
          system_sleep();
          resetGame();
          noInterrupts(); 
        }
      }                      
                      
      for (byte row = 0; row <NUM_ROWS; row++){
                        
        ssd1306_setpos(obstacle[i],row);
        ssd1306_send_data_start();
                          
        if (obstacle[i]>0&&obstacle[i] < SCREEN_WIDTH){
                             
          if ((row+1)*NUM_ROWS - gapOffset[i] <= NUM_ROWS){ // generate obstacle : top and transition
            byte temp = B11111111>>((row+1)*NUM_ROWS - gapOffset[i]); 
            byte tempB = B00000000; 
            if (gapBlock[i]>0){tempB=B10101010;}
            ssd1306_send_byte(temp);
            ssd1306_send_byte(temp|tempB>>1);
            ssd1306_send_byte(temp|tempB);
            ssd1306_send_byte(temp);
                                
          }else if (row*NUM_ROWS>=gapOffset[i] && (row+1)*NUM_ROWS<=gapOffset[i]+gapSize[i]){ // middle gap
            byte tempB = B00000000; 
            if (gapBlock[i]>0){tempB=B10101010;}
            ssd1306_send_byte(B00000000);
            ssd1306_send_byte(B00000000|tempB>>1);
            ssd1306_send_byte(B00000000|tempB);
            ssd1306_send_byte(B00000000);

          }else if ((gapOffset[i] +gapSize[i]) >= row*NUM_ROWS && (gapOffset[i] +gapSize[i]) <= (row+1)*NUM_ROWS){ // bottom transition
            //}else if ((gapOffset[i] +gapSize[i]) >= row*NUM_ROWS && (gapOffset[i] +gapSize[i]) <= (row+1)*NUM_ROWS){ // bottom transition
            //byte temp = B11111111<<((gapOffset[i] + gapSize[i])%NUM_ROWS); 
                                
            byte temp = B11111111<<((gapOffset[i] + gapSize[i])%NUM_ROWS); 
            byte tempB = B00000000; 
            if (gapBlock[i]>0){tempB=B10101010;}
            ssd1306_send_byte(temp);
            ssd1306_send_byte(temp|tempB>>1);
            ssd1306_send_byte(temp|tempB);
            ssd1306_send_byte(temp);
                                
          }else { // fill rest of obstacle
            ssd1306_send_byte(B11111111);
            ssd1306_send_byte(B11111111);
            ssd1306_send_byte(B11111111);
            ssd1306_send_byte(B11111111);
          }
          ssd1306_send_data_stop();
        }
      }
                   
    }
  }
                  
                 
                 
  if (playerOffset%NUM_ROWS!=0){ // overflow the player icon into the next screen row if split
    ssd1306_setpos(NUM_ROWS,playerOffset/NUM_ROWS);
    ssd1306_send_data_start();
    if (stopAnimate==0){
      ssd1306_send_byte((B00001100&flameMask[flames])<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01011110&flameMask[flames])<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B10010111&flameMask[flames])<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01010011&flameMask[flames])<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01010011&flameMask[flames])<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B10010111&flameMask[flames])<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01011110&flameMask[flames])<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B00001100&flameMask[flames])<<playerOffset%NUM_ROWS);
      if (fireCount >0){
        for (byte f = 0; f<=24; f++){
          ssd1306_send_byte(B00000100<<playerOffset%NUM_ROWS);
        }
        ssd1306_send_byte(B00010101<<playerOffset%NUM_ROWS);
        ssd1306_send_byte(B00001010<<playerOffset%NUM_ROWS);
        ssd1306_send_byte(B00010101<<playerOffset%NUM_ROWS);
        if (fire==1){beep(50,100);}
        fire = 0;
                              
      }
    }else {
      ssd1306_send_byte((B00001100&flameMask[flames] | random(0,255))<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01011110&flameMask[flames] | random(0,255))<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B10010111&flameMask[flames] | random(0,255))<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01010011&flameMask[flames] | random(0,255))<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01010011&flameMask[flames] | random(0,255))<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B10010111&flameMask[flames] | random(0,255))<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01011110&flameMask[flames] | random(0,255))<<playerOffset%NUM_ROWS);
      ssd1306_send_byte((B00001100&flameMask[flames] | random(0,255))<<playerOffset%NUM_ROWS);
    }
                        
    ssd1306_send_data_stop();
    ssd1306_setpos(NUM_ROWS,playerOffset/NUM_ROWS+1);
    ssd1306_send_data_start();
    if (stopAnimate==0){
      ssd1306_send_byte((B00001100&flameMask[flames])>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01011110&flameMask[flames])>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B10010111&flameMask[flames])>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01010011&flameMask[flames])>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01010011&flameMask[flames])>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B10010111&flameMask[flames])>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01011110&flameMask[flames])>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B00001100&flameMask[flames])>>NUM_ROWS-playerOffset%NUM_ROWS);
      if (fireCount >0){
        for (byte f = 0; f<=24; f++){
          ssd1306_send_byte(B00000100>>NUM_ROWS-playerOffset%NUM_ROWS);
        }
        ssd1306_send_byte(B00010101>>NUM_ROWS-playerOffset%NUM_ROWS);
        ssd1306_send_byte(B00001010>>NUM_ROWS-playerOffset%NUM_ROWS);
        ssd1306_send_byte(B00010101>>NUM_ROWS-playerOffset%NUM_ROWS);
        if (fire==1){beep(50,100);}
        fire = 0;
                              
      }
    }else {
      ssd1306_send_byte((B00001100&flameMask[flames] | random(0,255))>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01011110&flameMask[flames] | random(0,255))>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B10010111&flameMask[flames] | random(0,255))>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01010011&flameMask[flames] | random(0,255))>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01010011&flameMask[flames] | random(0,255))>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B10010111&flameMask[flames] | random(0,255))>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B01011110&flameMask[flames] | random(0,255))>>NUM_ROWS-playerOffset%NUM_ROWS);
      ssd1306_send_byte((B00001100&flameMask[flames] | random(0,255))>>NUM_ROWS-playerOffset%NUM_ROWS);
    }
    ssd1306_send_data_stop();
  }else {
    ssd1306_setpos(NUM_ROWS,playerOffset/NUM_ROWS);
    ssd1306_send_data_start();
    if (stopAnimate == 0){
      ssd1306_send_byte(B00001100&flameMask[flames]);
      ssd1306_send_byte(B01011110&flameMask[flames]);
      ssd1306_send_byte(B10010111&flameMask[flames]);
      ssd1306_send_byte(B01010011&flameMask[flames]);
      ssd1306_send_byte(B01010011&flameMask[flames]);
      ssd1306_send_byte(B10010111&flameMask[flames]);
      ssd1306_send_byte(B01011110&flameMask[flames]);
      ssd1306_send_byte(B00001100&flameMask[flames]);
      if (fireCount >0){
        for (byte f = 0; f<=24; f++){
          ssd1306_send_byte(B00000100);
        }
        ssd1306_send_byte(B00010101);
        ssd1306_send_byte(B00001010);
        ssd1306_send_byte(B00010101);
        if (fire==1){beep(50,100);}
        fire = 0;
                              
      }
    }else {
      ssd1306_send_byte(B00001100&flameMask[flames] | random(0,255));
      ssd1306_send_byte(B01011110&flameMask[flames] | random(0,255));
      ssd1306_send_byte(B10010111&flameMask[flames] | random(0,255));
      ssd1306_send_byte(B01010011&flameMask[flames] | random(0,255));
      ssd1306_send_byte(B01010011&flameMask[flames] | random(0,255));
      ssd1306_send_byte(B10010111&flameMask[flames] | random(0,255));
      ssd1306_send_byte(B01011110&flameMask[flames] | random(0,255));
      ssd1306_send_byte(B00001100&flameMask[flames] | random(0,255)); 
    }
    ssd1306_send_data_stop();
  }
                      
  // display score
                      
  //ssd1306_char_f6x8(32, 8, "score:");
                      
  char temp[10] = {0,0,0,0,0,0,0,0,0,0};
  itoa(score,temp,10);
  ssd1306_char_f6x8(92, 0, temp);
                          
                          
  flames = 0;
  interrupts();
                  
  //    
                     
              
              
              
  if (stopAnimate == 1){
    for (int i = 0; i<1000; i++){
      beep(1,random(0,i*2));
    }
    delay(2000);
    system_sleep(); 
  }
}
void resetGame(){
  delay(40);
  noInterrupts();
  ssd1306_init();
  ssd1306_fillscreen(0x00);
  
  ssd1306_char_f6x8(16, 2, "U F O  E S C A P E");
  ssd1306_char_f6x8(20, 4, "webboggles.com");
  ssd1306_char_f6x8(26, 6, "pakozm  2016");
  interrupts();
  beep(200,600);          beep(300,200);          beep(400,300);
  delay(2000);
  noInterrupts();
  
  stopAnimate = 0;
  score = 0;

  maxObstacles = 3;
  obstacleStep = 2;
  for (byte i = 0; i<9; i++){
    obstacle[i] = -50;
    gapOffset[i]=0;
  }
  stepsSinceLastObstacle = 0;
  playerOffset = 0; // y offset of the top of the player
}

void beep(int bCount,int bDelay){
  for (int i = 0; i<=bCount; i++){
    digitalWrite(BUZZER,HIGH);
    for(int i2=0; i2<bDelay; i2++){__asm__("nop\n\t");}
    digitalWrite(BUZZER,LOW);
    for(int i2=0; i2<bDelay; i2++){__asm__("nop\n\t");}
  }
}

// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void system_sleep() {
  ssd1306_fillscreen(0x00);
  ssd1306_send_command(0xAE);
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System actually sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON  
  ssd1306_send_command(0xAF);
}
