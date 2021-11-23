#include "class/Marker.hpp"
#include "class/PosMarker.hpp"

#include <iostream>
#include <chrono>
#include <ctime>
#include <memory>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

const float TAR_DEPTH = 25.0;   //目標距離 [cm]
const float TOL_DEPTH = 2.0;    //距離許容差 [cm]
const float TOL_ANGLE_LOOSE = 20.0;   //おおらかな角度許容差 [度]
const float TOL_ANGLE_STRICT = 10.0;   //厳密な角度許容差 [度]
const float TOL_DEFLEC = 0.1;   //カメラ角度許容差

void msleep(int ms) {   //ミリ秒スリープ
  struct timespec ts;

  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;

  nanosleep(&ts, NULL);
}

int main(void) {
  std::chrono::system_clock::time_point  start_time, end_time;  //時間計測用
  double elapsed;		//時間計測用

  Marker mk;

  mk.setMarker(0x9a1e);   //探すマーカーの模様を16進数4桁で設定

  std::unique_ptr<PosMarker> pm;  //スマートポインタ

  int ID = 0x11;                    //ArduinoのID
  int fd_motor = wiringPiI2CSetup(ID);

  signed char ret;    //Arduino返答(回転角度/4.5)

  float tol_angle = TOL_ANGLE_STRICT;    //角度許容差

  //============================================================
  //

  char buf[13];

  int fd_button = wiringPiI2CSetup(0x10);
  if (fd_button == -1) {
    fprintf(stderr, "Failed to initialize I2C. errno = %d\n", errno);
    return 1;
  } else {
    std::cout << "initializing I2C succeed" << std::endl;
  }

  //============================================================

  while (1) {
    while (1) {   //始動ボタン
      if (read(fd_button, buf, 13) != 0) {
        if (buf[4] == '1' && buf[6] == '0') {
          break;
        }
      }
    }

    while (1) {
      start_time = std::chrono::system_clock::now(); //計測開始時間
      
      pm = std::move(mk.processing());  //processingで作ったスマートポインタの所有権を移動する


      ret = wiringPiI2CRead(fd_motor);
      std::cout << "\t" << (int)ret << std::endl;

      if (pm != nullptr) {
        // std::cout << pm->getDepth() << std::endl;   //距離を表示
        // std::cout << pm->getAngle() << std::endl;   //角度を表示
        std::cout << pm->getDeflec();  //中心座標が右寄りなら正の数，左よりなら負の数を表示

        //============================================================
        //移動制御

        if (-tol_angle < pm->getAngle() && pm->getAngle() < tol_angle) {    //マーカーが移動体の方を向いているとき
          tol_angle = TOL_ANGLE_LOOSE;

          if (-1 < ret && ret < 1) {    //カメラが移動体の正面方向を向いているとき

            if (pm->getDepth() > TAR_DEPTH + TOL_DEPTH) {     //マーカーが遠いとき
              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x01)) < 0){  //前進
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x01\"" << std::endl;
              }
            }

            if (pm->getDepth() < TAR_DEPTH - TOL_DEPTH) {     //マーカーが近いとき
              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x02)) < 0){  //後退
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x02\"" << std::endl;
              }
            }

          } else {    //カメラが移動体の正面方向を向いていないとき
            tol_angle = TOL_ANGLE_STRICT;

            if (ret < 0) {     //カメラが右を向いているとき
              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x04)) < 0){  //右旋回
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x04\"" << std::endl;
              }

              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x09)) < 0){  //カメラ左回転
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x09\"" << std::endl;
              }
            }
            
            if (ret > 0) {     //カメラが左を向いているとき
              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x03)) < 0){  //左旋回
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x03\"" << std::endl;
              }

              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x0a)) < 0){  //カメラ右回転
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x0a\"" << std::endl;
              }
            }
          }

        } else { //マーカーが移動体の方を向いていないとき

          if ((90 - pm->getAngle()) / 2.7 - 2 < ret && ret < (90 - pm->getAngle()) / 2.7 + 2) {   //Arduinoの定数stepsに合わせて変更

            if (pm->getAngle() > tol_angle) {
              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x01)) < 0){  //前進
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x01\"" << std::endl;
              }
            }

            if (pm->getAngle() < -tol_angle) {
              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x02)) < 0){  //後退
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x02\"" << std::endl;
              }
            }

          } else {

            if (ret < (90 - pm->getAngle()) / 2.7) {     //移動体が進行方向より左向きのとき
              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x04)) < 0){  //右旋回
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x04\"" << std::endl;
              }

              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x09)) < 0){  //カメラ左回転
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x09\"" << std::endl;
              }
            }
            
            if (ret > (90 - pm->getAngle()) / 2.7) {     //移動体が進行方向より右向きのとき
              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x03)) < 0){  //左旋回
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x03\"" << std::endl;
              }

              if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x0a)) < 0){  //カメラ右回転
                std::cout << "write error" << std::endl;
              } else {
                // std::cout << "write \"0x0a\"" << std::endl;
              }
            }
            
          }

        }

        //============================================================
        //カメラ回転

        if (pm->getDeflec() > TOL_DEFLEC) {     //マーカーが視界右方のとき
          if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x0a)) < 0){
            std::cout << "write error" << std::endl;
          } else {
            // std::cout << "write \"0x0a\"" << std::endl;
          }
        }

        if (pm->getDeflec() < -TOL_DEFLEC) {     //マーカーが視界左方のとき
          if ((wiringPiI2CWriteReg8(fd_motor, 0x00, 0x09)) < 0){
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

      if (read(fd_button, buf, 13) != 0) {  //停止ボタン
        if (buf[6] == '1') {
          std::cout << "stopped" << std::endl;
          break;
        }
      }
    }
  }

  return 0;
}
