/*
  WorkRave-uino
  Date:  11/2/15 20:21 EST
  Coder: Flame Soulis (Daniel Pearce)
*/

//LED Handlers
#define LED_HEARTBEAT 	0
#define LED_MICROPAUSE	3
#define LED_RESTBREAK   4

//Buzzer
#define BUZZER_PORT	3

//Time Handlers
#define TIME_TICK       100
#define TIME_MICROWARN	150
#define TIME_MICROPAUSE	3000
#define TIME_MICROEVENT	200
#define TIME_RESTWARN	  300
#define TIME_RESTBREAK	27000
#define TIME_RESTEVENT	3000

//Stages
#define STAGE_IDLE		    1
#define STAGE_MICROWARN		2
#define STAGE_MICROEVENT	3
#define STAGE_RESTWARN		4
#define STAGE_RESTEVENT		5

//Global
int iStageTimer;
int iRestBreakTimer;
unsigned long lOperationTime;
byte bStageMode;

void setup() {
	//Setup global variables
  	iStageTimer = (TIME_MICROPAUSE - TIME_MICROWARN);
	iRestBreakTimer = (TIME_RESTBREAK - TIME_RESTWARN);
	bStageMode = STAGE_IDLE;
	
	//Setup Pins
	pinMode(LED_HEARTBEAT, OUTPUT);
	pinMode(LED_MICROPAUSE, OUTPUT);
	pinMode(LED_RESTBREAK, OUTPUT);
	pinMode(BUZZER_PORT, OUTPUT);

  //Test Flash
  byte a; bool b=true;
  for(a=0;a<20;++a) {
    b=!b;
    digitalWrite(LED_MICROPAUSE,(b) ? HIGH : LOW);
    digitalWrite(LED_RESTBREAK,(!b) ? HIGH : LOW);
    delay(250);
  }
  //Reset to low and begin!
  digitalWrite(LED_RESTBREAK, LOW);
  digitalWrite(LED_MICROPAUSE, LOW);
}

void loop() {
	//Grab the current 'time'
	lOperationTime = millis();
  //Do actions based on the current stage
	switch(bStageMode) {
		//IDLE
		case STAGE_IDLE:
			//Handle Ticks
			--iStageTimer;
			--iRestBreakTimer;
			//Check handles for events
			if(!iRestBreakTimer) {
				//Change stage to Rest Warning
				bStageMode = STAGE_RESTWARN;
				//Set timer to rest break's warning timer
				iStageTimer = TIME_RESTWARN;
			} else if(!iStageTimer) {
				//Change stage to Micro Warning
				bStageMode = STAGE_MICROWARN;
				//Set timer to micro pause's warning timer
				iStageTimer = TIME_MICROWARN;
			}
			break;
		//MICRO PAUSE WARNING
		case STAGE_MICROWARN:
			//Handle only the stage's timer
			--iStageTimer;
			//Engage buzzer to grab user's attention
			//(Every 10 ticks, toggle the buzzer on and off)
			digitalWrite(BUZZER_PORT, (iStageTimer/10) % 2 != 0);
			//Blink the Micro Pause LED
			digitalWrite(LED_MICROPAUSE, (iStageTimer/5) % 3 != 0);
			//Check if we need to enter Micro Pause Event
			if(!iStageTimer) {
				//Change stage to Micro Event
				bStageMode = STAGE_MICROEVENT;
				//Set timer to micro puase's event timer
				iStageTimer = TIME_MICROEVENT;
				//Disable the buzzer to be safe
				digitalWrite(BUZZER_PORT, LOW);
				//Enable the light for next stage
				digitalWrite(LED_MICROPAUSE, HIGH);
			}
			break;
		//MICRO PAUSE EVENT
		case STAGE_MICROEVENT:
			//Handle only the stage's timer
			--iStageTimer;
			//Check if we need to enter Micro Pause Event
			if(!iStageTimer) {
				//Change stage to Micro Event
				bStageMode = STAGE_IDLE;
				//Set timer to micro puase's event timer
				iStageTimer = (TIME_MICROPAUSE - TIME_MICROWARN);
				//Turn the LED off
				digitalWrite(LED_MICROPAUSE, LOW);
			}
			break;
    //REST BREAK WARNING
    case STAGE_RESTWARN:
      //Handle only the stage's timer
      --iStageTimer;
      //Engage buzzer to grab user's attention
      //(Every 10 ticks, toggle the buzzer on and off)
      digitalWrite(BUZZER_PORT, (iStageTimer/10) % 2 != 0);
      //Blink the Micro Pause LED
      digitalWrite(LED_RESTBREAK, (iStageTimer/5) % 3 != 0);
      //Check if we need to enter Micro Pause Event
      if(!iStageTimer) {
        //Change stage to Micro Event
        bStageMode = STAGE_RESTEVENT;
        //Set timer to micro puase's event timer
        iStageTimer = TIME_RESTEVENT;
        //Disable the buzzer to be safe
        digitalWrite(BUZZER_PORT, LOW);
        //Enable the light for next stage
        digitalWrite(LED_RESTBREAK, HIGH);
      }
      break;
    //REST BREAK EVENT
    case STAGE_RESTEVENT:
      //Handle only the stage's timer
      --iStageTimer;
      //Check if we need to enter Micro Pause Event
      if(!iStageTimer) {
        //Change stage to Micro Event
        bStageMode = STAGE_IDLE;
        //Set timer to rest break's event timer and micro pause
        iStageTimer = (TIME_MICROPAUSE - TIME_MICROWARN);
        iRestBreakTimer = (TIME_RESTBREAK - TIME_RESTWARN);
        //Turn the LED off
        digitalWrite(LED_RESTBREAK, LOW);
      }
      break;
	}
	//Sleep for the tick
	//(smartly by checking how long it took to do the loop)
	delay(TIME_TICK - (millis()-lOperationTime));
}
