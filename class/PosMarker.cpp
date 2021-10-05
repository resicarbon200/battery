#include "PosMarker.hpp"

#include <opencv2/opencv.hpp>

using cv::Point;

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

  depth = 2900 / mlen + 5;
  angle = asin(((2900 / rlen) - (2900 / llen)) / 6) * 180 / M_PI;

  deflec = (ul.x + ur.x + ll.x + lr.x) / (4.0 * 960) - 1; //FHDなので横幅1920px
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
