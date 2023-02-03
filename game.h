// defines
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
