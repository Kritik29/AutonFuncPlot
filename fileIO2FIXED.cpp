#include "PC_FileIO.c"
/*
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
*/
typedef struct{
//0-linear, 1-quadratic, 2-cubic
	int type;

// for quadratics and linear
	float x_3;
	float x_2;
	float x_1;
	float x_0;

	// for trig
	float a;	// a - vert stretch
	float b;	// b - hor stretch
	float c;	// c - hor shift
	float d;	// d - vert shift

	float yTheo;
	float angleTheo;
} FuncInfo;
/*
typedef struct{
tMotor port;
int power;
} MotorInfo;

typedef struct{
float x;
 	float y;
} Point;
*/

void fileIO(FuncInfo & funcPara, TFileHandle & fin)
{
	//assume file open is successful
	int num_func = 3;
	bool b1 = true;
	string funcTypeString = "";
	int lineNum = 0;

	for (int i=0; i<num_func; i++)
	{
		readTextPC(fin, funcTypeString);
		displayString(5, "aaaa %s ", funcTypeString);

		if(funcTypeString == "linear")
		{
			displayString(8, "Linear");
			funcPara.type = 0;
			b1 = readFloatPC (fin, funcPara.x_1);
			b1 = readFloatPC (fin, funcPara.x_0);
		}
		else if(funcTypeString == "quadratic")
		{
			displayString(7, "Quadratic");
			funcPara.type = 1;
			b1 = readFloatPC (fin, funcPara.x_2);
			b1 = readFloatPC (fin, funcPara.x_1);
			b1 = readFloatPC (fin, funcPara.x_0);
		}
		else if(funcTypeString == "cubic")
		{
			displayString(10, "Cubic");
			funcPara.type = 2;
			b1 = readFloatPC (fin, funcPara.x_3);
			b1 = readFloatPC (fin, funcPara.x_2);
			b1 = readFloatPC (fin, funcPara.x_1);
			b1 = readFloatPC (fin, funcPara.x_0);
		}
		else // sinusoidal
		{
			displayString(9, "Sine");
			funcPara.type = 3;
			b1 = readFloatPC (fin, funcPara.a);
			b1 = readFloatPC (fin, funcPara.b);
			b1 = readFloatPC (fin, funcPara.c);
			b1 = readFloatPC (fin, funcPara.d);
		}
		//displayString(lineNum, "%d, %f, %f, %f, %f, %f, %f, %f, %f", funcPara.type, funcPara.x_0, funcPara.x_1, funcPara.x_2, funcPara.x_3, funcPara.a, funcPara.b, funcPara.c, funcPara.d );
		displayString(lineNum, "-- %d", funcPara.type);
		lineNum ++;
	}
	closeFilePC(fin);
}


task main()
{
	TFileHandle fin; TFileHandle lol;
	bool fileOK2 = openReadPC(fin, "test1.txt");
	FuncInfo testing1;

	if (!fileOK2){
		displayString(1, "not found");
	}
	else{
		displayString(1, " found");
	}
fileIO(testing1, fin);

	/*bool fileOK = openReadPC(lol, "lolo.txt");

	string check = "";
	readTextPC(lol, check);
	bool k = displayTextLine(6, "%s", check);*/

	while (!getButtonPress(buttonAny)){}

	while (getButtonPress(buttonAny)){}

}
