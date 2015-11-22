/*
 * MemoryMan manges memory on the device.
 */

#include <EEPROM.h>

int panLocation = 0;

/*
 * PAN will always be 2 bytes. 
 */
void storePANInMemory(short value) {
  //One = Most significant -> Two = Least significant byte
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);
  EEPROM.write(panLocation, two);
  EEPROM.write(panLocation + 1, one);
}

short retrievePANInMemory() {
  long two = EEPROM.read(panLocation);
  long one = EEPROM.read(panLocation + 1);
  return (short) (((two << 0) & 0xFF) + ((one << 8) & 0xFFFF));
}

void clearItemsFromMemory()
{
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
}



