#pragma once

#include <bitset>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

class Marker {
  cv::VideoCapture cap; //カメラ
  unsigned int marker;  //マーカーのデータ
  std::bitset<16> bs;   //16bit型
  cv::Mat frame;      //1フレームの画像
  cv::Mat reversed;   //2値化して反転後の画像
public:
  Marker();
  ~Marker();

  void setMarker(unsigned int);
	void calcPos();
};
