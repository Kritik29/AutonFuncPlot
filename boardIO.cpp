#include "PC_FileIO.c"

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

/*typedef struct{
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
} FuncInfo; */


void boardIO(BoardInfo & board, TFileHandle fin1)
{
	//assume file opens successfully
	bool b2 = true;
	
	b2 = readFloatPC (fin1, board.low_x);
	b2 = readFloatPC (fin1, board.high_x);
	b2 = readFloatPC (fin1, board.low_y);
	b2 = readFloatPC (fin1, board.high_y);
	b2 = readFloatPC (fin1, board.width_CM);
	b2 = readFloatPC (fin1, board.length_CM);
	
	//closeFilePC(fin1);   close file in main?
}


int main ()
{
	TFileHandle fin1;
	BoardInfo b;
	boardIO(b, fin1);
	cout << b.low_x << endl << b.high_x << endl << b.low_x;
}


