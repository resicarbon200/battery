#include "Marker.hpp"

#include <iostream>
#include <algorithm>
#include <bitset>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>        //cvtColor
#include <opencv2/imgproc/types_c.h>  //定数CV_**
#include "PosMarker.hpp"

using namespace cv;
using namespace std;

Marker::Marker() {
  cout << "* Accessing a camera... " << flush;

  cap.open(0);

  if(!cap.isOpened()) //カメラが起動できなかった時のエラー処理
  {
    cerr << "failed." << endl;
    exit(-1);
  }
  cout << "succeed." << endl;

  cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
}

Marker::~Marker() {
  cap.release();
}

void Marker::setMarker(unsigned int marker) {
  this->marker = marker;
  bs = marker;
}

unique_ptr<PosMarker> Marker::processing() {
  cv::Mat frame;     // 1フレームの画像
  cv::Mat reversed;  // 2値化して反転後の画像

  cap >> frame; //USBカメラが得た動画の１フレームを格納

  //ぼかし（ノイズ対策）
  // blur(frame, frame, Size(2, 2));

  //白黒マーカー版
  /*
  //グレースケールに変換
  cvtColor(frame, frame, CV_BGR2GRAY);   

  //二値化
  //threshold(frame, frame, 110, 255, THRESH_BINARY);   //第3引数が閾値
  adaptiveThreshold(frame, frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 91, 4);   //第3引数が閾値

  //反転
  bitwise_not(frame, reversed);
  */

  //赤色マーカー版
  // /*
  Vec3b *pbgr;  //カラー画素(青緑赤)
  unsigned char *pgray;   //グレースケール画素
  reversed = Mat::zeros(cv::Size(frame.cols, frame.rows), CV_8UC1);
  for (int j = 0; j < frame.rows; j++) {
    pbgr = frame.ptr<Vec3b>(j);
    pgray = reversed.ptr<unsigned char>(j);
    for (int i = 0; i < frame.cols; i++) {
      float H = ((*pbgr)[1] - (*pbgr)[0]) / (2.0 * (*pbgr)[2] - (*pbgr)[0] - (*pbgr)[1]); //色相
      float S = ((*pbgr)[2] - std::min((*pbgr)[0], (*pbgr)[1])) / ((*pbgr)[2] + 1.0);    //彩度
      if (-0.4 < H && H < 0.4 && S > 0.2) {
        *pgray = 255;
      } else {
        *pgray = 0;
      }
      ++pbgr;
      ++pgray;
    }
  }
  // */

  // imshow("red", reversed);
  // int key = waitKey(300);
  // if (key == 'q') {
  //   destroyAllWindows();
  //   exit(0);
  // }

  //輪郭の座標リスト
  vector<vector<Point>> contours;
  
  //輪郭の取得
  findContours(reversed, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

  Mat1b rev = reversed;    //Mat1bに変換

  //輪郭の処理
  for (auto cont = contours.begin(); cont != contours.end(); cont++) {
    // polylines(smoothed, *cont, true, Scalar(0, 255, 0), 2);  //輪郭描画

    vector<Point> approx;
    approxPolyDP(*cont, approx, 10, true);    //多角形近似

    if (approx.size() == 4) {
      // polylines(smoothed, approx, true, Scalar(255, 0, 0), 4);  //近似図形描画

      int ul, ur, ll, lr;

      //左上の頂点を探す
      int min = INT_MAX;
      for(int i = 0; i < 4; i++) {
        int sum = approx[i].x + approx[i].y;
        if(sum < min){
          min = sum;
          ul = i;   //左上 UpperLeft
        }
      }

      ll = (ul + 1) % 4;  //LowerLeft
      lr = (ul + 2) % 4;  //LowerRight
      ur = (ul + 3) % 4;  //UpperRight

      unique_ptr<PosMarker> pm(new PosMarker(approx[ul], approx[ur], approx[ll], approx[lr]));

      int cnt_outer = 0;
      int cnt_black = 0;
      int cnt_white = 0;

      for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
          Point p = approx[ul] + pm->getUpper() * (2 * j + 1) / 12 + (pm->getLefter() * (11 - 2 * j) + pm->getRighter() * (2 * j + 1)) * (2 * i + 1) / 144;
          
          int brightness;
          if (p.x < rev.cols && p.y < rev.rows){
            brightness = rev(p);    //輝度　ただし白黒反転後の値なので注意
            // cout << brightness / 255 << " ";
          } else {
            brightness = -1;
            // cout << "x ";
          }

          if (i == 0 || i == 5 || j == 0 || j == 5) {
            if(brightness == 255) {
              cnt_outer++;
            }
          } else if ((marker >> (4 - i) * 4 + (4 - j)) & 1u != 0) {
            if(brightness == 255) {
              cnt_black++;
            }
          } else {
            if (brightness == 0) {
              cnt_white++;
            }
          }
        }
        // cout << endl;
      }

      // cout << "]" << endl;

      // cout << "枠一致率:" << cnt_outer << "/" << 20 << endl;
      // cout << "黒一致率:" << cnt_black << "/" << bs.count() << endl;
      // cout << "白一致率:" << cnt_white << "/" << 16 - bs.count() << endl;

      if (cnt_outer / 20.0 > ACCURACY && cnt_black / (float)bs.count() > ACCURACY && cnt_white / (float)bs.count() > ACCURACY) {
        return pm;
        // cout << "\033[1;5;33m HIT \033[m" << endl;
      }
      // else {
        // cout << endl;
      // }
      // cout << endl;
    }
  }

  return nullptr;
}
