#pragma once
#include "stdafx.h"

class VideoStitch				//读取 视频 并拼接 输出
{
public:
	bool readVideo(string dirName);						//将多个视频打开、读入第一帧到src[]中
	bool nextFrame();									//获取视频的下一帧
	VideoStitch & release();							//释放视频资源
	VideoStitch & stitchVideo(string savePath);			//拼接视频帧、输出最终的视频
	VideoStitch & stitchVideo2(string savePath);		//调用OpenCV自带的Stitcher类 函数，速度慢，效果好
//	VideoStitch & setResult(Mat mat);
private:
	Mat src[camMaxNum], result[camMaxNum];
	VideoCapture videoCapture[camMaxNum];
	StitchFrame stitchFrame[camMaxNum];
	VideoWriter outPut;
	Directory dir;
	vector<string> fileName;
};

