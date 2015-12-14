//Pins 30-37: Digital buttons for the octave notes
//Pin 50: Good motor
//Pin 51: Bad motor
//40: good fan
//41: bad fan

// Declare pins
int  buttonPin[8];    // the pin that the pushbutton is attached to
const int ledPin = 13;       // the pin that the LED is attached to
const int goodSprayPin = 50;  //Arduino pin for good spray motor
const int badSprayPin = 51;   //Arduino pin for bad spray motor
const int goodFanPin = 40;    //Arduino pin for good fan
const int badFanPin = 41;    //Arduino pin for bad fan

int buttonState[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // current state of the button
int lastButtonState[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // previous state of the button
int pivot = -1;
//int goodFanStartTime; //stopwatch for operating good fan for certain time
//int badFanStartTime;  //stopwatch for operating bad fan for certain time
int rightPoints = 0;
int wrongPoints = 0;
String incomingString = "Test 4";

void setup() {
  for (int i = 0; i < 8; i++)
  {
    buttonPin[i] = 30 + i;
    // initialize the button pin as a input:
    pinMode(buttonPin[i], INPUT);
  }
  // initialize the motor pins as an output:
  pinMode(goodSprayPin, OUTPUT);
  pinMode(badSprayPin, OUTPUT);
  // initialize serial communication:
  Serial.begin(9600);
}

void loop() {
  //Check if any button is pressed. Play corresponding reference note if pressed.
  checkButtons();

  //Check the most recent performance and evaluate.
  checkPerformance();

  //Spray time! Based on cumulative evaluation results from last 3 seconds, spray good/ bad.
  if (millis() % 3000 >= 0 && millis() % 3000 < 5) //every 3 seconds
  {
    if (rightPoints > wrongPoints)    //if cumulative rightPoints higher than wrong
      sprayGood();
    else
      sprayBad();

    rightPoints = 0;    //
    wrongPoints = 0;
  }

  //turn fans off if sufficient time has elapsed
  if (millis() - 1000 > goodFanStartTime) //if good fan was ON for 1000ms
  {
    digitalWrite(goodFanPin, LOW); //Turn good fan off
  }
  if (millis() - 1000 > badFanStartTime) //if bad fan was ON for 1000ms
  {
    digitalWrite(badFanPin, LOW); //Turn bad fan off
  }
}

void checkButtons()
{
  for (int i = 0; i < 8; i++)
  {
    buttonState[i] = digitalRead(buttonPin[i]);
    Serial.print("btn");
    Serial.print(i + 1);
    Serial.print(" ");
    if (buttonState[i] != lastButtonState[i] && buttonState[i] == HIGH)
    {
      if (pivot == i) // toggle back to zero.
      {
        pivot = -1;
        Serial.println(0);
      }
      else      //   if (pivot != i), activate this button.
      {
        pivot = i;
        Serial.println(1);
      }
    }
    else
    {
      if (pivot == i)
        Serial.println(1);
      else
        Serial.println(0);
    }
    // save the current state as the last state,
    //for next time through the loop
    lastButtonState[i] = buttonState[i];
  }
}

void sprayGood()
{
  digitalWrite(goodSprayPin, HIGH); //Good spray trigger
  delay(50);  //trigger stays momentarily ON for the motor
  digitalWrite(goodSprayPin, LOW); //Good spray off

  digitalWrite(goodFanPin, HIGH); //Trigger fan after spraying.
  //  goodFanStartTime = millis();  //Good fan was turned on at this point in time. It should be turned off after 1000ms.
}

void sprayBad()
{
  digitalWrite(badSprayPin, HIGH); //Bad spray trigger
  delay(50);  //trigger stays momentarily ON for the motor
  digitalWrite(badSprayPin, LOW); //Bad spray off

  digitalWrite(badFanPin, HIGH); //Trigger fan after spraying.
  //  badFanStartTime = millis();  //Bad fan was turned on at this point in time. It should be turned off after 1000ms.
}

void checkPerformance()
{
  if (Serial.available() > 0)    
    //Check for serial communications input from MAX. MAX listens to user's voice and detects the fequency.
    //if the performance is not being evaluated, MAX sends "hh" to arduino.
    //if the frequency is right, MAX sends "btn1 a1" or "btn2 b1" or "btn3 c1"... to arduino.
    //if the frequency is wrong, MAX sends "btn1 a0" or "btn2 b0" or "btn3 c0"...to arduino.
    
    { 
    incomingString = Serial.readStringUntil('\n');

    String firstValue = incomingString.substring(0,  incomingString.indexOf(','));
    String secondValue = incomingString.substring(incomingString.indexOf(',') + 1,  incomingString.indexOf(',', incomingString.indexOf(',') + 1));

    if (secondValue == "hh")
    { //Off. All buttons are off, stop evaluating. Set points to zero for next evaluation cycle.
      rightPoints = 0;
      wrongPoints = 0;
    }
    else if (secondValue == "a1" || secondValue == "b1" || secondValue == "c1" || secondValue == "d1" || secondValue == "e1" || secondValue == "f1" || secondValue == "g1" )
      rightPoints++;    //good
    else
      wrongPoints++;    //bad
  }
}

