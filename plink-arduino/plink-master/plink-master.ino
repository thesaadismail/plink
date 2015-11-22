/*
 * Plink Master
 * The master device will 
 *  - send commands to slave devices to display alert
 *  - turn on LED during alerts
 *  - recieve commands from slave devices to dismiss alert
 *  - connect to WiFi
 *  - sync medication alert times
 *  
 *  TODO: Wireup the hardware. Check if LEDs turn on. Check if broadcasting works
 */
 
#include <Printers.h>
#include <XBee.h>
#include "structman.h"
#include <SPI.h>
#include <WiFi.h>
#include <TimeLib.h>

int enableAlertButtonPin = 6;
int dismissAlertButtonPin = 5;

int medicationTimesArr[48];

int alertLed = 9;
int statusLed = 2;
//int dataLed = 3;
int errorLed = 3;

int enableAlertButtonState;
int dismissAlertButtonState;
int resetButtonState;

bool isSending;
bool isDismissing;

int piezoPin = 8;
bool alertEnabled = false;
bool clockTimeSync = false;

XBee xbee;

//3 Buttons: reset, dismiss alert, add light
//1 temp button: enable light

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  
  // initialize the LED pin as an output:
  //pinMode(dataLed, OUTPUT);
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(alertLed, OUTPUT);
  
  digitalWrite(alertLed, LOW);
  digitalWrite(statusLed, LOW);
  digitalWrite(errorLed, LOW);
  flashLed(alertLed, 3, 500);
  
  // initialize the pushbutton pin as an input:
  pinMode(enableAlertButtonPin, INPUT);
  pinMode(dismissAlertButtonPin, INPUT);
  
  // Tell XBee to use Hardware Serial. It's also possible to use SoftwareSerial
  xbee = XBee();
  xbee.setSerial(Serial); 

  medicationTimesArr[16-1] = 1; 

  connectToWiFiNetwork();
  retrieveClockTimeRequest();
}

void loop() {   
  char* data = checkForWiFiData();
  if(data!=0) {
    if(clockTimeSync == false) {
      //setting time manually for debugging purposes
      setTime(21,58,30,22,11,2015);
      
      //setupClockTime(data);
      clockTimeSync = true;
      retrieveMedicationTimesRequest();
    }
    else {
      setupMedicationTimes(data);
    }
  }

  //=====================================
  // SYNC MEDICATION TIMES EVERY 30 MINS
  //=====================================
  if((minute() == 29 || minute() == 59) && second() == 0) {
    retrieveMedicationTimesRequest();
  }
  
  //======================
  // PROCESS XBEE REQUEST
  //======================
  PlinkMessage plinkResponseMessage = checkForPacket();
  if(plinkResponseMessage.validPacket == true) {
    Rx16Response responsePacket = plinkResponseMessage.responsePacket;
    Serial.println(responsePacket.getData(0));
    processData(responsePacket.getData(0));
  }
   
  enableAlertButtonState = digitalRead(enableAlertButtonPin);  
  dismissAlertButtonState = digitalRead(dismissAlertButtonPin);

  //=====================
  // MANAGE STATUS BUTTON
  //=====================
  if (dismissAlertButtonState == HIGH || enableAlertButtonState == HIGH) {
    digitalWrite(statusLed, HIGH);
  } else {
    digitalWrite(statusLed, LOW);
  }
  
  //===============================
  // MANAGE ALERT (LED & BROADCAST)
  //===============================
  bool medicationAlertOn = (medicationTimesArr[hour()] == 1 && minute() == 0 && second() == 0) ||
                            (medicationTimesArr[hour()+24] == 1 && minute() == 0 && second() == 0);

  if(enableAlertButtonState == HIGH || medicationAlertOn) {
    turnOnAlertLed();
    broadcastEnableAlert();
  }

  if(dismissAlertButtonState == HIGH) {
    turnOffAlertLed();
    broadcastDismissAlert();  
  }

  //=====================
  // MANAGE ALERT AUDIO
  //=====================
  if(alertEnabled) {
    tone(piezoPin, 200, 500);
  }
  
  delay(10);
}

void setupClockTime(char* data)
{
  Serial.println(data);
  short count = 0;
  int timeArr[6];
  
  char* next_word = strtok(data, "-");
  while (next_word != NULL) {
    int nextNum = atoi(next_word);
    timeArr[count++] = nextNum;
    next_word = strtok(NULL, "-");
  }

  setTime(timeArr[0], timeArr[1], timeArr[2], timeArr[3], timeArr[4], timeArr[5]); 
}

void setupMedicationTimes(char* data)
{
  //medication times will arrive in hours split by dashes. 
  //0 at the end represents on the hour. 1 represents 30min increment.
  //example... 80-120-161-230
  //this equals, 8am, 12pm, 4:30pm, 11pm
  
  Serial.println(data);
  
  char* next_word = strtok(data, "-");
  while (next_word != NULL) {
    int nextNum = atoi(next_word);
    int medicationTimeInHour = (nextNum/10);
    if(nextNum % 10 == 0)
    {
       medicationTimesArr[medicationTimeInHour] = 1;
    }
    else
    {
       medicationTimesArr[medicationTimeInHour+24] = 1;
    }
    next_word = strtok(NULL, "-");
  }
}

void printAllMedicationTimes() {
  for(int i = 0; i<24; i++)
  {
    Serial.println(i);
    Serial.println(medicationTimesArr[i]);
    Serial.println(medicationTimesArr[i+24]);
    Serial.println();
    delay(20);
  }
}

void turnOnAlertLed() {
   alertEnabled = true;
   digitalWrite(alertLed, HIGH);
}
void turnOffAlertLed() {
   alertEnabled = false;
   digitalWrite(alertLed, LOW);
}

void broadcastEnableAlert() {
  if(isSending == false)
  {
     isSending = true;
     Serial.println("Sending Enable Broadcast");
     uint8_t payload[] = {generateEnableAlertCmd()};
     Tx16Request tx = Tx16Request(0xFFFF, payload, sizeof(payload));
     boolean successful = sendMessage(tx, false);
     Serial.println("Done Sending Enable Broadcast. Status: ");
     Serial.println(successful);
     Serial.println("-----------------------------------");
     isSending = false;
  }
}

void broadcastDismissAlert() {
  if(isDismissing == false)
  {
     isDismissing = true;
     Serial.println("Sending Dismiss Broadcast");
     
     uint8_t payload[] = {generateDismissAlertCmd()};
     Tx16Request tx = Tx16Request(0xFFFF, payload, sizeof(payload));
     boolean successful = sendMessage(tx, false);
   
     Serial.println("Done Sending Dismiss Broadcast. Status: ");
     Serial.println(successful);
     Serial.println("-----------------------------------");
     isDismissing = false;
  }
}

void processData(uint8_t command) {
  if(command == generateEnableAlertCmd()) {
    turnOnAlertLed();
  }
  else if(command == generateDismissAlertCmd()) {
    turnOffAlertLed();
  }
}

void resetDevice() {
  //reset device will clear items stored in memory and create a new PAN ID.
  //the destination address will always be 0 for the master device
  //the source address should be a wildcard so it can broadcast to all devices in the PAN

  clearItemsFromMemory();
  short randomPan = (short) generateRandomPAN();
  storePANInMemory(randomPan);
}

int generateRandomPAN() {
  return random(65535);
}

