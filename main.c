#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include <stdbool.h>


#define IN1_PIN 1
#define IN2_PIN 4
#define IN3_PIN 5
#define IN4_PIN 6
#define MAX_SPEED 40
#define MIN_SPEED 0
#define LEFT_TRACER_PIN 10
#define RIGHT_TRACER_PIN 11

#define IR_LEFT 27
#define IR_RIGHt 26

#define TRIG_PIN	28
#define ECHO_PIN	29

int ultraSonicRunningFlag = 0;
int leftTracer, rightTracer;
int leftIR, rightIR;
int counter = 0;
int turning = 0;
int pedoFlag = 0;

void initDCMotorSoft() {
	pinMode(IN1_PIN, SOFT_PWM_OUTPUT);
	pinMode(IN2_PIN, SOFT_PWM_OUTPUT);
	pinMode(IN3_PIN, SOFT_PWM_OUTPUT);
	pinMode(IN4_PIN, SOFT_PWM_OUTPUT);
	softPwmCreate(IN1_PIN, MIN_SPEED, MAX_SPEED);
	softPwmCreate(IN2_PIN, MIN_SPEED, MAX_SPEED);
	softPwmCreate(IN3_PIN, MIN_SPEED, MAX_SPEED);
	softPwmCreate(IN4_PIN, MIN_SPEED, MAX_SPEED);
	// printf("Init dc motor soft\n");
}

void goForwardSoft(int speed) {
	softPwmWrite(IN1_PIN, speed);
	softPwmWrite(IN2_PIN, MIN_SPEED);
	softPwmWrite(IN3_PIN, speed);
	softPwmWrite(IN4_PIN, MIN_SPEED);
	// printf("Forward soft\n");
}

void goBackwardSoft() {
	softPwmWrite(IN1_PIN, MIN_SPEED);
	softPwmWrite(IN2_PIN, MAX_SPEED);
	softPwmWrite(IN3_PIN, MIN_SPEED);
	softPwmWrite(IN4_PIN, MAX_SPEED);
	// printf("Backward soft\n");
}

void stopDCMotorSoft() {
	softPwmWrite(IN1_PIN, MIN_SPEED);
	softPwmWrite(IN2_PIN, MIN_SPEED);
	softPwmWrite(IN3_PIN, MIN_SPEED);
	softPwmWrite(IN4_PIN, MIN_SPEED);
	// printf("Stop dc motor soft\n");
}


void turnLeftSoft() {
	softPwmWrite(IN1_PIN, MAX_SPEED);
	softPwmWrite(IN2_PIN, MIN_SPEED);
	softPwmWrite(IN3_PIN, MIN_SPEED);
	softPwmWrite(IN4_PIN, MAX_SPEED);
	printf("Right soft\n");
}

void turnRightSoft() {
	softPwmWrite(IN1_PIN, MIN_SPEED);
	softPwmWrite(IN2_PIN, MAX_SPEED);
	softPwmWrite(IN3_PIN, MAX_SPEED);
	softPwmWrite(IN4_PIN, MIN_SPEED);
	printf("Left soft\n");
}

void initDCMotorHard() {
	pinMode(IN1_PIN, OUTPUT);
  	pinMode(IN2_PIN, OUTPUT);
  	pinMode(IN3_PIN, OUTPUT);
  	pinMode(IN4_PIN, OUTPUT);
	digitalWrite(IN1_PIN, HIGH);
	digitalWrite(IN2_PIN, HIGH);
	digitalWrite(IN3_PIN, HIGH);
	digitalWrite(IN4_PIN, HIGH);
	// printf("Init dc motor\n");
}

void stopDCMotorHard() {
	digitalWrite(IN1_PIN, LOW);
	digitalWrite(IN2_PIN, LOW);
	digitalWrite(IN3_PIN, LOW);
	digitalWrite(IN4_PIN, LOW);
	// printf("Stop dc motor\n");
}

void turnLeftHard() {
	digitalWrite(IN1_PIN, 0);
	digitalWrite(IN2_PIN, 1);
	digitalWrite(IN3_PIN, 1);
	digitalWrite(IN4_PIN, 0);
	delay(370);
	// printf("Left hard\n");
}

void turnRightHard() {
	digitalWrite(IN1_PIN, 1);
	digitalWrite(IN2_PIN, 0);
	digitalWrite(IN3_PIN, 0);
	digitalWrite(IN4_PIN, 1);
	// printf("Right hard\n");
}

void goForwardHard() {
	digitalWrite(IN1_PIN, 1);
	digitalWrite(IN2_PIN, 0);
	digitalWrite(IN3_PIN, 1);
	digitalWrite(IN4_PIN, 0);
	printf("Forward soft\n");
}

void goBackwardHard() {
	digitalWrite(IN1_PIN, 0);
	digitalWrite(IN2_PIN, 1);
	digitalWrite(IN3_PIN, 0);
	digitalWrite(IN4_PIN, 1);
	printf("Backward soft\n");
}

void initIR() {
	pinMode(LEFT_TRACER_PIN, INPUT);
	pinMode(RIGHT_TRACER_PIN, INPUT);
}


int getDistance() {
    int start_time=0, end_time=0;
    float distance=0;
    digitalWrite(TRIG_PIN, LOW) ;
    delay(250) ;
    digitalWrite(TRIG_PIN, HIGH) ;
    delayMicroseconds(10) ;
    digitalWrite(TRIG_PIN, LOW) ;
    
while (digitalRead(ECHO_PIN) == 0) ;
    start_time = micros() ;
    
while (digitalRead(ECHO_PIN) == 1) ;
    end_time = micros() ;
    
distance = (end_time - start_time) / 29. / 2. ;
    return (int)distance;
}


void readTracers() {
	leftTracer = digitalRead(LEFT_TRACER_PIN);
	rightTracer = digitalRead(RIGHT_TRACER_PIN);
}

void readIRs () {
	leftIR = digitalRead(IR_LEFT);
	rightIR = digitalRead(IR_RIGHt);
}

void fullStop() {
	stopDCMotorHard();
	delay(10);
	stopDCMotorSoft();
	delay(10);
}


int obs = 2;
bool isObs = FALSE;
bool leaveLastObj = TRUE;

void lastBlock () {
	while (1)
	{
		goBackwardSoft();
		delay(600);
		break;
	}
	
	while (leaveLastObj)
	{
		readTracers();
		readIRs();

		turnLeftSoft();
		delay(600);
		leaveLastObj = FALSE;
		break;
		if (!leftTracer && !rightTracer) {
			break;
		}
	}
	while (!leaveLastObj)
	{
		readTracers();
		readIRs();
		goForwardSoft(30);

		if (!leftTracer && !rightTracer) {
			break;
		}

		if (!leftTracer) {
			turnLeftSoft();
			delay(0.2);
		}

		if (!rightTracer) {
			turnRightSoft();
			delay(0.2);
		}
	}

	while (1)
	{
		readTracers();
		readIRs();
		goBackwardSoft();
		delay(700);
		break;
	}
	
	
}

bool lastLine = FALSE;

bool last = false;

bool last1 = false;
void finish() {
	while (!last)
	{
		readTracers();
		readIRs();

		turnRightSoft();
		delay(500);
		goForwardSoft(30);
		delay(1500);
		turnLeftSoft();
		delay(150);
		last = true;
		break;
		if (!leftTracer && !rightTracer) {
			break;
		}
	}
	while (last)
	{
		readTracers();
		readIRs();

		goForwardSoft(30);

		if (!leftTracer && !rightTracer) {
			if (lastLine) {
				last1 = true;
				break;
			}
		}

		if (leftTracer && rightTracer) {
			lastLine = true;
		}

		if (!leftTracer && rightTracer) {
			turnLeftSoft();
			delay(0.2);
		}

		if (!rightTracer) {
			turnRightSoft();
			delay(0.2);
		}

		if (!leftIR && !rightIR) {
			turnRightSoft();
			delay(500);
		}
	}
}

void goFromStart() {
	printf("Before Thread\n");
	initDCMotorSoft();

	readTracers();

	int dist = 1;


	while (1)
	{
		goForwardSoft();
		delay(2000);
		break;
	}

	while (1) {
		readTracers();
		readIRs();

		goForwardSoft(40);

		if (!leftTracer && !rightTracer) {
			break;
		}

		if (!leftTracer) {
			turnLeftSoft();
			delay(0.15);
		}

		if (!rightTracer) {
			turnRightSoft();
			delay(0.2);
		}

		if (obs == 3) {
			break;
		}

		if (!leftIR || !rightIR) {
			stopDCMotorHard();
			isObs = true;
			if (obs == 2) {
				++obs;
			}
		} else {
			if (isObs) {
				isObs = false;
				if (getDistance() >= 20) {
					printf("You entered: %d/n", obs);
					++obs;
				}
			}
		}
	}
	
	stopDCMotorHard();
	fullStop();
}

int main(void) {
	if(wiringPiSetup() == -1)
		return 0;

	pinMode(TRIG_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);

	initIR();

	goFromStart();

	if (obs == 3) {
		lastBlock();
		finish();
	}
}



