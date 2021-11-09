#include <wiringPiI2C.h>

int main(void) {
  int ID = 0x04;
  int fd = wiringPiI2CSetup(ID);

  while (1) {
    if ((wiringPiI2CWriteReg8(fd, 0x00, 0x52)) < 0){
      printf("write error");
    } else {
      printf("write \"0x52\"");
    }
  }
}