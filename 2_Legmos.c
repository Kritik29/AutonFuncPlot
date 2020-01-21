/*

LEGMOS


Student Names:
Mansheel Chahal
Colin Chatfield
Kritik Kaushal
Amjad Halis

*/

// include libraries
#include "PC_FileIO.c"

// constants
const int MARKER_ENCODER = 135;
const int TURN_SPD = 10;
const int DRIVE_SPD = 25;
const float RAD_TO_DEG = 180 / PI;
const float DEG_TO_CM = PI * 56 / 10 / 2 / 180;
const int RADIUS = 2.75;

// structs
typedef struct{
	float low_x;
	float high_x;
	float low_y;
	float high_y;
	float widthCM;
	float lengthCM;
	float xUnitDist;
	float yUnitDist;
} BoardInfo;

typedef struct{
	//0-linear, 1-quadratic, 2-cubic
	int type;

	// for quadratics and linear
	float x_3;
	float x_2;
	float x_1;
	float x_0;

	// for trig
	float a;    // a - vert stretch
	float b;    // b - hor stretch
	float c;    // c - hor shift
	float d;    // d - vert shift

	float yTheo;
	float angleTheo;
} FuncInfo;

typedef struct{
	float x;
	float y;
} Point;

/* function prototypes
void setMotorPower(MotorInfo, int);
void markerDown(bool); added

void rotateRobot(bool, int, int);
void drawAxis(BoardInfo); // *

Point returnStartPos(BoardInfo, FuncInfo); //returns start position
void moveToStart(Point startPos, MotorInfo left, MotorInfo right); //(Point startPoint, Point & pos)
//updtate rotate, robot position. Point cuurent

void drawFunction(BoardInfo, FuncInfo, Point & pos); //implement boudries checking here
FuncValue function(float, FuncInfo); //(float xPos, FuncInfo func) - returns yTheo and angleTheo

void InitializeBoard(ifstream, BoardInfo); added
void fileIO(FuncInfo & funcPara, TFileHandle & fin);
*/

// functions ************************************************************

void boardIO(BoardInfo & board, TFileHandle fin1)
{
	//assume file opens successfully
	bool b2 = true;

	b2 = readFloatPC (fin1, board.low_x);
	b2 = readFloatPC (fin1, board.high_x);
	b2 = readFloatPC (fin1, board.low_y);
	b2 = readFloatPC (fin1, board.high_y);
	b2 = readFloatPC (fin1, board.widthCM);
	b2 = readFloatPC (fin1, board.lengthCM);

	board.xUnitDist = board.lengthCM / (fabs(board.low_x) + fabs(board.high_x)) ;

	board.yUnitDist = board.widthCM / (fabs(board.low_y) + fabs(board.high_y)) ;
}

void fileIO(FuncInfo & funcPara, TFileHandle & fin)
{
	//assume file open is successful

	bool b1 = true;
	int fType = 0;
	int lineNum = 0;


	readIntPC(fin, fType);

	if(fType == 1)
	{
		funcPara.type = 0;
		b1 = readFloatPC (fin, funcPara.x_1);
		b1 = readFloatPC (fin, funcPara.x_0);
	}
	else if(fType == 2)
	{
		funcPara.type = 1;
		b1 = readFloatPC (fin, funcPara.x_2);
		b1 = readFloatPC (fin, funcPara.x_1);
		b1 = readFloatPC (fin, funcPara.x_0);
	}
	else if(fType == 3)
	{
		funcPara.type = 2;
		b1 = readFloatPC (fin, funcPara.x_3);
		b1 = readFloatPC (fin, funcPara.x_2);
		b1 = readFloatPC (fin, funcPara.x_1);
		b1 = readFloatPC (fin, funcPara.x_0);
	}
	else if(fType == 4)
	{
		funcPara.type = 3;
		b1 = readFloatPC (fin, funcPara.a);
		b1 = readFloatPC (fin, funcPara.b);
		b1 = readFloatPC (fin, funcPara.c);
		b1 = readFloatPC (fin, funcPara.d);
	}
	//displayString(lineNum, "%d, %f, %f, %f, %f, %f, %f, %f, %f", funcPara.type, funcPara.x_0, funcPara.x_1, funcPara.x_2, funcPara.x_3, funcPara.a, funcPara.b, funcPara.c, funcPara.d );
	displayString(lineNum, "-- %d", funcPara.type);
	lineNum ++;

	//closeFilePC(fin);
}


void markerDown(bool condition) //either down = true, up = false
{
	//value to put marker down onto white board, possibly (maybe not with the new magnet location needs to change it pushed too much on the board
/*
	nMotorEncoder[motorC] = 0;

	wait1Msec(500);

	if(condition)
	{
	motor[motorC] = -10;
	while(nMotorEncoder[motorC] > -MARKER_ENCODER)
	{}
	}
	else
	{
	motor[motorC] = 10;
	while(nMotorEncoder[motorC] < MARKER_ENCODER)
	{}
	}
	motor[motorC] = 0;
	wait1Msec(500);
	*/
}

void rotateRobotTo(int degree) //rotates robot to degree (from horizontal)
{
	wait1Msec(500);

	motor[motorA] = motor[motorD] = 0;
	int testGyro = getGyroDegrees(S1);
	if (testGyro > degree)
	{
		motor[motorA] = TURN_SPD;
		motor[motorD] = -TURN_SPD;
		while (getGyroDegrees(S1) > degree)
		{}
	}
	else
	{
		motor[motorA] = -TURN_SPD;
		motor[motorD] = TURN_SPD;
		while (getGyroDegrees(S1) < degree)
		{}
	}
	motor[motorA] = motor[motorD] = 0;

	wait1Msec(500);
}

void moveDist(float dist, int motorPow, int theoAngle)
{
	// x_units * board.xUnitDist
	// y_units * board.yUnitDist
	const float CONV = PI*2.75/180;
	float testAngle = 0, angleDiff = 0;
	nMotorEncoder[motorA] = 0;
	motor[motorA] = motor[motorD] = motorPow;
	while(fabs(nMotorEncoder[motorA]*CONV) < dist) //everything inside this loop accounts for drift
	{
		testAngle = getGyroDegrees(S1);
		angleDiff = testAngle - theoAngle;
		if(fabs(motorPow) + fabs(angleDiff) > 75)
		{
			angleDiff = 75 - motorPow;
			if(motorPow < 0) //this is to account for when it moves backwards
				angleDiff = -75 + motorPow;
		}
		motor[motorA] = motorPow + angleDiff;
		motor[motorD] = motorPow - angleDiff;
	}
	motor[motorA] = motor[motorD] = 0;
}

void drawAxis(BoardInfo board, Point & current)
{
	const int SPEED = 20;

	//starts in the top left corner, moves to top of y-axis
	moveDist(fabs(board.low_x*board.xUnitDist), SPEED, 0);

	//turn robot down
	rotateRobotTo(-90);

	//draw y-axis
	markerDown(true);
	moveDist(board.widthCM, SPEED, -90);
	markerDown(false);

	//turn horizontally
	rotateRobotTo(0);

	//drive upperDomain*unitsDomain
	moveDist(board.high_x*board.xUnitDist, SPEED, 0);

	//turn up
	rotateRobotTo(90);

	//move up to x-axis
	moveDist(fabs(board.low_y*board.yUnitDist), SPEED, 90);

	//turn horizontally
	rotateRobotTo(0);

	//draw cmLength, aka the x-axis
	markerDown(true);
	//moves backwards to account for the string getting tangled
	moveDist(board.lengthCM, -SPEED, 0);
	markerDown(false);

	current.x = board.low_x;
	current.y = 0;
}


void linear(float xPos, FuncInfo & linFunc)
{

	linFunc.yTheo = linFunc.x_1 * xPos + linFunc.x_0;
	linFunc.angleTheo = atan(linFunc.x_1);
	linFunc.angleTheo = linFunc.angleTheo * RAD_TO_DEG;
}

void quadratic(float xPos, FuncInfo & quadFunc)
{
	quadFunc.yTheo = quadFunc.x_2 * pow(xPos, 2) + quadFunc.x_1 * xPos + quadFunc.x_0;
	quadFunc.angleTheo = atan(2 * quadFunc.x_2 * xPos + quadFunc.x_1);
	quadFunc.angleTheo = quadFunc.angleTheo * RAD_TO_DEG;
}

void cubic(float xPos, FuncInfo & cubFunc)
{
	cubFunc.yTheo = cubFunc.x_3 * pow(xPos, 3) + cubFunc.x_2 * pow (xPos, 2) + cubFunc.x_1 * xPos + cubFunc.x_0;
	cubFunc.angleTheo = atan(3 * cubFunc.x_3 * pow(xPos, 2) + 2 * cubFunc.x_2 * xPos + cubFunc.x_1);
	cubFunc.angleTheo = cubFunc.angleTheo * RAD_TO_DEG;
}

void sinusoidal(float xPos, FuncInfo & sinFunc)
{
	sinFunc.yTheo = (sinFunc.a)*sin(sinFunc.b * (xPos - sinFunc.c)) + sinFunc.d;

	sinFunc.angleTheo = atan(sinFunc.a*sinFunc.b*cos(sinFunc.b * (xPos - sinFunc.c)));

	sinFunc.angleTheo = sinFunc.angleTheo * RAD_TO_DEG;
}

void funcReturn(float xPos, FuncInfo & func)
{
	if(func.type == 0)
		linear(xPos, func);

	else if(func.type == 1)
		quadratic(xPos, func);

	else if(func.type == 2)
		cubic(xPos, func);
	else
		sinusoidal(xPos, func);
}

void draw(BoardInfo board, FuncInfo func, Point & pos)
{
	markerDown(true);
	wait1Msec(100);
	motor[motorA] = motor[motorD] = DRIVE_SPD;
	funcReturn(pos.x, func);

	while (pos.x < board.high_x)
	{
		displayBigTextLine(8, "%d, %d", pos.x, pos.y);
		displayBigTextLine(10, "%d", func.yTheo);
		//pos.y < board.high_y + 2 && pos.y > board.low_y - 2 &&
		if (func.yTheo > board.low_y  && func.yTheo < board.high_y )
		{
			nMotorEncoder[motorA] = 0;
			nMotorEncoder[motorD] = 0;

			int testAngle = getGyroDegrees(S1);
			int theoAngle = (int)func.angleTheo;
			int pAngleDiff = (testAngle - theoAngle) * 2; //proportional angle difference

			if(DRIVE_SPD + fabs(pAngleDiff) > 100)
				pAngleDiff = 100 - DRIVE_SPD * pAngleDiff / fabs(pAngleDiff); //maxing out drive speed, multiplying by sign of pAngleDiff

			motor[motorA] = DRIVE_SPD + pAngleDiff;
			motor[motorD] = DRIVE_SPD - pAngleDiff;

			wait1Msec(50);

			pos.x += (nMotorEncoder[motorA] + nMotorEncoder[motorD]) / 2.0 * cosDegrees((getGyroDegrees(S1) + testAngle) / 2.0) * DEG_TO_CM / board.xUnitDist;
			pos.y += (nMotorEncoder[motorA] + nMotorEncoder[motorD]) / 2.0 * sinDegrees((getGyroDegrees(S1) + testAngle) / 2.0) * DEG_TO_CM / board.yUnitDist;
			funcReturn(pos.x, func);
		}
		else
		{
			rotateRobotTo(0);
			motor[motorA] = motor[motorD] = DRIVE_SPD;

			displayBigTextLine(8, "%d, %d", pos.x, pos.y);
			displayBigTextLine(10, "%d", func.yTheo);

			while((func.yTheo > board.high_y || func.yTheo < board.low_y) && pos.x < board.high_x)
			{
				displayBigTextLine(8, "%d, %d", pos.x, pos.y);
				displayBigTextLine(10, "%d", func.yTheo);

				nMotorEncoder[motorA] = 0;
				nMotorEncoder[motorD] = 0;

				int testAngle = getGyroDegrees(S1);
				int pAngleDiff = testAngle * 2;

				if(DRIVE_SPD + pAngleDiff > 100)
					pAngleDiff = 100 - DRIVE_SPD;
				motor[motorA] = DRIVE_SPD + pAngleDiff;
				motor[motorD] = DRIVE_SPD - pAngleDiff;

				wait1Msec(50);

				pos.x += (nMotorEncoder[motorA] + nMotorEncoder[motorD]) / 2.0 * cosDegrees((getGyroDegrees(S1) + testAngle) / 2.0) * DEG_TO_CM / board.xUnitDist;
				pos.y += (nMotorEncoder[motorA] + nMotorEncoder[motorD]) / 2.0 * sinDegrees((getGyroDegrees(S1) + testAngle) / 2.0) * DEG_TO_CM / board.yUnitDist;
				funcReturn(pos.x, func);
			}

			motor[motorA] = motor[motorD] = 0;

			if(pos.x < board.high_x) // if not at end, continue driving (tolerance, to make sure robot doesnt start driving upward at the end)
			{
				rotateRobotTo(func.angleTheo);
				motor[motorA] = motor[motorD] = DRIVE_SPD;
			}
		}
	}
	motor[motorA] = motor[motorD] = 0;
	markerDown(false);
}

void moveToLowX(Point & current, BoardInfo board){
	int moveBackX = 0;
	int moveBackY = 0;

	// move to left side of board after func ends
	moveBackX = current.x - board.low_x;
	moveBackY = current.y;

	moveDist(moveBackX * board.xUnitDist, -20, 0);
	if (moveBackY < 0){ // if robot is below x axis, rotate ccw: +90
		rotateRobotTo(90);
		moveDist(moveBackY * board.yUnitDist, 20, 90);
	}
	else if (moveBackY > 0){ // cw: -90
		rotateRobotTo(-90);
		moveDist(moveBackY * board.yUnitDist, 20, -90);
	}

	current.x = board.low_x;
	current.y = 0;
}


void moveToStartPosition(BoardInfo board, FuncInfo func, Point & current){

	funcReturn(board.low_x, func);
	float startY = func.yTheo;

	if (startY > board.high_y)
	{
		rotateRobotTo(90);
		moveDist(board.high_y * board.yUnitDist, DRIVE_SPD, 90);
		rotateRobotTo(0);
		current.y = board.high_y;
	}
	else if (startY < board.high_y)
	{
		rotateRobotTo(-90);
		moveDist(fabs(board.low_y * board.yUnitDist), DRIVE_SPD, -90);
		rotateRobotTo(0);
		current.y = board.low_y;
	}
	else
	{
		if(startY > 0)
		{
			rotateRobotTo(90);
			moveDist(startY * board.yUnitDist, DRIVE_SPD, 90);
		}
		else
		{
			rotateRobotTo(-90);
			moveDist(fabs(startY * board.yUnitDist), DRIVE_SPD, -90);
		}
		rotateRobotTo(func.angleTheo);
		current.y = startY;
	}
	current.x = board.low_x;
}

task main()
{
	// setting up sensors
	SensorType[S1] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
	while (getGyroRate(S1) != 0)
	{}
	resetGyro(S1);

	//Point defaultStart;
	Point startPos;
	startPos.x = 0;
	startPos.y = 0;

	Point current; //currPos
	current.x = 0;
	current.y = 0;

	FuncInfo funcToDraw;
	funcToDraw.type = 0;
	funcToDraw.x_3 = 0;
	funcToDraw.x_2 = 0;
	funcToDraw.x_1 = 0;
	funcToDraw.x_0 = 0;
	funcToDraw.a = 0;
	funcToDraw.b = 0;
	funcToDraw.c = 0;
	funcToDraw.d = 0;
	funcToDraw.yTheo = 0;
	funcToDraw.angleTheo = 0;

	// open files
	TFileHandle finFunc; TFileHandle finBoard;
	bool fileOK1 = openReadPC(finFunc, "demo.txt");
	bool fileOK2 = openReadPC(finBoard, "board.txt");

	if (!fileOK1 || !fileOK2){
		displayString(1, "File not found.");
	}
	else{
		displayString(1, "File found.");
	}


	// board info
	// make function to get boardInfo from a file
	BoardInfo board;
	board.low_x = 0;
	board.high_x = 0;
	board.low_y = 0;
	board.high_y = 0;
	board.widthCM = 0;
	board.lengthCM = 0;
	board.xUnitDist = 0;
	board.yUnitDist = 0;

	boardIO(board, finBoard);
	closeFilePC(finBoard);


	//draw Axes
	//drawAxis(board, current);

	int numFunctions = 0;
	readIntPC (finFunc, numFunctions);
	displayString(4, "%d", numFunctions);

	// drawing
	for (int count = 1; count <= numFunctions; count ++){
		startPos. x = 0;
		startPos. y = 0;

		fileIO(funcToDraw, finFunc);

		if (count != 1){
			moveToLowX(current, board);
		}

		moveToStartPosition(board, funcToDraw, current,);
		markerDown(true);

		displayBigTextLine(10, "enter");
		while(!getButtonPress(buttonEnter)){}
		while(getButtonPress(buttonAny)){}
		eraseDisplay();

		draw(board, funcToDraw, current);
		markerDown(false);

	}

	displayBigTextLine(10, "down");
	while(!getButtonPress(buttonDown)){}
	while(getButtonPress(buttonAny)){}

}
