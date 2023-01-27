/*****************************************************************************/
/*                                                                           */
/*                                  game.c                                   */
/*                                                                           */
/*              C version of Boulder Dash by Dustan Skrakl                   */
/*                Converted to C from the original basic                     */
/*                               by Rob Rose                                 */
/*                                                                           */
/*                                                                           */
/* This software is provided "as-is", without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated, but is not required.                                      */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <conio.h>
#include <unistd.h>
#include <cbm.h>

#include "leveldata.h"

#include "game.h"

// globals
uint8_t diamondsCollected=0;
uint16_t score=0;
uint8_t diamondsRemaining=0;
uint8_t playerX=3, playerY=2;
uint8_t tick=0;
uint8_t doorX=0, doorY=0;
bool levelComplete=false;
uint8_t currentLevel=0;
uint8_t livesRemaining=1;   // keep at 1
uint8_t screenX=0,screenY=0;
uint16_t hscroll=0,vscroll=0;
uint16_t seconds=0;
bool dead=false;

uint8_t fallingX[30], fallingY[30], falling=0, fallingSave=0;



void moveCharacter()
{
  // set y position
  vpoke((playerY*16) & 255, 0x1FC0C);
  vpoke(playerY/16, 0x1FC0D);

  // set x position
  vpoke((playerX*16) & 255, 0x1FC0A);
  vpoke(playerX/16,0x1FC0B);
}

void checkForBoulder(uint8_t positionX, uint8_t positionY)
{
  // if there a boulder at positionX,positionY
  if (vpeek(65536+positionY*128+positionX*2) == STATIONARY_BOULDER) {
    // recursive function call
    checkForBoulder(positionX,positionY-1);
    falling++;
    fallingX[falling] = positionX;
    fallingY[falling] = positionY;
  }
}

bool checkForASpace(uint8_t positionX, uint8_t positionY)
{
  if (vpeek(65536+positionY*128+positionX*2) == EMPTY_TILE) {
    return true;
  } else {
    return false;
  }
}

bool checkForPlayer(uint8_t positionX, uint8_t positionY)
{
  if (vpeek(65536+positionY*128+positionX*2) == PLAYER_TILE) {
    return true;
  } else {
    return false;
  }
}

void moveFallingBoulders()
{
  if (falling==0) return;

  waitvsync();
  tick++;

  // move lowest boulder

  // delete the boulder
  vpoke(EMPTY_TILE,65536+fallingY[falling]*128+fallingX[falling]*2);
  // print boulder at new postion
  vpoke(STATIONARY_BOULDER,65536+(fallingY[falling]+1)*128+fallingX[falling]*2);

  // new boulder postion
  fallingY[falling]++;

  if (checkForASpace(fallingX[falling],fallingY[falling]+1)==false) {
    // if there is no space below, boulder has finished falling

    // if player is below save the boulder position
    if (checkForPlayer(fallingX[falling],fallingY[falling]+1)) {
      fallingSave=falling;
    }

    falling--;
  }

  playDropBoulderSound();
}


void willPickUpDiamond()
{
  playDiamondSound();

  diamondsCollected++;
  score+=100;
  if (diamondsCollected==diamondsRemaining) {
    vpoke(OPEN_DOOR,65536+doorY*128+doorX*2);
  }
  if (diamondsCollected>diamondsRemaining) {
    score+=50;
  }
}


bool moveWillCauseCollision(uint8_t nextX, uint8_t nextY)
{
  uint8_t a;

  a=vpeek(65536+nextY*128+nextX*2);

  if (a==3 | a==4 | a==5 | a==6 | a==7) {
    return true;
  } else if (a==9) {
    willPickUpDiamond();
  } else if (a==8) {
    levelComplete=true;
    playEndLevelSound();
  }
  return false;
}


void moveRight()
{
  uint8_t i;
  uint8_t spriteIndex=0;
  uint8_t s=1,m=0;
  uint16_t hm=0;

  if (moveWillCauseCollision(playerX+1, playerY) == false) {
    checkForBoulder(playerX,playerY-1);

    // put space character at players current position
    vpoke(EMPTY_TILE,65536+playerY*128+playerX*2);

    playerX++;

    // place player at new position
    vpoke(PLAYER_TILE,65536+playerY*128+playerX*2);

    // move screen and player sprite
    hm=(playerX-screenX-1)*16;

    if (playerX < 5 | playerX > 34) {
      s=0;
      m=1;
    }
    if ((playerX-screenX) < 15) {
      s=0;
      m=1;
    }
    if (s==1) screenX++;

    for (i=0; i<8;i++) {
      hscroll=hscroll+s*2;
      hm=hm+m*2;
      // character position
      vpoke(hm & 255, 0x1FC0A);
      vpoke(hm / 256, 0x1FC0B);

      // hscroll
      //VERA.layer0.hscroll = hscroll & 255;
      VERA.layer0.hscroll = hscroll;

      spriteIndex=spriteIndex+8;
      if (spriteIndex==32) spriteIndex=0;
      // change sprite
      vpoke(64+spriteIndex, 0x1FC08);

      waitvsync();
      tick++;
    }
  }
}


void moveLeft()
{
  uint8_t i;
  uint8_t spriteIndex=0;
  uint8_t s=1,m=0;
  uint16_t hm=0;

  if (moveWillCauseCollision(playerX-1, playerY) == false) {
    checkForBoulder(playerX,playerY-1);

    // put space character at players current position
    vpoke(EMPTY_TILE,65536+playerY*128+playerX*2);

    playerX--;

    // place player at position
    vpoke(PLAYER_TILE,65536+playerY*128+playerX*2);

    // move screen and player sprite
    hm=(playerX-screenX+1)*16;

    if (playerX < 4) {
      s=0;
      m=1;
    }
    if ((playerX-screenX) > 3) {
      s=0;
      m=1;
    }
    if (s==1) screenX--;

    for (i=0; i<8;i++) {
      hscroll=hscroll-s*2;
      hm=hm-m*2;
      // character position
      vpoke(hm & 255, 0x1FC0A);
      vpoke(hm/256, 0x1FC0B);

      // hscroll
      //VERA.layer0.hscroll = hscroll & 255;
      VERA.layer0.hscroll = hscroll;

      spriteIndex=spriteIndex+8;
      if (spriteIndex==32) spriteIndex=0;
      // change sprite
      vpoke(96+spriteIndex, 0x1FC08);

      waitvsync();
      tick++;
    }
  }
}


void moveDown()
{
  uint8_t i;
  uint8_t spriteIndex=0;
  uint8_t s=1,m=0;
  uint16_t vm=0;

  if (moveWillCauseCollision(playerX, playerY+1) == false) {
    checkForBoulder(playerX,playerY-1);

    // put space character at players current position
    vpoke(EMPTY_TILE,65536+playerY*128+playerX*2);

    playerY++;

    // place player at position
    vpoke(PLAYER_TILE,65536+playerY*128+playerX*2);

    // move screen and player sprite
    vm=(playerY-screenY-1)*16;

    if (playerY < 5 | playerY > 18) {
      s=0;
      m=1;
    }
    if ((playerY-screenY) < 12) {
      s=0;
      m=1;
    }
    if (s==1) screenY++;

    for (i=0; i<8;i++) {
      vscroll=vscroll+s*2;
      vm=vm+m*2;
      // character position
      vpoke(vm & 255, 0x1FC0C);
      vpoke(vm/256, 0x1FC0D);

      // hscroll
      //VERA.layer0.hscroll = vscroll & 255;
      VERA.layer0.vscroll = vscroll;

      spriteIndex=spriteIndex+8;
      if (spriteIndex==32) spriteIndex=0;
      // change sprite
      vpoke(spriteIndex, 0x1FC08);

      waitvsync();
      tick++;
    }
  }
}


void moveUp()
{
  uint8_t i;
  uint8_t spriteIndex=0;
  uint8_t s=1,m=0;
  uint16_t vm=0;

  if (moveWillCauseCollision(playerX, playerY-1) == false) {

    // put space character at players current position
    vpoke(EMPTY_TILE,65536+playerY*128+playerX*2);

    playerY--;

    // place player at position
    vpoke(PLAYER_TILE,65536+playerY*128+playerX*2);

    // move screen and player sprite
    vm=(playerY-screenY+1)*16;

    if (playerY < 4) {
      s=0;
      m=1;
    }
    if ((playerY-screenY) > 3) {
      s=0;
      m=1;
    }
    if (s==1) screenY--;

    for (i=0; i<8;i++) {
      vscroll=vscroll-s*2;
      vm=vm-m*2;
      // character position
      vpoke(vm & 255, 0x1FC0C);
      vpoke(vm/256, 0x1FC0D);

      // hscroll
      //VERA.layer0.hscroll = vscroll & 255;
      VERA.layer0.vscroll = vscroll;

      spriteIndex=spriteIndex+8;
      if (spriteIndex==32) spriteIndex=0;
      // change sprite
      vpoke(32+spriteIndex, 0x1FC08);

      waitvsync();
      tick++;
    }
  }
}


void showMessage(char *s1)
{
  textcolor(COLOR_LIGHTGREEN);   // green
  gotoxy(6,8); printf("%s", s1);

  sleep(4);

  bgcolor(COLOR_BLACK);     // black background
  clrscr();                 // clear screen

  sleep(1);
}

void showDialogBox(char *s1, char *s2)
{
  uint8_t x,y;

  bgcolor(COLOR_BLACK);     // black background color
  clrscr();                 // clear screen

  for (x=7;x<33;x++) {      // width
    for (y=16;y<39;y++) {   // height
      vpoke(0xdd, 110592+y*128+x*2+1);
    }
  }

  for (x=8;x<32;x++) {      // width
    for (y=18;y<38;y++) {   // height
      vpoke(0xbd, 110592+y*128+x*2+1);
    }
  }

  bgcolor(COLOR_GRAY1);     // light black

  // position 8,9 is top left of dialog

  textcolor(COLOR_LIGHTBLUE);
  gotoxy(12,11); printf("%s", s1);

  textcolor(COLOR_LIGHTGREEN);
  gotoxy(16,13); printf("%s", s2);

  textcolor(COLOR_LIGHTBLUE);
  gotoxy(16,15); printf("%s", "get ready");

  sleep(4);
}

void createStatusLine()
{
  // remove dialog box
  bgcolor(COLOR_BLACK);     // black background color
  clrscr();       // clear screen

  textcolor(COLOR_LIGHTGREEN);
  bgcolor(COLOR_GRAY1);
  //puts("diamonds:       score:       time:");
  puts("diamonds:       score:       time:      ");

  // finally
  moveCharacter();          // move character to start position
  vera_sprites_enable(true);   // enable sprites
}

void loadRoom1()
{
  uint8_t x,y,a;
  char lives[] = " lives  ";

  // set level variables
  levelComplete=false;
  diamondsRemaining=10;
  playerX=3;
  playerY=2;
  doorX=38;
  doorY=16;
  seconds=45;

  // draw room 1
  for (y=0;y<22;y++) {
    for (x=0;x<40;x++) {
      a=room1[y][x];
      vpoke(a,65536+y*128+x*2);
      vpoke(0,65536+y*128+x*2+1);
    }
  }
  lives[7] = livesRemaining + '0';

  showDialogBox("level 1 - warmup", lives);

  createStatusLine();
}

void loadRoom2()
{
  uint8_t x,y,a;
  char lives[] = " lives  ";

  // set level variables
  levelComplete=false;
  diamondsRemaining=85;
  playerX=1;
  playerY=1;
  doorX=38;
  doorY=20;
  seconds=70;

  // draw room 2
  for (y=0;y<22;y++) {
    for (x=0;x<40;x++) {
      a=room2[y][x];
      vpoke(a,65536+y*128+x*2);
      vpoke(COLOR_BLACK,65536+y*128+x*2+1);
    }
  }

  lives[7] = livesRemaining + '0';

  showDialogBox("level 2 - zig zag", lives);

  createStatusLine();
}

void loadRoom3()
{
  uint8_t x,y,a;
  char lives[] = " lives  ";

  // set level variables
  levelComplete=false;
  diamondsRemaining=32;
  playerX=1;
  playerY=1;
  doorX=1;
  doorY=20;
  seconds=60;

  // draw room 3
  for (y=0;y<22;y++) {
    for (x=0;x<40;x++) {
      a=room3[y][x];
      vpoke(a,65536+y*128+x*2);
      vpoke(COLOR_BLACK,65536+y*128+x*2+1);
    }
  }

  lives[7] = livesRemaining + '0';

  showDialogBox("level 3 - maze", lives);

  createStatusLine();
}

void loadRoom4()
{
  uint8_t x,y,a;
  char lives[] = " lives  ";

  // set level variables
  levelComplete=false;
  diamondsRemaining=25;
  playerX=1;
  playerY=1;
  doorX=37;
  doorY=20;
  seconds=100;

  // draw room 4
  for (y=0;y<22;y++) {
    for (x=0;x<40;x++) {
      a=room4[y][x];
      vpoke(a,65536+y*128+x*2);
      vpoke(COLOR_BLACK,65536+y*128+x*2+1);
    }
  }

  lives[7] = livesRemaining + '0';

  showDialogBox("level 4 - dungeon", lives);

  createStatusLine();
}

void loadRoom5()
{
  uint8_t x,y,a;
  char lives[] = "lives  ";

  // set level variables
  levelComplete=false;
  diamondsRemaining=10;
  playerX=1;
  playerY=2;
  doorX=38;
  doorY=20;
  seconds=40;

  // draw room 5
  for (y=0;y<22;y++) {
    for (x=0;x<40;x++) {
      a=room5[y][x];
      vpoke(a,65536+y*128+x*2);
      vpoke(COLOR_BLACK,65536+y*128+x*2+1);
    }
  }

  lives[6] = livesRemaining + '0';

  showDialogBox("level 5 - one way", lives);

  createStatusLine();
}

void checkLevel()
{
  // check game status
  if (seconds==0) {
    livesRemaining--;
    if (livesRemaining==0) {    // start new game
      if (currentLevel!=0) {  // not startup
        vera_sprites_enable(false);   // disable sprites
        showDialogBox("sorry, game over!", "try again!");
      }
      currentLevel=1;
      score=0;
      livesRemaining=3;
      startLevel();
    } else if (dead) {  // restart level
      dead=false;
      startLevel();
    } else {    // restart level
      showMessage("sorry out of time, try again!");
      startLevel();
    }
  } else if (levelComplete) {    // go to next level
    if (currentLevel >= 1) {
      showMessage("level complete, well done!");
    }
    currentLevel++;   // next level
    startLevel();
  }
}

void startLevel()
{
  VERA.layer0.hscroll = 0;
  VERA.layer0.vscroll = 0;
  diamondsCollected=0;
  seconds=0;
  screenX=0;
  screenY=0;
  hscroll=0;
  vscroll=0;

  vera_sprites_enable(false);   // disable sprites

  if (currentLevel==1) {
    // load room1 and set variables
    loadRoom1();
  } else if (currentLevel==2) {
    // load room2 and set variables
    loadRoom2();
  } else if (currentLevel==3) {
    // load room3 and set variables
    loadRoom3();
  } else if (currentLevel==4) {
    // load room4 and set variables
    loadRoom4();
  } else if (currentLevel==5) {
    // load room5 and set variables
    loadRoom5();
  } else {
    // all levels complete - game over

    playDiamondSound();
    showDialogBox("congratulations", "you won!");
    playDiamondSound();
    // reset to start a new game
    seconds=0;
    currentLevel=0;
    livesRemaining=1;
  }
}

void playDiamondSound()
{
  asm("jmp $0400");
}

void playDropBoulderSound()
{
  asm("jmp $0403");
}

void playEndLevelSound()
{
  asm("jmp $0406");
}
