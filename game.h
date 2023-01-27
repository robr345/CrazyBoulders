/*

  The original version of Crazy Boulders by Dustan Strakl was programmed
  in Basic see https://github.com/Dooshco/X16-CrazyBoulders. Dustan has
  lots of useful information on programming the X16 see his readme file.

  This C version is an exercise in writing a game in C using the cc65
  compiler. Using Dustan's game design made it easer to consentrate on
  the coding while learning how game mechanics work.

  This programme uses the excelent libX16.lib by Chris Love to provide
  vload and load_file functions to provide the C equivilent of the
  basic commands VLOAD and LOAD.

  This programme also uses the EFFECTS.PRG by Dustan Strakl to provide
  sound effects.

*/

// tile defines
#define EMPTY_TILE            0
#define PLAYER_TILE           1
#define DIRT_FLOOR            2
#define STATIONARY_BOULDER    3
#define FALLING_BOULDER       4
#define STONE_WALL            5
#define BRICK_WALL            6
#define CLOSED_DOOR           7
#define OPEN_DOOR             8
#define DIAMOND               9

// function prototypes
void playDiamondSound();
void playDropBoulderSound();
void playEndLevelSound();

void moveCharacter();
void checkForBoulder(uint8_t positionX, uint8_t positionY);
bool checkForASpace(uint8_t positionX, uint8_t positionY);
bool checkForPlayer(uint8_t positionX, uint8_t positionY);
void moveFallingBoulders();
void willPickUpDiamond();
bool moveWillCauseCollision(uint8_t nextX, uint8_t nextY);

void moveRight();
void moveLeft();
void moveDown();
void moveUp();

void showMessage(char *s1);
void showDialogBox(char *s1, char *s2);
void createStatusLine();


void loadRoom1();
void loadRoom2();
void loadRoom3();
void loadRoom4();
void loadRoom5();

void checkLevel();
void startLevel();
