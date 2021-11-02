#include "class/Marker.hpp"
#include "class/PosMarker.hpp"

#include <iostream>
#include <chrono>
#include <ctime>
#include <memory>
#include <wiringPi.h>
#include <wiringPiI2C.h>

void msleep(int ms) {
  struct timespec ts;

  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;

  nanosleep(&ts, NULL);
}

int main(void) {
  std::chrono::system_clock::time_point  start_time, end_time;  //時間計測用
  double elapsed;		//時間計測用

  Marker mk;

  mk.setMarker(0x9a1e);

  std::unique_ptr<PosMarker> pm;

  int ID = 0x04;
  int fd = wiringPiI2CSetup(ID);

  while (1) {
    start_time = std::chrono::system_clock::now(); // 計測開始時間
    
    pm = std::move(mk.processing());

    if (pm != nullptr) {
      std::cout << pm->getDepth() << std::endl;   //距離を表示
      // std::cout <<< pm->getAngle() << std::endl;   //角度を表示
      // std::cout <<< pm->getDeflec() << std::endl;  //中心座標が右寄りなら正の数，左よりなら負の数を表示
    } else {
      std::cout << "?" << std::endl;
    }

    pm.reset();

    if ((wiringPiI2CWriteReg8(fd, 0x04, 0x52)) < 0){
      printf("write error");
    }

    end_time = std::chrono::system_clock::now();  // 計測終了時間
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    if(elapsed >= 1) {
      msleep(500 - elapsed);
    }
  }

  return 0;
}
