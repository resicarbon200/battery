#include "Marker.hpp"

#include <iostream>
#include <bitset>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>        //cvtColor
#include <opencv2/imgproc/types_c.h>  //定数CV_**

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
}

Marker::~Marker() {}

void Marker::setMarker(unsigned int marker) {
  this->marker = marker;
  bs = marker;
}


// int main(void) {
//   cout << "Marker.cpp" << endl;

//   return 0;
// }
