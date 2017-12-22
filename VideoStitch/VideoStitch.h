#pragma once
#include "stdafx.h"

class VideoStitch
{
public:
	VideoStitch();
	~VideoStitch();
	VideoStitch & readVideo(char dirName[],int videoNum);
	VideoStitch & stitchVideo(char dirName[]);
	VideoStitch & setResult(Mat mat);
private:
	Mat src[camMaxNum], result[1];
};

VideoStitch::VideoStitch()
{
}

VideoStitch::~VideoStitch()
{
}