/**
* @file Marker.hpp
* @brief カメラでマーカーを認識するためのクラス
*
* @details
* setMarker関数で設定したマーカーの位置を，processing関数でPosMarkerクラスとして得る．
*/

#pragma once

#include <bitset>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "PosMarker.hpp"

class Marker {
  const float ACCURACY = 0.8; ///< マーカーの色一致率(0~1) 大きいほど誤りに厳しい

  cv::VideoCapture cap; ///< カメラインスタンス
  unsigned int marker;  ///< マーカーの模様
  std::bitset<16> bs;   ///< 16bit型マーカーの模様
public:
  Marker();   ///< コンストラクタ
  ~Marker();  ///< デストラクタ

  /**
  * @brief マーカーの模様を16進数4桁で設定する．
  * 例）setMarker(0x9a1e)
  *
  * @param[in] marker マーカーの模様を16進数4桁で表したもの
  */
  void setMarker(unsigned int marker);
  /**
   * @brief マーカーの位置を計算し，PosMarkerに格納し，そのスマートポインタを返す
   * 
   * @return std::unique_ptr<PosMarker>
   * マーカーの位置を示すPosMarkerクラスのスマートポインタ．所有権をmoveする必要あり．
  */
  std::unique_ptr<PosMarker> processing();
};
