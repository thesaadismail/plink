#include <SPI.h>
#include <WiFi.h>
#include <Bridge.h>
#include <HttpClient.h>

WiFiClient client;


bool sentConnection = false;
bool gettingData = false;

char ipAddr[] = "http://plink-55964.onmodulus.net";

char ssid[] = "";     //  your network SSID (name) 
char pass[] = "";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char macChar[] = "DEADBEEFFEED";     //  your network SSID (name) 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

void connectToWiFiNetwork()
{
  // attempt to connect using WPA2 encryption:
  Serial.println("Attempting to connect to WPA network...");
  status = WiFi.begin(ssid, pass);

  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  // if you are connected, print out info about the connection:
  else {
    Serial.println("Connected to network");
    printWifiStatus();
  }
}

bool sendingRequest = false;

void retrieveMedicationTimesRequest() {
  if(sendingRequest == false)
  {
    sendingRequest = true;
    Serial.println("\nretrieving medication times request");
    Serial.println("Starting connection to server...");
    // if you get a connection, report back via serial:
    if (client.connect(ipAddr, 80)) {
      Serial.println("connected to server");
      // Make a HTTP request:
      client.println("GET /retrieveTimes HTTP/1.1");
      client.println("Host: plink-55964.onmodulus.net");
      client.println("Connection: close");
      client.println();
    }
    else {
      Serial.println("did not connect to server");
    }
    
  }
}

void retrieveClockTimeRequest() {
  Serial.println("\nretrieving clock time request");
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(ipAddr, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /getClockTimeForArduino HTTP/1.1");
    client.println("Host: plink-55964.onmodulus.net");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("did not connect to server");
  }
}

int lineNum = 0;
int dataLen = 0;
char* rawTimesCharArr;
int rawTimesCharArrCount = -1;

char* checkForWiFiData() {
  // if there are incoming bytes available 
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();

      //Serial.write(c);
      
      if(lineNum == 6 && rawTimesCharArrCount == -1) {
        char charToStr[2] = "\0";
        charToStr[0] = c;
        charToStr[1] = '\0';
        
        int dataLen = 0;
        dataLen = atoi(charToStr);
        rawTimesCharArr = new char[dataLen];
        
        //Serial.println("====");
        //Serial.println(dataLen);
        rawTimesCharArrCount++;
      }
      else if(lineNum == 7) {
        rawTimesCharArr[rawTimesCharArrCount] = c;
        rawTimesCharArrCount++;
      }
      
      if(c == '\n') {
        lineNum++;
        //Serial.println("==== NEW LINE ====");
      }
  }
  
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    //Serial.println("disconnecting from server.");
    client.stop();
    if(rawTimesCharArrCount!=-1)
    {
      //Serial.println("again");
      rawTimesCharArrCount = -1;
      lineNum = 0;
      sendingRequest = false;
      return rawTimesCharArr;
    }
  }

  return 0;
}

void printWifiStatus() {
 // print the SSID of the network you're attached to:
 Serial.print("SSID: ");
 Serial.println(WiFi.SSID());

 // print your WiFi shield's IP address:
 IPAddress ip = WiFi.localIP();
 Serial.print("IP Address: ");
 Serial.println(ip);

 // print the received signal strength:
 long rssi = WiFi.RSSI();
 Serial.print("signal strength (RSSI):");
 Serial.print(rssi);
 Serial.println(" dBm");
}

