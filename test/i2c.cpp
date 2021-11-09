#include <iostream>
#include <wiringPiI2C.h>

int main(void) {
  int ID = 0x11;
  int fd = wiringPiI2CSetup(ID);

  // while (1) {
    if ((wiringPiI2CWriteReg8(fd, 0x00, 0x01)) < 0){
      std::cout << "write error" << std::endl;
    } else {
      std::cout << "write \"0x52\"" << std::endl;
    }
  // }
}
