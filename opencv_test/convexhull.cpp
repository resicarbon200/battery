#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>        //cvtColor
#include <opencv2/imgproc/types_c.h>  //定数CV_**
#include <opencv2/highgui.hpp>        //Canny

using namespace cv;
using namespace std;

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

  Mat frame;    //1フレームの画像
  Mat smoothed;
  Mat gray;
  Mat binary;
  Mat reversed;
  Mat edge;

  double area;
  
  while(1)//無限ループ
  {
    cap >> frame; //USBカメラが得た動画の１フレームを格納

    //ぼかし（ノイズ対策）
    blur(frame, smoothed, Size(15, 15));

    //グレースケールに変換
    cvtColor(smoothed, gray, CV_BGR2GRAY);   

    //二値化
    threshold(gray, binary, 100, 255, THRESH_BINARY);   //第3引数が閾値

    //反転
    bitwise_not(binary, reversed);

    //輪郭の座標リスト
    vector<vector<Point>> contours;
    
    //輪郭の取得
    findContours(reversed, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    Mat1b rev = reversed;    //Mat1bに変換

    //輪郭の処理
    for(auto cont = contours.begin(); cont != contours.end(); cont++) {
        polylines(smoothed, *cont, true, Scalar(0, 255, 0), 2);  //輪郭描画

        vector<Point> approx;
        approxPolyDP(*cont, approx, 10, true);    //多角形近似

        if(approx.size() == 4) {
          polylines(smoothed, approx, true, Scalar(255, 0, 0), 4);  //近似図形描画

          int ul, ur, ll, lr;
          Point upper, lower, lefter, righter;

          //左上の頂点を探す
          int min = INT_MAX;
          for(int i = 0; i < 4; i++) {
            int sum = approx[i].x + approx[i].y;
            if(sum < min){
              min = sum;
              ul = i;
            }
          }

          ll = (ul + 1) % 4;
          lr = (ul + 2) % 4;
          ur = (ul + 3) % 4;

          upper = approx[ur] - approx[ul];
          lower = approx[lr] - approx[ll];
          lefter = approx[ll] - approx[ul];
          righter = approx[lr] - approx[ur];

          cout << "上の辺" << upper << endl;
          cout << "下の辺" << lower << endl;
          cout << "左の辺" << lefter << endl;
          cout << "右の辺" << righter << endl;

          cout << endl;
          cout << "1=黒/0=白の行列" << endl;
          cout << ">" << endl;

          const int marker = 0x9a1e;

          for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 6; j++) {
              Point p = approx[ul] + (upper * (j + 0.5) + lower * (j + 0.5) + lefter * (i + 0.5) + righter * (i + 0.5)) / 12;
              circle(smoothed, p, 5, Scalar(0, 0, 255), -1);
              int brightness = rev(p);
              cout << brightness / 255 << " ";
            }
            cout << endl;
          }

          cout << endl;

          // vector<cv::Point> convh;
          // convexHull(*cont, convh);     //凸包取得
          // area = contourArea(convh);    //面積取得
        }
    }

    // putText(smoothed, to_string(area).c_str(), Point(0, 50), FONT_HERSHEY_PLAIN, 4, (0, 0, 0), 5, LINE_AA);

    imshow("window", smoothed);   //画像を表示

    // int key = waitKey(1000);   //キー待ち(ミリ秒)
    int key = waitKey(1);
    
    if(key == 'q') {  //qが押されたとき
      break;//whileループから抜ける（終了）
    }
  }
  destroyAllWindows();
  return 0;
}
