#ifndef _SNAKE_H_
#define _SNAKE_H_

#include "Point.h"
#include <Windows.h>
#include <conio.h>
#include "Direction.h"
#include "Bullet.h"

#define min_x 1
#define max_x 79
#define min_y 3
#define max_y 23
#define BULLET_SPEED 2
#define MAX_PUNISH_TIME 5
#define MAX_SNAKE_SIZE 15
#define GAME_KEYS 5
#define SIZE_OF_MAGAZINE 5

extern char screen[24][81];
class TheSnakeGame;

class Snake {
    enum { FIRST_SNAKE = 0, SECOND_SNAKE = 1};
    enum { INIT_SNAKE_SIZE = 3,BULLET_KEY = 4 };
    Point body[MAX_SNAKE_SIZE];						//The body of the snake
    Direction direction;				//Current direction of the snake
    int size = INIT_SNAKE_SIZE;						//Current snake size
    char c;								//The snakes visual printed char
    char gameKeys[GAME_KEYS];					// the keys for the four possible directions
    Bullet magazine[SIZE_OF_MAGAZINE];
    int shotsFired = 0;
    TheSnakeGame * snakeGame;
    int punishTimer = 0;
    Point startPoint;
public:
    Snake(const Point& head, char c1, Direction dir = Direction::DOWN);
    void setKeys(char keyLeft, char keyRight, char keyUp, char keyDown, char bulletKey);
    bool checkKey(char keyToCheck);
    void changeDir(char keyPressed);
    void setBullet();

    void enableBullet(int next_x, int next_y);

    void makeNextMove(Point & tempPoint, int & next_x, int & next_y);

    bool checkSnakeDirOverride(Direction tempDir);
    Point move(char keyPressed);
    void moveBullets();
    bool checkBulletHit(int x, int y, int shotIndex);
    void hanldeCreatureBulletHit(int x, int y);
    void handleSnakeHit(int x, int y);
    void getPunished();
    void deleteSnakesBullet(int x, int y);
    bool checksCurrentSnakeBullet(int x, int y);
    void checksOtherSnakeIndex(int x, int y);
    void updateNextXY(int & next_x, int & next_y);
    void resetSnakeValues();
    void resetPunishmentStatus();
    int getDirX() { return body[0].getDirX(); }
    int getDirY() { return body[0].getDirY(); }
    Direction getDirection() { return direction; }
    char getSnakeChar() const { return c; }
    int getSize() { return size; }
    int getStepsToPunishment() { return punishTimer; }
    int getStartX() { return startPoint.getX(); }
    int getStartY() { return startPoint.getY(); }
    void printBullets();
    void resetMagazine();
    void resetShotsFired() { shotsFired = 0; }
    Point getBody() { return body[0]; }

    void resetSize() { size = INIT_SNAKE_SIZE; }	//Resets the snake size (new game)

    bool increaseBody(int _size)	//Increases snake size after eating a number
    {								//Returns true if the snake reached the maximum size
	if (_size < MAX_SNAKE_SIZE - 1)
	    size++;
	else
	    return true;
	return false;
    }
    bool isPunished()
    {
	if (punishTimer > 0)
	    return true;
	return false;
    }
    void setGamePtr(TheSnakeGame* ptr) { snakeGame = ptr; }
    void reducePunishment();
    void randomizeNewSnakeCoord();
    void newSnakePosition(int newX, int newY);
    bool checkRandSnakeCoords(int x, int y);
    void addBulletToMag() { shotsFired--; }
    void deleteSnakeFromScreen();
    void drawSnakeOnScreen();
};
#endif
