#include "class/Marker.hpp"
#include "class/PosMarker.hpp"

#include <iostream>
#include <chrono>
#include <ctime>
#include <memory>
#include <wiringPiI2C.h>

const float TAR_DEPTH = 20.0;   //目標距離
const float TOL_DEPTH = 2.0;    //距離許容差
const float TOL_ANGLE = 10.0;   //角度許容差
const float TOL_DEFLEC = 0.3;   //カメラ角度許容差

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

  int ID = 0x11;
  int fd = wiringPiI2CSetup(ID);

  signed char ret;    //Arduino返答(回転角度/4.5)

  while (1) {
    start_time = std::chrono::system_clock::now(); // 計測開始時間
    
    pm = std::move(mk.processing());


    ret = wiringPiI2CReadReg8(fd, 0x11);
    std::cout << "\t" << (int)ret << std::endl;

    if (pm != nullptr) {
      // std::cout << pm->getDepth() << std::endl;   //距離を表示
      // std::cout << pm->getAngle() << std::endl;   //角度を表示
      std::cout << pm->getDeflec();  //中心座標が右寄りなら正の数，左よりなら負の数を表示

      //============================================================
      //移動制御

      if (-TOL_ANGLE < pm->getAngle() && pm->getAngle() < TOL_ANGLE) {    //マーカーが移動体の方を向いているとき
        if (ret = 0) {    //カメラが移動体の正面方向を向いているとき

          if (pm->getDepth() > TAR_DEPTH + TOL_DEPTH) {     //マーカーが遠いとき
            if ((wiringPiI2CWriteReg8(fd, 0x00, 0x01)) < 0){
              std::cout << "write error" << std::endl;
            } else {
              // std::cout << "write \"0x01\"" << std::endl;
            }
          }

          if (pm->getDepth() < TAR_DEPTH - TOL_DEPTH) {     //マーカーが近いとき
            if ((wiringPiI2CWriteReg8(fd, 0x00, 0x02)) < 0){
              std::cout << "write error" << std::endl;
            } else {
              // std::cout << "write \"0x0a\"" << std::endl;
            }
          }

        } else {    //カメラが移動体の正面方向を向いていないとき

          if (ret < 0) {     //カメラが右を向いているとき
            if ((wiringPiI2CWriteReg8(fd, 0x00, 0x04)) < 0){
              std::cout << "write error" << std::endl;
            } else {
              // std::cout << "write \"0x04\"" << std::endl;
            }
          }
          
          if (ret > 0) {     //カメラが左を向いているとき
            if ((wiringPiI2CWriteReg8(fd, 0x00, 0x03)) < 0){
              std::cout << "write error" << std::endl;
            } else {
              // std::cout << "write \"0x03\"" << std::endl;
            }
          }

      } else { //マーカーが移動体の方を向いていないとき

      }

      //============================================================
      //カメラ回転

      if (pm->getDeflec() > CAMERA_DEF) {     //マーカーが視界右方のとき
        if ((wiringPiI2CWriteReg8(fd, 0x00, 0x0a)) < 0){
          std::cout << "write error" << std::endl;
        } else {
          // std::cout << "write \"0x0a\"" << std::endl;
        }
      }

      if (pm->getDeflec() < -CAMERA_DEF) {     //マーカーが視界左方のとき
        if ((wiringPiI2CWriteReg8(fd, 0x00, 0x09)) < 0){
          std::cout << "write error" << std::endl;
        } else {
          // std::cout << "write \"0x09\"" << std::endl;
        }
      }

      //============================================================
    } else {
      std::cout << "?";
    }


    pm.reset();

    end_time = std::chrono::system_clock::now();  // 計測終了時間
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    if(elapsed >= 1) {
      msleep(100 - elapsed);
    }
  }

  return 0;
}
