#pragma once

typedef struct {
	int llen;		//左辺長さ
	int rlen;		//右辺長さ
	int angle;	//マーカ面の角度(左向きが負　右向きが正)
	int deflec;	//映像中のマーカ位置(左寄りが負　右寄りが正)
} markerpos;