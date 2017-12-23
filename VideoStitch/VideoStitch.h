#pragma once
#include "stdafx.h"

class VideoStitch
{
public:
	
	bool readVideo(string dirName,int videoNum);
	VideoStitch & nextFrame();
	VideoStitch & release(int videoSeq);
	VideoStitch & stitchVideo(char dirName[]);
	VideoStitch & setResult(Mat mat);
private:
	Mat src[camMaxNum], result[camMaxNum];
	VideoCapture videoCapture[camMaxNum];
	StitchFrame stitchFrame[camMaxNum];
	VideoWriter outPut;
	Directory dir;
	vector<string> fileName;
};

