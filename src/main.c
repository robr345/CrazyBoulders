#include <stdint.h>
#include <stdio.h>
#include <conio.h>
#include <cbm.h>
#include <joystick.h>
#include <stdbool.h>
#include <unistd.h>
#include "load.h"
#include "vload.h"

#include "game.h"

extern uint8_t tick;
extern uint16_t seconds;
extern uint16_t score;
extern uint8_t fallingSave;
extern uint8_t livesRemaining;

extern uint8_t diamondsCollected;
extern uint8_t diamondsRemaining;

extern bool dead;


void fileNotFoundError(char *filename)
{
  // change VERA config so the error message can be displayed
  VERA.layer1.config    = 0b01100000;
  VERA.layer1.tilebase  = 0b11111000;

  bgcolor(COLOR_BLUE);
  textcolor(COLOR_WHITE);
  videomode(VIDEOMODE_80COL);

  printf("error, %s file not found", filename);
  printf("\nprogram exiting, cannot continue");
}

int main(void)
{
  bool update=false;
  uint16_t size;
  uint8_t joy;

  // set up the joystick
  joy_install(cx16_std_joy);

  // Switch back to the uppercase character set.
  cbm_k_bsout(CH_FONT_UPPER);

  clrscr(); // clear screen


  // **** display the titlescreen ****

  size = vload_host("titlescreen.bin",0x00000);
  if (size==0) {
    fileNotFoundError("titlescreen.bin");
    return 1;
  }

  VERA.display.hscale   = 64;
  VERA.display.vscale   = 64;
  VERA.layer1.config    = 0b00011111;
  VERA.layer1.tilebase  = 0;

  sleep(4);

  // **** end display tilescreen ****

  // load the VERA
  size = vload_host("lightfont.bin",0x06000);
  if (size==0) {
    fileNotFoundError("lightfont.bin");
    return 1;
  }
  size = vload_host("tilesheet.bin",0x12000);
  if (size==0) {
    fileNotFoundError("tilesheet.bin");
    return 1;
  }
  size = load_file_host("effects.prg",0x0400);
  if (size==0) {
    fileNotFoundError("effects.prg");
    return 1;
  }
  size = vload_host("sprite.bin",0x04000);
  if (size==0) {
    fileNotFoundError("sprite.bin");
    return 1;
  }

  // set up screen
  VERA.layer0.config    = 0b00010011;   // layer 0, 64x32 tiles 8bpp mode
  VERA.layer0.mapbase   = 128;          // map base at 0x10000
  VERA.layer0.tilebase  = 0b10010011;   // tile base at 0x12000 16x16 pixel tiles
  VERA.layer1.config    = 0b01100000;   // layer 1 128x64 tiles 2 color 1 bpp
  VERA.layer1.tilebase  = 0b00110000;   // tile base at 0x06000, 8x8 pixel tiles
  VERA.control          = 0b00000000;   // control address select 0
  //VERA.display.video    = 0b01100001;   // video sprites on, layer 0 on
  VERA.display.hscale   = 64;
  VERA.display.vscale   = 64;
  VERA.layer0.hscroll   = 0;
  VERA.layer0.vscroll   = 0;

  // A value of 0b01 shows layer 0, a value of 0b10 shows layer 1
  vera_layer_enable(0b11);

  // set up sprite 1
  VERA.address_hi = 0x11;   // inc by 1, memory start at 0x10000
  VERA.address = 0xFC08;    // full address 0x1FC08
  VERA.data0 = 0x0000;      // %00010000
  VERA.data0 = 0x82;        // 256 color mode address at 0x04000
  VERA.data0 = 100;         //  X position
  VERA.data0 = 0;           //  X position
  VERA.data0 = 100;         //  Y position
  VERA.data0 = 0;           //  Y position
  VERA.data0 = 0b00001100;  // in front
  VERA.data0 = 0b01010000;  // 16x16 pixel pallette offset 0


  while(true) {     // game loop

    checkLevel();   // check game status

    waitvsync();
    tick++;

    if (tick>=60) {
      tick=0;
      seconds--;
    }

    moveFallingBoulders();

    update=!update;
    if (update) {   // every other vsync
      gotoxy(10,0); printf("%d/%d", diamondsCollected, diamondsRemaining);
      gotoxy(23,0); printf("%d", score);
      gotoxy(35,0); printf("%d ", seconds);
    }

    joy = joy_read(0);

    if (joy & JOY_RIGHT_MASK) {
      moveRight();
    } else if (joy & JOY_LEFT_MASK) {
      moveLeft();
    } else if (joy & JOY_DOWN_MASK) {
      moveDown();
    } else if (joy & JOY_UP_MASK ) {
      moveUp();
    } else {
      if (fallingSave) {  // kill the player
        playDropBoulderSound();
        showMessage("you have been squished dead");
        dead=true;
        seconds=0;
      }
    }
    fallingSave=0;
  }

  return 0;

}
