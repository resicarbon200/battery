#include "PosMarker.hpp"

#include <opencv2/opencv.hpp>

using cv::Point;

const float PARAM_D = 2532.9;     // 距離計算用パラメータ1
const float PARAM_D_BIAS = 4.23;  // 距離計算用パラメータ2(バイアス)
const float PARAM_D_ERR = -3.06;  // 距離計算用パラメータ3(誤差調整)
const float MARKER_SIZE = 6.0;      // マーカーの1辺の長さ(cm)

PosMarker::PosMarker(Point ul, Point ur, Point ll, Point lr) {
  this->ul = ul;
  this->ur = ur;
  this->ll = ll;
  this->lr = lr;

  upper = ur - ul;
  lower = lr - ll;
  lefter = ll - ul;
  righter = lr - ur;

  llen = norm(lefter);
  rlen = norm(righter);
  mlen = (llen + rlen) / 2;

  depth = PARAM_D / (mlen + PARAM_D_ERR) + PARAM_D_BIAS;
  angle = asin(((PARAM_D / (rlen + PARAM_D_ERR)) - (PARAM_D / (llen + PARAM_D_ERR))) / MARKER_SIZE) * 180 / M_PI;

  deflec = (ul.x + ur.x + ll.x + lr.x) / (4.0 * 320) - 1; //横幅640px
}

cv::Point PosMarker::getUpper() { return upper; }
cv::Point PosMarker::getLower() { return lower; }
cv::Point PosMarker::getLefter() { return lefter; }
cv::Point PosMarker::getRighter() { return righter; }
float PosMarker::getLlen() { return llen; }
float PosMarker::getRlen() { return rlen; }
float PosMarker::getMlen() { return mlen; }
float PosMarker::getDepth() { return depth; }
float PosMarker::getAngle() { return angle; }
float PosMarker::getDeflec() { return deflec; }
