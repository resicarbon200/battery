#include "class/Marker.hpp"

#include <iostream>
#include <chrono>
#include <ctime>

void msleep(int ms) {
	struct timespec ts;

	ts.tv_sec = ns / 1000;
	ts.tv_nsec = ns % 1000;

	nanosleep(&ts, NULL);
}

int main(void) {
  std::chrono::system_clock::time_point  start_time, end_time;  //時間計測用
  double elapsed;		//時間計測用

  Marker mk;

  mk.setMarker(0x9a1e);

	while(1) {
    start_time = std::chrono::system_clock::now(); // 計測開始時間
		
		

    end_time = std::chrono::system_clock::now();  // 計測終了時間
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

		if(elapsed >= 1) {
			msleep(elapsed);
		}
	}

  return 0;
}
