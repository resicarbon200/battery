#include <iostream>
#include <bitset>
#include <chrono>
#include <ctime>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>        //cvtColor
#include <opencv2/imgproc/types_c.h>  //定数CV_**
#include <opencv2/highgui.hpp>        //Canny

using namespace cv;
using namespace std;

void msleep(int ms) {
  struct timespec ts;

  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;

  nanosleep(&ts, NULL);
}

int main(void) {
  cout << "* PROGRAM START" << endl;
  cout << "* Accessing a camera... " << flush;

  VideoCapture cap(0); // USBカメラのオープン

  if(!cap.isOpened()) //カメラが起動できなかった時のエラー処理
  {
    cerr << "failed." << endl;
    return -1;
  }
  cout << "succeed." << endl;
  cout << "* Press 'q' to finish this program." << endl;

  const unsigned int marker = 0x9a1e;   //マーカーのデータ
  bitset<16> bs(marker);
  std::chrono::system_clock::time_point  start_time, end_time;
  double elapsed;

  Mat frame;    //1フレームの画像
  // Mat smoothed;
  // Mat gray;
  // Mat binary;
  Mat reversed;

  //double area;

  while(1)//無限ループ
  {
    start_time = std::chrono::system_clock::now(); // 計測開始時間

    cap >> frame; //USBカメラが得た動画の１フレームを格納

    //ぼかし（ノイズ対策）
    blur(frame, frame, Size(2, 2));

    //グレースケールに変換
    cvtColor(frame, frame, CV_BGR2GRAY);   

    //二値化
    //threshold(frame, frame, 110, 255, THRESH_BINARY);   //第3引数が閾値
    adaptiveThreshold(frame, frame, 255, ADAPTIVE_THRESH_GAUSSIAN_C. THRESH_BINARY, 50, 10);   //第3引数が閾値

    //反転
    bitwise_not(frame, reversed);

    //輪郭の座標リスト
    vector<vector<Point>> contours;
    
    //輪郭の取得
    findContours(reversed, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    Mat1b rev = reversed;    //Mat1bに変換

    //輪郭の処理
    for(auto cont = contours.begin(); cont != contours.end(); cont++) {
        // polylines(smoothed, *cont, true, Scalar(0, 255, 0), 2);  //輪郭描画

        vector<Point> approx;
        approxPolyDP(*cont, approx, 10, true);    //多角形近似

        if(approx.size() == 4) {
          // polylines(smoothed, approx, true, Scalar(255, 0, 0), 4);  //近似図形描画

          int ul, ur, ll, lr;
          Point upper, lower, lefter, righter;

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

          upper = approx[ur] - approx[ul];
          lower = approx[lr] - approx[ll];
          lefter = approx[ll] - approx[ul];
          righter = approx[lr] - approx[ur];

          cout << "********************" << endl;

          cout << "中心座標" << approx[0] + approx[1] + approx[2] + approx[3] / 4 << endl;
          cout << endl;

          cout << "上の辺" << upper << endl;
          cout << "下の辺" << lower << endl;
          cout << "左の辺" << lefter << endl;
          cout << "右の辺" << righter << endl;
          cout << endl;

          double llen, rlen, mlen;
          llen = norm(lefter);
          rlen = norm(righter);
          mlen = (llen + rlen) / 2;

          cout << "左の辺の長さ:" << llen << endl;
          cout << "右の辺の長さ:" << rlen << endl;
          cout << "平均:" << mlen << endl;
          cout << "距離:" << 2900 / mlen + 5 << "cm" << endl;
          cout << "角度:" << asin(((2900 / rlen) - (2900 / llen)) / 6) * 180 / M_PI << endl;
          cout << endl;

          cout << "1=黒/0=白の行列" << endl;
          cout << "[" << endl;

          int cnt_outer = 0;
          int cnt_black = 0;
          int cnt_white = 0;

          for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 6; j++) {
              Point p = approx[ul] + upper * (2 * j + 1) / 12 + (lefter * (11 - 2 * j) + righter * (2 * j + 1)) * (2 * i + 1) / 144;
              // circle(smoothed, p, 5, Scalar(0, 0, 255), -1);
              
              int brightness;
              if(p.x < rev.cols && p.y < rev.rows){
                brightness = rev(p);    //輝度　ただし白黒反転後の値なので注意
                cout << brightness / 255 << " ";
              } else {
                brightness = -1;
                cout << "x ";
              }

              if(i == 0 || i == 5 || j == 0 || j == 5) {
                if(brightness == 255) {
                  cnt_outer++;
                }
              }else if((marker >> (4 - i) * 4 + (4 - j)) & 1u != 0) {
                if(brightness == 255) {
                  cnt_black++;
                }
              }else {
                if(brightness == 0) {
                  cnt_white++;
                }
              }
            }
            cout << endl;
          }

          cout << "]" << endl;

          cout << "枠一致率:" << cnt_outer << "/" << 20 << endl;
          cout << "黒一致率:" << cnt_black << "/" << bs.count() << endl;
          cout << "白一致率:" << cnt_white << "/" << 16 - bs.count() << endl;

          if(cnt_outer / 20.0 > 0.8 && cnt_black / (float)bs.count() > 0.8 && cnt_white / (float)bs.count() > 0.8) {
            cout << "\033[1;5;33m HIT \033[m" << endl;
          } else {
            cout << endl;
          }
          cout << endl;
        }
    }

    // imshow("window", smoothed);   //画像を表示

    cout << "1ループ終了" << endl;
    cout << endl;

    imshow("window", frame);   //画像を表示
    waitKey(1);

    end_time = std::chrono::system_clock::now();  // 計測終了時間
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    cout << "time:" << elapsed << "ms" << endl << endl;

    if(elapsed >= 1) {
      msleep(500 - elapsed);
    }

    // int key = waitKey(1000);   //キー待ち(ミリ秒)
    // int key = waitKey(1);
    
    // if(key == 'q') {  //qが押されたとき
    //   break;//whileループから抜ける（終了）
    // }
  }
  destroyAllWindows();
  
  return 0;
}
