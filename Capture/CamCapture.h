#pragma once

#include "stdafx.h"

#define camMaxNum 6

class CamCapture
{

private:
	//VideoCapture videoCapture(0);
public:
	//CamCapture& videoSave();
	CamCapture & capture(int camSeq, int width, int heigth, double fps, string savePath);		//单摄像头采集视频 savePath为保存的路径
	CamCapture & capture(int  camNum, int width, int heigth, double fps, char saveDirName[]);	//多摄像头同时采集视频 saveDirPath为保存的文件夹路径
};
