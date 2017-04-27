#pragma once
#ifndef _CREATURE_H_
#define _CREATURE_H_
#include "Point.h"
#include <iostream>
#include <stdlib.h>

class TheSnakeGame;
class Snake;
class Creature {
	char symbol;
	Point body;
	Direction direction;
	TheSnakeGame * game;
	Snake *snakes;
	Point startPoint;
	Direction defaultDir;
public:
	enum { max_x = 79, min_x = 1, min_y = 3, max_y = 23, FIRST_SNAKE = 0, SECOND_SNAKE = 1,FIRST_FLIER=0,SECOND_FLIER=1 };
	Creature(const Point& startingPoint, char c, Direction _direction) : body(startingPoint), symbol(c), direction(_direction) 
	{
		startPoint = startingPoint;
		defaultDir = _direction;
		body.changeDir(_direction);
	}
	virtual void move() = 0;
	bool handleNotSpace(int x, int y);
	Point implBodyChangesCrossBorders();
	Point implBodyChangesInBorders();
	void addBulletToSnake(int snakeInd);
	void setGamePtr(TheSnakeGame* ptr);
	void setSnakesPtr(Snake* ptr);
	void creatureHitSnake(int snakeInd);
	void creatureHitNumber(int x, int y);
	void deletePreviousStep()	{	body.draw(' ');}
	void drawSymbol() { body.draw(symbol); }
	Point getBody() { return body; }
	int checkBulletSource(int nextX, int nextY);
	void resetCreature();

	void searchNumberEaterRoute();
	void changeCreatureDir(Direction _direction) { body.changeDir(_direction); }
};

#endif
