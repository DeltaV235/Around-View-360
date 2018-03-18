#pragma once
#include "stdafx.h"

class VideoStitch				//��ȡ ��Ƶ ��ƴ�� ���
{
public:
	bool readVideo(string dirName);						//�������Ƶ�򿪡������һ֡��src[]��
	bool nextFrame();									//��ȡ��Ƶ����һ֡
	VideoStitch & release();							//�ͷ���Ƶ��Դ
	VideoStitch & stitchVideo(string savePath);			//ƴ����Ƶ֡��������յ���Ƶ
	VideoStitch & stitchVideo2(string savePath);		//����OpenCV�Դ���Stitcher�� �������ٶ�����Ч����
//	VideoStitch & setResult(Mat mat);
private:
	Mat src[camMaxNum], result[camMaxNum];
	VideoCapture videoCapture[camMaxNum];
	StitchFrame stitchFrame[camMaxNum];
	VideoWriter outPut;
	Directory dir;
	vector<string> fileName;
};

