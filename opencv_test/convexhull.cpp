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
  Mat edge;

  // int count;

  while(1)//無限ループ
  {
    cap >> frame; //USBカメラが得た動画の１フレームを格納

    //ぼかし（ノイズ対策）
    blur(frame, smoothed, Size(15, 15));

    //グレースケールに変換
    cvtColor(smoothed, gray, CV_BGR2GRAY);   

    //輪郭の座標リスト
    vector<vector<Point>> contours;

    //二値化
    // threshold(gray, binary, count / 10, 255, THRESH_BINARY);   //第3引数が閾値
    threshold(gray, binary, 55, 140, THRESH_BINARY);   //第3引数が閾値

    //輪郭の取得
    findContours(binary, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
 
    // 検出された輪郭線を緑で描画
    for (auto contour = contours.begin(); contour != contours.end(); contour++){
        polylines(smoothed, *contour, true, Scalar(0, 255, 0), 2);
    }   
 
    // //エッジ検出
    // Canny(gray, edge, 60, 180);

    // putText(frame, to_string(count / 10).c_str(), Point(0, 50), FONT_HERSHEY_PLAIN, 4, (255, 255, 255), 5, LINE_AA);
    // if(count++ > 2500) {
    //   count = 0;
    // }

    imshow("window", smoothed);//画像を表示．

    // int key = waitKey(1000);   //キー待ち(ミリ秒)
    int key = waitKey(1);
    
    if(key == 'q') {  //qが押されたとき
      break;//whileループから抜ける（終了）
    }
  }
  destroyAllWindows();
  return 0;
}
