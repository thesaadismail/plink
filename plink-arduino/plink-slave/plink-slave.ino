/*
 * Plink Slave
 * The slave device will 
 *  - recieve commands from master device to display alerts
 *  - send commands to master device to dismiss alerts
 *  - recieve commands from master device to dismiss alerts
 */
 
#include <Printers.h>
#include <XBee.h>
#include <SPI.h>
#include "structman.h"

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
}

void loop() {   
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
  if(enableAlertButtonState == HIGH) {
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
