#pragma once
#ifndef _LINEFLIER_
#define _LINEFLIER_
#include "Creature.h"
#include "Direction.h"



class LineFlier : public Creature
{
	int lineFlierInd;
public:
	LineFlier(Point startingPoint, Direction movingDirection) : Creature(startingPoint, '!', movingDirection)
	{
		if (startingPoint.getX() == 30)//if its the first fly
			lineFlierInd = FIRST_FLIER;
		else
			lineFlierInd = SECOND_FLIER;
	}
	virtual void move();
	void moveLeftFly();
	void moveRightFly();

};



#endif
