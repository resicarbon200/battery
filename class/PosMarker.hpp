#pragma once

class PosMarker {
  int llen;		//左辺長さ
  int rlen;		//右辺長さ
  int mlen;		//2辺の長さの平均
  int depth;	//距離
  int angle;	//マーカ面の角度(左向きが負　右向きが正)
  int deflec;	//映像中のマーカ位置(左寄りが負　右寄りが正)
  
  PosMarker();  //デフォルトコンストラクタの禁止
public:
  PosMarker(int, int, int);
  int getLlen();
  int getRlen();
  int getMlen();
  int getDepth();
  int getAngle();
  int getDeflec();
};