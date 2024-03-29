#include <opencv2/imgproc/types_c.h>  //定数CV_**

#include <bitset>
#include <iostream>
#include <opencv2/highgui.hpp>  //Canny
#include <opencv2/imgproc.hpp>  //cvtColor
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(void) {
  cout << "* PROGRAM START" << endl;
  cout << "* Accessing a camera... " << flush;

  VideoCapture cap(0);  // USBカメラのオープン

  if (!cap.isOpened())  //カメラが起動できなかった時のエラー処理
  {
    cerr << "failed." << endl;
    return -1;
  }
  cout << "succeed." << endl;
  cout << "* Press 'q' to finish this program." << endl;

  const unsigned int marker = 0x2a69;  //マーカーのデータ
  bitset<32> bs(marker);

  Mat frame;  // 1フレームの画像
  Mat smoothed;
  Mat gray;
  Mat binary;
  Mat reversed;
  Mat edge;

  double area;

  while (1)  //無限ループ
  {
    cap >> frame;  // USBカメラが得た動画の１フレームを格納

    //ぼかし（ノイズ対策）
    // blur(frame, frame, Size(2, 2));

    Vec3b *pbgr;           //カラー画素(青緑赤)
    unsigned char *pgray;  //グレースケール画素
    reversed = Mat::zeros(cv::Size(frame.cols, frame.rows), CV_8UC1);
    for (int j = 0; j < frame.rows; j++) {
      pbgr = frame.ptr<Vec3b>(j);
      pgray = reversed.ptr<unsigned char>(j);
      for (int i = 0; i < frame.cols; i++) {
        float H = ((*pbgr)[1] - (*pbgr)[0]) /
                  (2.0 * (*pbgr)[2] - (*pbgr)[0] - (*pbgr)[1]);  //色相
        float S = ((*pbgr)[2] - std::min((*pbgr)[0], (*pbgr)[1])) /
                  ((*pbgr)[2] + 1.0);  //彩度
        if (-0.4 < H && H < 0.4 && S > 0.2) {
          *pgray = 255;
        } else {
          *pgray = 0;
        }
        ++pbgr;
        ++pgray;
      }
    }

    //輪郭の座標リスト
    vector<vector<Point>> contours;

    //輪郭の取得
    findContours(reversed, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    Mat1b rev = reversed;  // Mat1bに変換

    //輪郭の処理
    for (auto cont = contours.begin(); cont != contours.end(); cont++) {
      polylines(smoothed, *cont, true, Scalar(0, 255, 0), 2);  //輪郭描画

      vector<Point> approx;
      approxPolyDP(*cont, approx, 10, true);  //多角形近似

      if (approx.size() == 4) {
        polylines(smoothed, approx, true, Scalar(255, 0, 0), 4);  //近似図形描画

        int ul, ur, ll, lr;
        Point upper, lower, lefter, righter;

        //左上の頂点を探す
        int min = INT_MAX;
        for (int i = 0; i < 4; i++) {
          int sum = approx[i].x + approx[i].y;
          if (sum < min) {
            min = sum;
            ul = i;  //左上 UpperLeft
          }
        }

        ll = (ul + 1) % 4;  // LowerLeft
        lr = (ul + 2) % 4;  // LowerRight
        ur = (ul + 3) % 4;  // UpperRight

        upper = approx[ur] - approx[ul];
        lower = approx[lr] - approx[ll];
        lefter = approx[ll] - approx[ul];
        righter = approx[lr] - approx[ur];

        cout << "********************" << endl;

        cout << "中心座標" << approx[0] + approx[1] + approx[2] + approx[3] / 4
             << endl;
        cout << endl;

        cout << "上の辺" << upper << endl;
        cout << "下の辺" << lower << endl;
        cout << "左の辺" << lefter << endl;
        cout << "右の辺" << righter << endl;
        cout << endl;

        cout << "大きさ" << (norm(lefter) + norm(righter)) / 2 << endl;
        cout << endl;

        cout << "1=赤/0=白の行列" << endl;
        cout << "[" << endl;

        int cnt_outer = 0;
        int cnt_black = 0;
        int cnt_white = 0;

        for (int i = 0; i < 6; i++) {
          for (int j = 0; j < 6; j++) {
            Point p = approx[ul] + upper * (2 * j + 1) / 12 +
                      (lefter * (11 - 2 * j) + righter * (2 * j + 1)) *
                          (2 * i + 1) / 144;
            circle(smoothed, p, 5, Scalar(0, 0, 255), -1);

            int brightness;
            if (p.x < rev.cols && p.y < rev.rows) {
              brightness = rev(p);  //輝度　ただし白黒反転後の値なので注意
              cout << brightness / 255 << " ";
            } else {
              brightness = -1;
              cout << "x ";
            }

            if (i == 0 || i == 5 || j == 0 || j == 5) {
              if (brightness == 255) {
                cnt_outer++;
              }
            } else if ((marker >> (4 - i) * 4 + (4 - j)) & 1u != 0) {
              if (brightness == 255) {
                cnt_black++;
              }
            } else {
              if (brightness == 0) {
                cnt_white++;
              }
            }
          }
          cout << endl;
        }

        cout << "]" << endl;

        cout << "枠一致率:" << cnt_outer << "/" << 20 << endl;
        cout << "赤一致率:" << cnt_black << "/" << bs.count() << endl;
        cout << "白一致率:" << cnt_white << "/" << 16 - bs.count() << endl;

        if (cnt_outer / 20.0 > 0.8 && cnt_black / (float)bs.count() > 0.8 &&
            cnt_white / (float)bs.count() > 0.8) {
          cout << "\033[1;5;33m HIT \033[m" << endl;
        } else {
          cout << endl;
        }
        cout << endl;
      }
    }

    // imshow("window", smoothed);   //画像を表示
    // imshow("window", frame);  //画像を表示

    cout << "1ループ終了" << endl;
    cout << endl;

    // int key = waitKey(1);

    // if (key == 'q') {  // qが押されたとき
    //   break;           // whileループから抜ける（終了）
    // }
  }
  destroyAllWindows();
  return 0;
}
