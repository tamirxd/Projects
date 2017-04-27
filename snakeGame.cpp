#include "snakeGame.h"
static Mission mission;
static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


TheSnakeGame::TheSnakeGame()
{
    snakes[FIRST_SNAKE].setKeys('a', 'd', 'w', 'x', 'z');
    snakes[SECOND_SNAKE].setKeys('j', 'l', 'i', 'm', 'n');
    resetScreen();						//reset the screen array to empy spaces
    SetConsoleTextAttribute(hConsole, GREY);	//the default text color is grey
    for (int i = 0; i < arrSize; i++)			//set creatures pointers in array
    {
	creatures[i] = &lineFliers[i];
	creatures[i + 2] = &colFliers[i];
    }
    creatures[NUMBER_OF_CREATURES - 1] = &numberEater;
    setGamePtrToSnakesAndCreatures();
}

void TheSnakeGame::run()
{
    bool winner = false;				//If the one of the players has rechead 15 points
    int decision;
    decision = menu();//decision is EXIT GAME || INSTRUCTIONS || NEW GAME

    while (true)
    {
	SetConsoleTextAttribute(hConsole, GREY);//the default text color is grey
	if (winner)
	{
	    winner = false;
	    resetGameParameters();
	    decision = menu();
	}
	char keyPressed = 0;
	if (replay)
	{
	    keyPressed = KEYS::ESC;
	    replay = false;
	    printAllUsedNums();
	}
	if (decision == EXIT)//user wants to exit
	    break;
	if (decision == INSTRUCTIONS)
	{
	    char esc = _getch();//waiting for an ESC after reading inst'
	    decision = menu();
	}

	if (_kbhit() || keyPressed == KEYS::ESC)
	{
	    if (!(keyPressed == KEYS::ESC))
		keyPressed = _getch();
	    if (keyPressed == KEYS::ESC)
	    {
		decision = submenu();
		if (replay)
		    continue;
		if (decision == EXIT)
		    break;
		else
		{
		    decision = handleEscKey(decision);
		    if (decision == INSTRUCTIONS)
		    {
			char esc = _getch();//waiting for an ESC after reading inst'
			decision = menu();
		    }
		}

	    }
	}
	else
	    keyPressed = 0;
	Sleep(50);
	winner = nextGameStep(keyPressed);
	decision = NO_VALUE;

	if (winner)
	    printWinningMessege();
    }
}

void TheSnakeGame::initiateReplay()		//set the random array to the start of the turn numbers. set snakes to the start of the turn locations
{
    lastTurnReplay.printScreens();
    Sleep(200);
}



void TheSnakeGame::printWinningMessege()
{
    char winner;
    if (snakes[FIRST_SNAKE].getSize() == MAX_SNAKE_SIZE)
	winner = snakes[FIRST_SNAKE].getSnakeChar();
    else
	winner = snakes[SECOND_SNAKE].getSnakeChar();
    clrscr();
    gotoxy(1, 1);
    cout << "Player " << winner << " has won the game! well done! Returning to main Menu";
    playWinningMusic();
    Sleep(5000);
}

void TheSnakeGame::setGamePtrToSnakesAndCreatures()	//sets all needed inter-class pointers
{
    for (int i = 0; i < arrSize; i++)			//set ptr of snakegame to the snakes
	snakes[i].setGamePtr(this);
    for (int i = 0; i < NUMBER_OF_CREATURES; i++)		//set ptr of snakeGame to all the creatures
    {
	creatures[i]->setGamePtr(this);
	creatures[i]->setSnakesPtr(snakes);
    }
}

void TheSnakeGame::playWinningMusic()
{
    Beep(523, 400); // 523 hertz (C5) for 400 milliseconds
    Beep(587, 400);
    Beep(659, 400);
    Beep(698, 400);
    Beep(784, 400);
    Beep(698, 400);
    Beep(659, 400);
    Beep(587, 400);
    Beep(523, 400);
}


void TheSnakeGame::resetGameParameters()		//reset all game parameters to default-for a new game
{
    deleteAllNumbersFromArr();
    resetScreen();
    logSize = 0;
    steps = 0;
    lastDeletedRandInd = NO_VALUE;
    _flushall();
    clrscr();
    for (int i = 0; i < arrSize; i++)
	snakes[i].resetSnakeValues();
    for (int i = 0; i < NUMBER_OF_CREATURES; i++)
    {
	creatures[i]->resetCreature();
	if (typeid(*creatures[i]) == typeid(ColFlier))
	    if (!(static_cast<ColFlier*>(creatures[i])->isOnScreen()))
		static_cast<ColFlier*>(creatures[i])->setOnOff(true);
    }
    lastTurnReplay.emptyScreens();
    numberEater.resetNumberEater();
}


void TheSnakeGame::resetScreen()
{
    for (int i = 0; i < MAX_LINES; i++)
	for (int j = 0; j < MAX_COLS; j++)
	    screen[i][j] = ' ';
}

void TheSnakeGame::fullBoard()			//When the board is full- 60 numbers on board
{
    gotoxy(1, 3);
    SetConsoleTextAttribute(hConsole, GREY);
    cout << "There are 60 numbers on the screen. checking for a solution...";
    mission.checksSolutions(randNum, logSize, missionIndex, true);
    deleteHalfNumbers();
    mission.randomMission(missionIndex);
}

bool TheSnakeGame::nextGameStep(char keyPressed)
{
    Direction tempDir;
    bool winner;
    int numEaten;
    Point pos;
    winner = false;
    printScoreBoard();
    moveColFliers();

    for (size_t i = 0; i < arrSize; ++i)
    {
	moveLineFliers();
	moveNumberEater();
	snakes[i].moveBullets();		//Will move bullets twice as fast compared to the snakes speed
	numEaten = NO_VALUE;
	steps++;
	Sleep(50);
	if (steps == STEPS_TO_MOVE_SNAKE && logSize <= MAX_RAND_SIZE - 1)
	{

	    if (lastDeletedRandInd == NO_VALUE)
		randNum[logSize].createRandom(randNum, logSize, snakes, arrSize, lastDeletedRandInd);
	    else
	    {
		randNum[lastDeletedRandInd].createRandom(randNum, logSize, snakes, arrSize, lastDeletedRandInd);
		lastDeletedRandInd = NO_VALUE;
	    }
	    logSize++;
	    steps = 0;
	    checkSnakesPunishmentStatus();   //Checks and Reduces the snake punishment timer if needed
	}
	if (logSize == MAX_RAND_SIZE)		//If  there is 60 numbers on board
	    fullBoard();

	// move and check if the snake ate a number
	//checks if in the next coord there is an other snakes body	,return true if there isnt
	if (!snakes[i].isPunished())
	{
	    pos = snakes[i].getBody();				//checks and makes the next step
	    tempDir = snakes[i].getDirection();
	    pos.changeDir(tempDir);
	    pos.moveImpl();
	    numEaten = eatNumber(pos.getX(), pos.getY());

	    pos = snakes[i].move(keyPressed);
	}
	if (numEaten != NO_VALUE)					//the snake aàe a number
	    numIsEatenActions(winner, numEaten, i);
	lastTurnReplay.addScreen(snakes[FIRST_SNAKE].getSize() - 3, snakes[SECOND_SNAKE].getSize() - 3);
    }
    return winner;
}



void TheSnakeGame::moveNumberEater()
{
    numberEater.move();
}
void TheSnakeGame::searchShortestRoute()
{
    mission.checkSolutionForNumberEater(randNum, logSize, missionIndex, &numberEater);
}

void TheSnakeGame::moveLineFliers()
{
    for (int i = 0; i < arrSize; i++)
	creatures[i]->move();
}


void TheSnakeGame::moveColFliers()
{
    for (int i = 2; i < NUMBER_OF_CREATURES; i++)
	if ((static_cast<ColFlier*>(creatures[i])->isOnScreen()))
	    creatures[i]->move();
}

void TheSnakeGame::numIsEatenActions(bool &winner, int& numEaten, int snakeInd)
{
    logSize--;
    if (mission.handleCurrentMission(numEaten, missionIndex))		//checks if the num fitts the mission
    {
	winner = mission.handleCorrectAnswer(snakeInd, snakes);
	printScoreBoard();
    }
    else                      //the number eaten is wrong
    {
	winner = mission.handleWrongAnswer(snakeInd, snakes);
	printWrongAnswerCase();
    }
    endOfTurnActions();
}

void TheSnakeGame::creatureEatNumberActions()
{
    logSize--;
    sortArray(logSize);
    lastDeletedRandInd = NO_VALUE;
}

void TheSnakeGame::endOfTurnActions()			//Actions to do at after a number has been eaten
{

    deleteHalfNumbers();
    lastDeletedRandInd = NO_VALUE;
    mission.randomMission(missionIndex);
    missionIndex = mission.getMissionIndex();
    numberEater.resetNumberEater();
    lastTurnReplay.emptyScreens();
}


void TheSnakeGame::numberEaterHitPlayer(char playerChar)
{
    gotoxy(1, 2);
    cout << "                                    ";
    SetConsoleTextAttribute(hConsole, RED);
    if (playerChar == '@')
    {
	gotoxy(1, 2);
	cout << "NumberEater ate player @! Player # gains a point.";
	Sleep(1000);
	snakes[SECOND_SNAKE].increaseBody(snakes[SECOND_SNAKE].getSize());
	gotoxy(1, 2);
	cout << "                                                  ";
    }
    else
    {
	gotoxy(1, 2);
	cout << "Number Eater ate player #! Player @ gains a point.";
	Sleep(1000);
	snakes[FIRST_SNAKE].increaseBody(snakes[FIRST_SNAKE].getSize());
	gotoxy(1, 2);
	cout << "                                                  ";
    }
}

void TheSnakeGame::printWrongAnswerCase()
{
    printScoreBoard();
    gotoxy(1, 3);
    cout << "Checking for a correct solution on the board...";
    Sleep(1000);
    gotoxy(1, 3);
    cout << "                                                 ";
    mission.checksSolutions(randNum, logSize, missionIndex, false);
}



void TheSnakeGame::checkSnakesPunishmentStatus()		//Checks if any snake is punished-if so reduces his punishment time
{
    int i;
    for (i = 0; i < arrSize; i++)
	if (snakes[i].isPunished())
	    snakes[i].reducePunishment();
}

int TheSnakeGame::handleEscKey(int decision)
{
    gotoxy(0, 0);
    cout << "                                                                                ";
    gotoxy(0, 1);
    cout << " ";
    switch (decision)
    {
    case CONTINUE:
	SetConsoleTextAttribute(hConsole, YELLOW);
	printAllUsedNums();
	mission.randomMission(missionIndex, 0);
	break;
    case START_OVER://delete all numbers from screen and from array
	deleteAllNumbersFromArr();
	mission.randomMission(missionIndex, 0);
	logSize = 0;
	break;
    case NEW_MISSION://change mission, cut numbers in two
	deleteHalfNumbers();
	mission.randomMission(missionIndex);
	SetConsoleTextAttribute(hConsole, YELLOW);
	printAllUsedNums();
	break;
    case NEW_GAME://delete all points, all numbers, get a new mission
	resetGameParameters();
	mission.randomMission(missionIndex);
	break;
    case INSTRUCTIONS:
	handlePressedKey('1');
	return INSTRUCTIONS;
    }
    return 0;
}

void TheSnakeGame::deleteAllNumbersFromArr()	//Deletes all numbers from the random array
{
    Point tempPoint;
    int size;
    for (int i = 0; i < logSize; i++)
    {
	if (randNum[i].isRandOn())
	{
	    tempPoint = randNum[i].getNumCoords(0);
	    size = randNum[i].getNumDigits();
	    randNum[i].~Random();
	    for (int j = 0; j < size; j++)
		screen[tempPoint.getY()][tempPoint.getX() + j] = ' ';
	}
    }
}

void TheSnakeGame::deactivateCreature(int x, int y)	//Deactivate a col flier/number eater given the x,y
{
    int i;
    for (i = 0; i < arrSize; i++)
	if (colFliers[i].getBody().getY() == y && colFliers[i].getBody().getX())
	    colFliers[i].setOnOff(false);

    if (numberEater.getBody().getX() == x && numberEater.getBody().getY() == y)
	numberEater.setOnScreen(false);
}

void TheSnakeGame::printAllUsedNums()//for continue and new mission
{
    for (int i = 0; i < logSize; i++)
	if (randNum[i].isRandOn())
	{
	    gotoxy(randNum[i].getNumCoords(0).getX(), randNum[i].getNumCoords(0).getY());
	    cout << randNum[i].getNum();
	}
}

void TheSnakeGame::deleteHalfNumbers()				//Deletes half of the number on the board
{
    int counter = 0, indexToDelete;
    int halfOf = logSize / 2;
    if (logSize == MAX_RAND_SIZE)
	logSize--;
    while (counter < halfOf)
    {
	indexToDelete = rand() % logSize + 1;			//Randomize a random index of max value of size/2
	if (randNum[indexToDelete].isRandOn() && indexToDelete != lastDeletedRandInd)
	{
	    deletePrintedNum(&(randNum[indexToDelete]));
	    counter++;
	}
    }
    if (logSize % 2 == 0)
    {
	sortArray(halfOf);
	logSize = halfOf;
    }
    else
    {
	sortArray(halfOf + 1);
	logSize = halfOf + 1;
    }

}

void TheSnakeGame::sortArray(int size)		//Sorts the random array-so there will be no "holes"
{
    Random* tempArr = new Random[size];

    copyArray(tempArr, logSize);
    replaceArry(tempArr, size);
    gotoxy(1, 1);

}

void TheSnakeGame::replaceArry(Random* tempArr, int size)	//replace the old random array with sorted one
{
    int i;

    for (i = 0; i < size; i++)
    {
	randNum[i].copyRandItem(&tempArr[i]);
	tempArr[i].~Random();
    }

    delete[]tempArr;
    tempArr = nullptr;
}

void TheSnakeGame::copyArray(Random* tempArr, int physSize)
{
    int i;
    int ind = 0;
    for (i = 0; i < MAX_RAND_SIZE; i++)
    {
	if (randNum[i].isRandOn())
	{
	    tempArr[ind].copyRandItem(&randNum[i]);
	    randNum[i].~Random();
	    ind++;
	}
    }

}

void TheSnakeGame::hideSnake(int snakeInd)		//Hides a snake from the game
{
    snakes[snakeInd].getPunished();
}

void TheSnakeGame::checkOtherSnakeBullet(int snakeIndex, int x, int y)
{
    snakes[snakeIndex].checksCurrentSnakeBullet(x, y);
}


int TheSnakeGame::eatNumber(int next_x, int next_y)//Gets num that the snake ate
{
    Point tempPoint;
    int num = NO_VALUE;
    bool isDeleted = false;
    if ((screen[next_y][next_x] >= '0' && screen[next_y][next_x] <= '9'))
    {
	for (int i = 0; i < logSize; i++)									//the whole random array
	{
	    for (int j = 0; j < randNum[i].getNumDigits() && isDeleted == false; j++)						//the whole point array inside one random
	    {				//we wanna know if the next snake's point is a point in the array - that means it has a number there
		tempPoint = randNum[i].getNumCoords(j);
		if ((tempPoint.getX() == next_x) && (tempPoint.getY() == next_y)/*&& randNum[i].isRandOn()*/)
		{									//this is the number the snake ate
		    num = randNum[i].getNum();				//Gets the num which is eaten
		    deletePrintedNum(&(randNum[i]));		//Sends the adress of the number and the eaten digit index
		    lastDeletedRandInd = i;
		    isDeleted = true;
		}
	    }
	}
    }
    return num;
}


void TheSnakeGame::deletePrintedNum(Random *toDelete)	//Delets the eaten number from the screen and rand array, releasing the dynamic point 
{														//-array of that number
    toDelete->updateRandomOnScreen(toDelete->getNumCoords(0), toDelete->getNumDigits(), ' ');
    Point tempPoint = toDelete->getNumCoords(0);
    int size = toDelete->getNumDigits();
    for (int j = 0; j < size; j++)
	screen[tempPoint.getY()][tempPoint.getX() + j] = ' ';
    toDelete->~Random();
}

int TheSnakeGame::menu()
{
    char keyPressed;
    clrscr();
    gotoxy(1, 1);
    cout << "Hello there, and welcome to the snake game, please press 1 for instructions," << endl;
    gotoxy(1, 2);
    cout << "2 to start a new game, or 9 if you want to exit" << endl;
    keyPressed = _getch();
    return (handlePressedKey(keyPressed));
}

int TheSnakeGame::handlePressedKey(char keyPressed)
{
    char fileOrNot;
    clrscr();
    switch (keyPressed)
    {
    case '1'://INSTRUCTIONS
	gotoxy(1, 1);
	cout << "Collect a number that fits the mission. Be the first to get 12 points.";
	gotoxy(1, 2);
	cout << "Correct answer? get a point. wrong answer? the enemy gets a point. KEYS:";
	gotoxy(1, 3);
	cout << "P1:(a,w,d,x,z) P2:(j,i,l,m,n) z,n accordingly, to shoot. main menu - ESC";
	return INSTRUCTIONS;
    case '2'://NEW GAME
	gotoxy(1, 1);
	cout << "If you want to load missions from file press 1, for regular game press 2";
	fileOrNot = _getch();
	clrscr();
	if (fileOrNot == '1')
	{
	    missionsFromFile = true;
	    ifstream inFile("missions.txt");
	    if (inFile.is_open())
	    {
		gotoxy(1, 1);
		cout << "Good luck!" << endl;
		Sleep(500);
		gotoxy(1, 1);
		cout << "           ";
		mission.readMissionsFromFile(inFile);
	    }
	    else//file didnt open
	    {
		gotoxy(1, 1);
		cout << "Error opening file";
		Sleep(500);
	    }
	}
	resetGameParameters();
	mission.randomMission(missionIndex);
	return NEW_GAME;
    case '9'://EXIT
	return EXIT;
    }
    return 0;
}

int TheSnakeGame::submenu()
{
    clrscr();
    int decision;
    char screen[MENU_LINES][MAX_COLS] = { "Press 1 to exit, 2 for menu, 3 to continue, 4 to start this mission again\n"," 5 to start a new mission, or 6 to start a new game\n" };
    gotoxy(1, 1);
    for (int i = 0; i < MENU_LINES; i++)
	cout << screen[i];
    char keyPressed = _getch();
    clrscr();
    switch (keyPressed)
    {
    case '1':
	return EXIT;
    case '2':
	decision = menu();
	return decision;
    case CONTINUE:
	return CONTINUE;
    case START_OVER:
	return START_OVER;
    case NEW_MISSION:
	return NEW_MISSION;
    case NEW_GAME:
	return NEW_GAME;
    case REPLAY:
	initiateReplay();
	replay = true;
	break;
    }
    return 0;
}

void TheSnakeGame::printScoreBoard()
{

    SetConsoleTextAttribute(hConsole, MUSTARD);
    gotoxy(51, 1);
    cout << "Player @ score: " << snakes[FIRST_SNAKE].getSize() - INIT_SNAKE_SIZE;
    gotoxy(51, 2);
    cout << "Player # score: " << snakes[SECOND_SNAKE].getSize() - INIT_SNAKE_SIZE;
    gotoxy(70, 1);
    snakes[0].printBullets();
    gotoxy(70, 2);
    snakes[1].printBullets();
    SetConsoleTextAttribute(hConsole, GREY);
}
