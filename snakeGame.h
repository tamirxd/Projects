#pragma once
#ifndef _THESNAKEGAME_H_
#define _THESNAKEGAME_H_

#include <iostream>
#include <Windows.h>
#include <conio.h>
#include "snake.h"
#include "Random.h"
#include "Mission.h"
#include "gotoXY.h"
#include "Colors.h"
#include "Creature.h"
#include "LineFlier.h"
#include "numberEater.h"
#include "ColFlier.h"
#include <typeinfo>
#include "Replay.h"

#define MENU_LINES 4
#define MAX_RAND_SIZE 60
#define STEPS_TO_MOVE_SNAKE 5
#define MAX_SNAKE_SIZE 15
#define NUMBER_OF_CREATURES 5
#define NO_VALUE -1
#define MAX_LINES 24
#define MAX_COLS 81
#define BULLET_SPEED 2
#define ROWFLIER_SPEED 2
#define COLFLIER_SPEED 1

using namespace std;
extern char screen[24][81];
enum KEYS { ESC = 27 };

class TheSnakeGame {
	enum { EXIT = '0', CONTINUE = '3', START_OVER = '4', NEW_MISSION = '5', NEW_GAME = '6', INSTRUCTIONS = '1',REPLAY = '7' };
	enum { arrSize = 2, FIRST_SNAKE = 0, SECOND_SNAKE = 1};
	enum { INIT_SNAKE_SIZE };
	Random randNum[MAX_RAND_SIZE];					//Random numbers array
	int logSize = 0;					//Randoms numbers generated last index
	int lastDeletedRandInd = -1;				//Last deleted random number array index
	int steps = 0;						//Steps taken by the players
	int missionIndex;
	bool replay = false;
	bool missionsFromFile = false;
	Replay lastTurnReplay;
	Creature* creatures[NUMBER_OF_CREATURES];
	Snake snakes[arrSize] = {
		{ { 10, 9 }, '@',DOWN }, // snake 1 right
		{ { 70, 9 }, '#',DOWN }, // snake 2	left
	};
	LineFlier lineFliers[arrSize] = {
		{ { 30, 23 }, RIGHT },		//right flier
		{ { 50, 15 }, LEFT } };		//left flier
	ColFlier colFliers[arrSize] = {
		{ { 45, 23 }, UP },			//right flier
		{ { 55, 15 }, DOWN } };		//left flier
	NumberEater numberEater{ { 10,19 } };
public:
	int menu();
	int handlePressedKey(char keyPressed);
	int submenu();
	void printScoreBoard();
	void deleteAllNumbersFromArr();
	void deactivateCreature(int x, int y);
	int eatNumber(int next_x, int next_y);
	void deletePrintedNum(Random * toDelete);
	void sortArray(int size);
	void replaceArry(Random* tempArr, int size);
	void copyArray(Random* tempArr, int physSize);
	void hideSnake(int snakeInd);
	void checkOtherSnakeBullet(int snakeIndex, int x, int y);
	void printAllUsedNums();
	void deleteHalfNumbers();
	void checkSnakesPunishmentStatus();
	int handleEscKey(int decision);
	TheSnakeGame();
	void run();
	void initiateReplay();
	void printWinningMessege();
	void setGamePtrToSnakesAndCreatures();
	void playWinningMusic();
	void resetGameParameters();
	void resetScreen();
	void fullBoard();
	bool nextGameStep(char keyPressed);
	void moveNumberEater();
	void searchShortestRoute();
	void moveLineFliers();
	void moveColFliers();
	void numIsEatenActions(bool & winner, int & numEaten, int snakeInd);
	void creatureEatNumberActions();
	void endOfTurnActions();
	void numberEaterHitPlayer(char playerChar);
	void printWrongAnswerCase();
	int getLogSize() { return logSize; }
	int getSteps() { return steps; }
};

#endif


