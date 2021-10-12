#include "class/Marker.hpp"
#include "class/PosMarker.hpp"

#include <iostream>
#include <chrono>
#include <ctime>
#include <memory>

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

  while(1) {
    start_time = std::chrono::system_clock::now(); // 計測開始時間
    
    pm = std::move(mk.processing());

    if (pm != nullptr) {
      std::cout << pm->getDepth() << std::endl;
    } else {
      std::cout << "?" << std::endl;
    }

    pm.reset();

    end_time = std::chrono::system_clock::now();  // 計測終了時間
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    if(elapsed >= 1) {
      msleep(500 - elapsed);
    }
  }

  return 0;
}
