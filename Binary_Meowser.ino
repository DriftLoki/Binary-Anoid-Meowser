// Modified for Binary Fire (Fire on pull, Fire on release)

#include <debounce.h>

#define trigger 10
#define mosfet 8 // Output pin for the Nexperia PSMN1R0-30YLC

//Keep these the same
int dartQueue = 0;
long timeInPushState = 0; //for tracking delay
int shotDelay; 
String pushState = "idle"; 
int firedDarts = 0;

//Adjustable variables
const int solenoidOn = 40; //time it takes solenoid to extend then retract
const int solenoidOff = 32;
const int solenoidCold = 15; //extra time for first shot
int dpsSetting = 9; //how many darts fire in a second

static void triggerHandler(uint8_t btnId, uint8_t btnState) { 
  if(millis() < 500){  //ensures that arduino has time to fully boot
    dartQueue = 0;
    return;
  }

  // --- BINARY MODIFICATION ---
  // The debounce library calls this function ONLY when the trigger changes state.
  // Instead of setting the queue to 40 or 0, we simply add 1 to the queue 
  // on BOTH the press and the release.
  
  if (dartQueue < 3) { // Caps the queue so spamming the trigger doesn't cause a runaway
    dartQueue++; 
  }
}

static Button triggerButton(0, triggerHandler); //declaring button for debouncing

int delayCalc(){
  int calc = (1000 / dpsSetting) - solenoidOn - solenoidOff; //what the delay should be per dart - time solenoid needs to actuate
  if(calc < 0){ //if we somehow get a math error, delay forces 0
    return 0;
  }
  return calc;
}

void pollButtons(){ //called to check state of trigger. 
  triggerButton.update(digitalRead(trigger)); 
}

int coldStart(){ //adjusts delay for first shot to account for a cold start
  if(firedDarts > 0){
    return 0;
  } else {
    return solenoidCold;
  }
}

void setup() {
  pinMode(trigger, INPUT_PULLUP);  //IO configuration
  pinMode(mosfet, OUTPUT);

  triggerButton.setPushDebounceInterval(20); //debounce timing intervals
  triggerButton.setReleaseDebounceInterval(20);
  shotDelay = delayCalc(); //Some math to figure out the shot delay
  delay(50); //give the outputs/inputs aquick 50ms to settle.
  dartQueue = 0; //ensure this doesnt change
}

void loop() {
  pollButtons(); //check what the trigger is doing
  
  if(dartQueue > 0){  //darts ready to go???
    if(pushState == "idle"){  //start here, system is ready to go
      digitalWrite(mosfet, HIGH); //solenoid is extending
      pushState = "thrusting"; //change state
      timeInPushState = millis(); //timestamp to track time
    }
    if(pushState == "thrusting"){ //solenoid is now moving foward
      if((timeInPushState + solenoidOn + (shotDelay/2)) + coldStart() < millis()){ //wait until the 1/2 of the delay + time to extend + timestamp + and coldstart has passed to move on
        digitalWrite(mosfet, LOW); //when 1/2 of total calculated delay has passed, solenoid should be fully extended so turn off mosfet to retract solenoid
        pushState = "retracting"; //solenoid is now retracting
        timeInPushState = millis(); //new timestamp for retraction time
      }
    }
    if(pushState == "retracting"){ //if the solenoid is retracting
      if((timeInPushState + solenoidOff + (shotDelay/2)) < millis()){ //wait until 1/2 of delay + time to retract + timestamp has passed to mvoe on
        dartQueue--; //1 cycle complete, subtract from queue
        firedDarts++; //add to fired count so coldStart works properly
        pushState = "idle"; //solenoid has fully retracted so put it in ready state
        timeInPushState = millis(); //extra timestamp to keep it relative incase another one misses
      }
    }
  } 
  else { //no darts in queue ?
    digitalWrite(mosfet, LOW); //turn off solenoid
    firedDarts = 0; //reset to allow coldStart to work
  }
}