#pragma once

#include "stdafx.h"

#define camMaxNum 6

class CamCapture
{

private:
	//VideoCapture videoCapture(0);
public:
	//CamCapture& videoSave();
	CamCapture & capture(int camSeq, int width, int heigth, double fps, string savePath);		//������ͷ�ɼ���Ƶ savePathΪ�����·��
	CamCapture & capture(int  camNum, int width, int heigth, double fps, char saveDirName[]);	//������ͷͬʱ�ɼ���Ƶ saveDirPathΪ������ļ���·��
};
