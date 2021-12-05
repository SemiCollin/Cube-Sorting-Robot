#define leftWheel motorA
#define rightWheel motorD
#define Sorter motorB
#define Touch S1
#define Gyro S2
#define Color S3
const int RED_POSITION = -220;
const int YELLOW_POSITION = 0;
const int GREEN_POSITION = 220;
const int SPEED = 10;
const int DRIVE_SPEED = 10;
const int YELLOW = 6;
const int GREEN = 3;
const int RED = 5;
const int DRIVE_BACKWARDS = -10;
const float CORRECTION_FACTOR = 1.2;
const float CORRECTION_FACTOR_GYRO = 1.6;

int promptForCubes()
{

//	while(!getButtonPress(buttonAny))
//	{}
	int numCubes = 0;
	displayString(3, "Please indicate how many cubes");
	displayString(4, "you want to sort. Up to");
	displayString(5, "increase, down to decrease.");
	displayString(6, "Press Enter to start sorting");

	while(!getButtonPress(buttonEnter))
	{
		if(getButtonPress(buttonUp))
		{
			while(getButtonPress(buttonUp))
			{}
			numCubes++;
		}
		else if(getButtonPress(buttonDown))
		{
			while(getButtonPress(buttonDown))
			{}
			numCubes--;
		}
		displayString(7, "Cube Count : %i", numCubes);
	}
	return numCubes;
}

void driveUntilCube(int speed)
{
	float error = 0;
	const float CORRECTION_FACTOR = 1.8;
	const float CORRECTION_FACTOR_GYRO = 3;
	while ((SensorValue[Color] != RED) && (SensorValue[Color] != YELLOW) && (SensorValue[Color] != GREEN)) // works if no numbers pressed
	{
		int gyroValue = getGyroDegrees(S2);
		motor[leftWheel] = speed-error;
		motor[rightWheel] = speed+error;
		error = (nMotorEncoder(leftWheel) - nMotorEncoder(rightWheel))*CORRECTION_FACTOR;
		if (fabs(gyroValue)>5)
		{
			error += gyroValue*CORRECTION_FACTOR_GYRO;
		}

	}
		motor[leftWheel] = 0;
		motor[rightWheel] = 0;
}

void sort(int &numRed, int &numYellow, int &numGreen) // ex. S3 as colour sensor and MotorC
{
	int colour = SensorValue[S3];

	if (colour == RED)
	{
		numRed++;
		motor[Sorter] = -SPEED;
		while (nMotorEncoder[Sorter] > RED_POSITION) // global constant for tray position
		{}
		motor[Sorter] = 0;
	}

	else if (colour == YELLOW)
	{
		numYellow++;
		if (nMotorEncoder[Sorter] > 0)
		{
			motor[Sorter] = -SPEED;
		}
		else
		{
			motor[Sorter] = SPEED;
		}
		while (nMotorEncoder[Sorter] != YELLOW_POSITION) // global constant for tray position
		{}
		motor[Sorter] = 0;
	}

	else if (colour == GREEN)
	{
		numGreen++;
		motor[Sorter] = SPEED;
		while (nMotorEncoder[Sorter] < GREEN_POSITION) // global constant for tray position
		{}
		motor[Sorter] = 0;
	}
}

void driveDistance(float dist,int speed)
{
	nMotorEncoder(leftWheel) = 0;
	nMotorEncoder(rightWheel) = 0;
	const int ENC_LIMIT = dist*115/(PI*4);
	int error = 0;

	while (fabs((nMotorEncoder(leftWheel)+nMotorEncoder(rightWheel)))/2 < ENC_LIMIT)
	{
		int gyroValue = getGyroDegrees(S2);
		motor[leftWheel] = speed-error;
		motor[rightWheel] = speed+error;
		error = (nMotorEncoder(leftWheel) - nMotorEncoder(rightWheel))*CORRECTION_FACTOR;
		if (fabs(gyroValue)>5)
		{
			error += gyroValue*CORRECTION_FACTOR_GYRO;
		}
	}

	motor[leftWheel] = motor[rightWheel] = 0;
}

void leaveCubes ()
{
	const int NUM_COLOURS = 3;

	for (int colours_Sorted = 0; colours_Sorted < NUM_COLOURS; colours_Sorted++)
	{
		motor[Sorter] = -SPEED;
		while (nMotorEncoder[Sorter] > RED_POSITION) {}
		motor[Sorter] = 0;

		driveDistance(6, DRIVE_BACKWARDS);

		motor[Sorter] = SPEED;
		while (nMotorEncoder[Sorter] < GREEN_POSITION) {}
		motor[Sorter] = 0;

		driveDistance(12,DRIVE_SPEED);

	}
		driveDistance(10,DRIVE_SPEED);

		motor[Sorter] = -SPEED;
		while (nMotorEncoder[Sorter] > YELLOW_POSITION) {}
		motor[Sorter] = 0;
}


void cleanUp(int numCubes)
{
	int count = 0;
	int numRed = 0;
	int numYellow = 0;
	int numGreen = 0;

//	while(count < numCubes)
	for (int count = 0; count < numCubes ; count++)
	{
		displayString(8, "Count : %i", count);
		driveUntilCube(DRIVE_SPEED);
		sort(numRed, numYellow, numGreen);
		driveDistance(5, DRIVE_SPEED);
	}
	  driveDistance(20, DRIVE_SPEED);
}



task main()
{
	int count = 0;
	int numRed = 0;
	int numYellow = 0;
	int numGreen = 0;
	int numCubes = 0;

	numCubes = promptForCubes();

	SensorType[Touch] = sensorEV3_Touch; // CHANGE SENSOR VALUE
	wait1Msec(50);
	SensorType[Color] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[Color] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[Gyro] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[Gyro] = modeEV3Gyro_Calibration;
	wait1Msec(50);
	SensorMode[Gyro] = modeEV3Gyro_RateAndAngle;


	resetGyro(Gyro);
	nMotorEncoder[Sorter] = 0;
	clearTimer(T1);

	cleanUp(numCubes);

	leaveCubes();

	string program_length = getTimerValue(T1);

	displayBigTextLine(2, "The robot took %s seconds to sort the cubes", program_length);
	displayBigTextLine(5, "Number of green cubes sorted : %i", numGreen);
	displayBigTextLine(8, "Number of yellow cubes sorted : %i", numYellow);
	displayBigTextLine(11, "Number of red cubes sorted : %i", numRed);

	while(!getButtonPress(buttonAny))
	{}

}
