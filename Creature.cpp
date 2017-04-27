#include "Creature.h"
#include "snakeGame.h"
void Creature::creatureHitNumber(int x, int y)
{
	bool creatureAllwaysWin;
	int num;
	num = game->eatNumber(x, y);
	game->creatureEatNumberActions();
}

void Creature::setGamePtr(TheSnakeGame* ptr) { game = ptr; }

int Creature::checkBulletSource(int nextX, int nextY)
{
	int i;
	bool isDeleted = false;
	for (i = 0; i < 2; i++)
	{
		isDeleted = snakes[i].checksCurrentSnakeBullet(nextX, nextY);
		if (isDeleted)
			return i;
	}
}

void Creature::resetCreature()
{
	deletePreviousStep();
	body = startPoint;
	direction = defaultDir;
	body.changeDir(defaultDir);
	drawSymbol();
	if (typeid(this) == typeid(NumberEater))
		static_cast<NumberEater*>(this)->setHunger(false);				//reset numbereater target chase to false
	else if (typeid(this) == typeid(ColFlier))
		static_cast<ColFlier*>(this)->setOnOff(true);		//return colflier to screen
}

void Creature::searchNumberEaterRoute()
{
	game->searchShortestRoute();
}


void Creature::setSnakesPtr(Snake * ptr)
{
	snakes = ptr;
}

void Creature::creatureHitSnake(int snakeInd)
{
	game->hideSnake(snakeInd);
}

Point Creature::implBodyChangesCrossBorders()
{
	body.moveImpl();
	return body;
}

Point Creature::implBodyChangesInBorders()		//Adjusts body changes to creatures with border limits
{
	body.bounceBackImpl();
	return body;
}

void Creature::addBulletToSnake(int snakeInd)
{
	snakes[snakeInd].addBulletToMag();
}
bool Creature::handleNotSpace(int nextX, int nextY) {				//we are here because next x and next y are not space
	int snakeHitInd;
	if (screen[nextY][nextX] == '@')				//its a snake
	{
		if (typeid(this) == typeid(NumberEater))//if its the number eater that hit a snake
		{
			game->numberEaterHitPlayer(snakes[FIRST_SNAKE].getSnakeChar());;//increases the other snake's body
			game->endOfTurnActions();//ends the current turn
		}
		else
			creatureHitSnake(FIRST_SNAKE);
	}
	else if (screen[nextY][nextX] == '#')
	{
		if (typeid(this) == typeid(NumberEater))//if its the number eater that hit a snake
		{
			game->numberEaterHitPlayer(snakes[SECOND_SNAKE].getSnakeChar());;//increases the other snake's body
			game->endOfTurnActions();//ends the current turn
		}
		else
			creatureHitSnake(SECOND_SNAKE);
	}
	else if (screen[nextY][nextX] >= '0' && screen[nextY][nextX] <= '9')			//its a number
	{
		creatureHitNumber(nextX, nextY);
		if (dynamic_cast<NumberEater*>(this))
			static_cast<NumberEater*>(this)->resetTarget();
	}
	else if (screen[nextY][nextX] == '*')				//its a bullet
	{
		snakeHitInd = checkBulletSource(nextX, nextY);
		if (typeid(this) == typeid(ColFlier) || typeid(this) == typeid(NumberEater))
		{
			addBulletToSnake(snakeHitInd);
			if (typeid(this) == typeid(NumberEater))
				static_cast<NumberEater*>(this)->setOnScreen(false);
			else
				static_cast<ColFlier*>(this)->setOnOff(false);
		}
	}
	else if (screen[nextY][nextX] == '$')
		return true;
	return false;
}

