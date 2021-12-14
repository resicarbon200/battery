/**
 * @file PosMarker.hpp
 * @brief マーカーの位置などを持っているクラス
 *
 * @details
 * マーカーまでの距離、マーカーの向き、マーカーのある方向を計算する
 */

#pragma once

#include <opencv2/opencv.hpp>

class PosMarker {
  const int PARAM_D = 2900;   ///< 距離計算用パラメータ

  cv::Point ul;   ///< 左上の頂点の座標
  cv::Point ur;   ///< 右上の頂点の座標
  cv::Point ll;   ///< 左下の頂点の座標
  cv::Point lr;   ///< 右下の頂点の座標
  cv::Point upper;    ///< 上の辺ベクトル
  cv::Point lower;    ///< 下の辺ベクトル
  cv::Point lefter;   ///< 左の辺ベクトル
  cv::Point righter;  ///< 右の辺ベクトル
  float llen;    ///< 左辺の長さ
  float rlen;    ///< 右辺の長さ
  float mlen;    ///< 2辺の長さの平均
  float depth;   ///< 距離 [cm]
  float angle;   ///< マーカ面の角度(左向きが負　右向きが正) [度]
  float deflec;  ///< 映像中のマーカ位置(左寄りが負　右寄りが正)(-1~+1)

  PosMarker();  ///< デフォルトコンストラクタは禁止
 public:
  /**
   * @brief マーカーの頂点の座標から距離などを計算する
   *
   * @param[in] ul 左上の頂点の座標
   * @param[in] ur 右上の頂点の座標
   * @param[in] ll 左下の頂点の座標
   * @param[in] lr 右下の頂点の座標
   */
  PosMarker(cv::Point ul, cv::Point ur, cv::Point ll, cv::Point lr);
  cv::Point getUpper();     ///< 上の辺ベクトルを取得
  cv::Point getLower();     ///< 下の辺ベクトルを取得
  cv::Point getLefter();    ///< 左の辺ベクトルを取得
  cv::Point getRighter();   ///< 右の辺ベクトルを取得
  float getLlen();      ///< 左辺の長さを取得
  float getRlen();      ///< 右辺の長さを取得
  float getMlen();      ///< 2辺の長さの平均を取得
  float getDepth();     ///< 距離 [cm]を取得
  float getAngle();     ///< マーカ面の角度(右向きが正) [度]を取得
  float getDeflec();    ///< 映像中のマーカ位置(右寄りが正)(-1~+1)を取得
};