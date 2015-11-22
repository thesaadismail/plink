#include <Printers.h>
#include <XBee.h>

unsigned long start = millis();

PlinkMessage checkForPacket()
{
  PlinkMessage plinkPacketResponse;
  plinkPacketResponse.validPacket = false;
  
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something
    Serial.println("got something");
    uint8_t apiID = xbee.getResponse().getApiId();
    if (apiID == RX_16_RESPONSE || apiID == RX_64_RESPONSE) {
      // got a rx packet
      Serial.println("got an rx packet");

      uint8_t option = 0;
      uint8_t data = 0;

      if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
        Serial.println("RX_16_RESPONSE");
        xbee.getResponse().getRx16Response(plinkPacketResponse.responsePacket);
        option = plinkPacketResponse.responsePacket.getOption();
        data = plinkPacketResponse.responsePacket.getData(0);
        Serial.println(data);
        plinkPacketResponse.validPacket = true;

        //          for (int i = 0; i < rx16.getDataLength (); i++)
        //            Serial.print((char) rx16.getData(i));
        //            Serial.println();


      } else {
        //        Rx64Response rx64 = Rx64Response();
        //          xbee.getResponse().getRx64Response(rx64);
        //          option = rx64.getOption();
        //          data = rx64.getData(0);
      }

      // TODO check option, rssi bytes
      flashLed(statusLed, 1, 10);

      // set dataLed PWM to value of the first byte in the data
      analogWrite(statusLed, data);
    }
    else if (apiID == TX_STATUS_RESPONSE) {
      Serial.println("got an tx response packet");
    }
    else if (apiID == TX_16_REQUEST || apiID == TX_64_REQUEST) {
      Serial.println("got an tx request packet");
    }
    else {
      Serial.println("not something we were expecting");
      flashLed(errorLed, 1, 25);
    }
  } else if (xbee.getResponse().isError()) {
    Serial.println("Error reading packet.  Error code: ");
    Serial.println("xbee.getResponse().getErrorCode()");
    // or flash error led
    flashLed(errorLed, 1, 25);
  }
  
  return plinkPacketResponse;
}

boolean sendMessage(Tx16Request tx, boolean waitForResponse)
{
  TxStatusResponse txStatus = TxStatusResponse();

  // start transmitting after a startup delay.  Note: this will rollover to 0 eventually so not best way to handle
  if (millis() - start > 15000) {
    // break down 10-bit reading into two bytes and place in payload
    //      pin5 = analogRead(5);
    //      payload[0] = pin5 >> 8 & 0xff;
    //      payload[1] = pin5 & 0xff;

    xbee.send(tx);

    // flash TX indicator
    flashLed(statusLed, 1, 100);
  }

  if (!waitForResponse) {
    return true;
  }

  boolean success = false;
  // after sending a tx request, we expect a status response
  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {
    // got a response!

    // should be a znet tx status
    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      xbee.getResponse().getTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getStatus() == SUCCESS) {
        // success.  time to celebrate
        flashLed(statusLed, 5, 50);
        success = true;
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        flashLed(errorLed, 3, 500);
        success = false;
      }
    }
  } else if (xbee.getResponse().isError()) {
    //nss.print("Error reading packet.  Error code: ");
    //nss.println(xbee.getResponse().getErrorCode());
    // or flash error led
  } else {
    // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
    flashLed(errorLed, 2, 50);
  }

  delay(1000);
  return success;
}

void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

