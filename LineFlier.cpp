#pragma once
#include "LineFlier.h"
#include "Creature.h"
#include "snakeGame.h"

void LineFlier::move() 
{
	bool onEdge = false, creatureBlock = false;
	Point nextPoint = getBody();
	if (lineFlierInd == FIRST_FLIER)
		nextPoint.moveImpl();
	else
	{
		nextPoint.bounceBackImpl();
		if (nextPoint.getX() == min_x || nextPoint.getX() == max_x)
			onEdge = true;
	}

	if (screen[nextPoint.getY()][nextPoint.getX()] != ' ' && onEdge == false)//if we have a number\bullet\snake
		creatureBlock =	handleNotSpace(nextPoint.getX(), nextPoint.getY());
	if (!creatureBlock)
	{
		if (lineFlierInd == FIRST_FLIER)//we are with the first fly, which goes right
			moveRightFly();
		else
			moveLeftFly();
	}
}

void LineFlier::moveLeftFly()		//this flier cannot move thourgh borders
{
	deletePreviousStep();
	implBodyChangesInBorders();
	drawSymbol();
}


void LineFlier::moveRightFly()//updates according to the borders. this flier can move through the screen
{
	deletePreviousStep();
	implBodyChangesCrossBorders();
	drawSymbol();
}





