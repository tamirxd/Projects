#pragma once
#include "Snake.h"
#include "snakeGame.h"


Snake::Snake(const Point& head, char c1, Direction dir)
{
    startPoint = head;
    c = c1;
    direction = dir;

    for (int i = size; i < MAX_SNAKE_SIZE - 1; i++)
	body[i] = 0;
}

void Snake::setKeys(char keyLeft, char keyRight, char keyUp, char keyDown, char bulletKey) {
    gameKeys[Direction::LEFT] = keyLeft;
    gameKeys[Direction::RIGHT] = keyRight;
    gameKeys[Direction::UP] = keyUp;
    gameKeys[Direction::DOWN] = keyDown;
    gameKeys[BULLET_KEY] = bulletKey;
}

bool Snake::checkKey(char keyToCheck)		//checks if a given key is related to the snake keys
{
    for (const auto& key : gameKeys)
	if (key == keyToCheck)
	    return true;
    return false;
}

void Snake::changeDir(char keyPressed)
{
    Direction tempDir;
    for (const auto& key : gameKeys) {
	if (key == keyPressed && key == gameKeys[BULLET_KEY])//if we are on the bullet key
	    setBullet();
	if (key == keyPressed && key != gameKeys[BULLET_KEY])		//if we got and key and its not a shooting key
	{
	    tempDir = (Direction)(&key - gameKeys);
	    if (checkSnakeDirOverride(tempDir))
	    {

		direction = (Direction)(&key - gameKeys);
		return;
	    }
	}
    }
}


void Snake::setBullet()
{
    Point tempHead;
    int next_x, next_y;
    if (shotsFired == SIZE_OF_MAGAZINE)			//the snake has no more bullets in the magazine
	return;
    else
    {
	makeNextMove(tempHead, next_x, next_y);
	enableBullet(next_x, next_y);
    }
}

void Snake::enableBullet(int next_x, int next_y)
{
    magazine[shotsFired].changeDir(direction);		//gets the snake's direction
    magazine[shotsFired].setXY(next_x, next_y);		//the bullet will start in the next x y
    magazine[shotsFired].setOnOff(true);
    magazine[shotsFired].setFirstMove(true);
    shotsFired++;
}

void Snake::makeNextMove(Point& tempPoint, int& next_x, int& next_y)
{
    tempPoint.changeDir(direction);
    next_x = body[0].getX() + tempPoint.getDirX();			//The next x ,y of the snake
    next_y = body[0].getY() + tempPoint.getDirY();
    updateNextXY(next_x, next_y);
}


bool Snake::checkSnakeDirOverride(Direction tempDir) //Checks if an opposite direction to the current direction was used
{
    if ((tempDir == LEFT && direction == RIGHT) || (tempDir == RIGHT && direction == LEFT) || (tempDir == DOWN && direction == UP) || (tempDir == UP && direction == DOWN))
	return false;
    return true;
}

Point Snake::move(char keyPressed)
{
    Point tempHead = body[0];							//Will be used to get the next direction without changing it
    int next_x, next_y;
    bool isFree = true;
    int snakeSize = getSize();
    changeDir(keyPressed); //Need to somehow give temphead the next x,y ,and change the direction of the snake in the loop
    makeNextMove(tempHead, next_x, next_y);

    if (screen[next_y][next_x] != ' ')
    {
	isFree = false;
	if (screen[next_y][next_x] == '!' || screen[next_y][next_x] == '$')
	    getPunished();
	else if (screen[next_y][next_x] == '%')
	{
	    getPunished();
	    snakeGame->numberEaterHitPlayer(getSnakeChar());
	    snakeGame->endOfTurnActions();
	}
    }
    if (isFree && checkSnakeDirOverride(direction))			//Not wokring when the other snake hits the locked snake.
    {
	if (body[snakeSize - 1].getY() >= 4)				//For the first deletion only, need to find a better solution
	    body[snakeSize - 1].draw(' ');
	for (int i = snakeSize - 1; i > 0; --i)
	    body[i] = body[i - 1];
	body[0].move(direction);
	body[0].draw(c);
    }
    return body[0];
}

void Snake::reducePunishment()				//reduces the punishment timer of a player by 1
{
    punishTimer--;
    if (punishTimer <= 0)					//if no punishment time is left, return the snake to a random location
	randomizeNewSnakeCoord();
}

void Snake::randomizeNewSnakeCoord()		//randomizes a new location for the punished snake
{
    int randY, randX;
    bool isCoordEmpty = false;
    while (isCoordEmpty == false)
    {
	randY = rand() % 20 + 4;
	randX = rand() % 79 + 1;
	isCoordEmpty = checkRandSnakeCoords(randX, randY);
    }
    newSnakePosition(randX, randY);			//implement the new location
}

void Snake::newSnakePosition(int newX, int newY)		//implements the new snakes location
{
    direction = DOWN;
    changeDir(direction);
    body[0] = Point(newX, newY);
    body[0].draw(c);

    for (int i = size; i < MAX_SNAKE_SIZE - 1; i++)
	body[i] = 0;
}

bool Snake::checkRandSnakeCoords(int x, int y)	//checks for a clear area for the snake to respawn
{
    bool isOk = true;
    for (int i = -1; i < 4; i++)
	isOk = isOk && (screen[y][x + i] == ' ' && screen[y - 1][x] == ' ' && screen[y + 1][x] == ' ');
    return isOk;
}

void Snake::deleteSnakeFromScreen()
{
    int i;
    for (i = 0; i < getSize(); i++)
	body[i].draw(' ');
}

void Snake::drawSnakeOnScreen()
{
    int i;
    for (i = 0; i < getSize(); i++)
	body[i].draw(getSnakeChar());
}

void Snake::printBullets()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char ch = 254;
    SetConsoleTextAttribute(hConsole, LIGHTÉGREY);

    for (int i = SIZE_OF_MAGAZINE; i > shotsFired; i--)
	cout << ch << " ";

    SetConsoleTextAttribute(hConsole, GREY);
    for (int i = 0; i < shotsFired; i++)
	cout << ch << " ";
}



void Snake::resetMagazine()		//reset all magazine bullets to off
{
    for (int i = 0; i < SIZE_OF_MAGAZINE; i++)
	magazine[i].setOnOff(false);
}

void Snake::moveBullets()			//move all active bullets
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    bool bulletCourse = true;
    for (int j = 0; j < BULLET_SPEED; j++)
    {
	for (int i = 0; i < SIZE_OF_MAGAZINE; i++)
	    if (magazine[i].isItOn())
	    {
		if (magazine[i].isFirstMove())					//Not the first move of the bullet
		    magazine[i].setFirstMove(false);
		else
		    magazine[i].draw(' ');

		magazine[i].moveImpl();
		bulletCourse = checkBulletHit(magazine[i].getX(), magazine[i].getY(), i);	//checks for bullets hit

		if (bulletCourse)					//if the bullet course is clean
		    magazine[i].draw(magazine[i].getBulletChar(), getSnakeChar());
	    }
    }
}

bool Snake::checkBulletHit(int x, int y, int shotIndex) //NEED TO CHANGE THIS TO SWITCH CASE
{
    int checkIfDeleted;
    if (screen[y][x] == ' ')         //if the next coord of the bullet if free
	return true;
    else                      //if the bullet hit something
    {
	if (screen[y][x] >= '0' && screen[y][x] <= '9')   //we hit a number
	{
	    checksCurrentSnakeBullet(x, y);
	    checkIfDeleted = snakeGame->eatNumber(x, y);
	}
	char screenChar = screen[y][x];
	switch (screenChar)
	{
	    case '*':
		deleteSnakesBullet(x, y);
		break;
	    case '!':
		checksCurrentSnakeBullet(x, y);
		break;
	    case '$':
		hanldeCreatureBulletHit(x, y);
		break;
	    case '%':
		hanldeCreatureBulletHit(x, y);
		break;
	    default:
		handleSnakeHit(x, y);
	        break;
	}
	Point(x, y).draw(' ');						//draw a space in the hit coord
    }
    return false;

}

void Snake::hanldeCreatureBulletHit(int x, int y)//the creatures are colFliers and numberEaters
{
    checksCurrentSnakeBullet(x, y);
    snakeGame->deactivateCreature(x, y);
    shotsFired--;
}


void Snake::handleSnakeHit(int x, int y)
{
    checksCurrentSnakeBullet(x, y);
    if (screen[y][x] == '@')		//if the next point is the first snake
    {
	snakeGame->hideSnake(FIRST_SNAKE);
	if (getSnakeChar() != '@')//the current snake hit itself
	    shotsFired--;						//Add a bullet to the magazine
    }
    else if (screen[y][x] == '#')
    {
	snakeGame->hideSnake(SECOND_SNAKE);
	if (getSnakeChar() != '#')//the current snake hit itself
	    shotsFired--;						//Add a bullet to the magazine
    }
}

void Snake::getPunished()	//Delete snake from screen and add a 5 turns punishment
{
    int i;
    punishTimer = MAX_PUNISH_TIME;						//Total of 5 random numbers // enum
    for (i = 0; i < size; i++)
	body[i].draw(' ');
}


void Snake::deleteSnakesBullet(int x, int y)
{
    checksCurrentSnakeBullet(x, y);			//delete the current snake's bullet
    checksOtherSnakeIndex(x, y);			//delete the other snake's bullet

}

bool Snake::checksCurrentSnakeBullet(int x, int y)				//handle current snake's bullet array
{
    for (int i = 0; i < SIZE_OF_MAGAZINE; i++)
    {
	if (magazine[i].getX() == x && magazine[i].getY() == y)
	{
	    magazine[i].setOnOff(false);
	    return true;
	}
    }
    return false;
}

void Snake::checksOtherSnakeIndex(int x, int y)					//handle the other snake bullet array
{
    if (getSnakeChar() == '@')
	snakeGame->checkOtherSnakeBullet(SECOND_SNAKE, x, y);
    else
	snakeGame->checkOtherSnakeBullet(FIRST_SNAKE, x, y);

}

void Snake::updateNextXY(int &next_x, int & next_y)
{
    if (next_x == 0)				//Checks if the snake head reached screen left border
	next_x = max_x;
    else if (next_x == max_x + 1)		//Checks if the snake head reached screen right border
	next_x = min_x;
    if (next_y == min_y)					//Checks if the snake head reached screen upper border
	next_y = max_y;									//Sets the snakes head in the other border
    else if (next_y == max_y + 1)			//Checks if the snake head reached screen lower border
	next_y = min_y + 1;
}

void Snake::resetSnakeValues()
{
    resetSize();
    resetMagazine();
    resetShotsFired();
    resetPunishmentStatus();
    newSnakePosition(getStartX(), getStartY());
}

void Snake::resetPunishmentStatus()
{
    punishTimer = 0;
}



