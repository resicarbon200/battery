#pragma once

#include <opencv2/opencv.hpp>

class PosMarker {
  cv::Point ul;   //左上の頂点座標
  cv::Point ur;   //右上の頂点座標
  cv::Point ll;   //左下の頂点座標
  cv::Point lr;   //右下の頂点座標
  cv::Point upper;  //上の辺
  cv::Point lower;  //上の辺
  cv::Point lefter;  //上の辺
  cv::Point righter;  //上の辺
  float llen;		//左辺長さ
  float rlen;		//右辺長さ
  float mlen;		//2辺の長さの平均
  float depth;	//距離 [cm]
  float angle;	//マーカ面の角度(左向きが負　右向きが正) [度]
  float deflec;	//映像中のマーカ位置(左寄りが負　右寄りが正)
  
  PosMarker();  //デフォルトコンストラクタの禁止
public:
  PosMarker(cv::Point, cv::Point, cv::Point, cv::Point);
  cv::Point getUpper();
  cv::Point getLower();
  cv::Point getLefter();
  cv::Point getRighter();
  float getLlen();
  float getRlen();
  float getMlen();
  float getDepth();
  float getAngle();
  float getDeflec();
};