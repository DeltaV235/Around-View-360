#include "stdafx.h"
#include "CamCapture.h"


 CamCapture & CamCapture::capture(int camNum, int Width, int Heigth)
{	
	VideoCapture capture(camNum);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, Width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, Heigth);
	//capture.
	return *this;
	// TODO: 在此处插入 return 语句
}