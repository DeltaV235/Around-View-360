#pragma once

#include "stdafx.h"


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/stitching.hpp>
#include <iostream>
#include <vector>  


#define camMaxNum 6

class CamCapture
{

private:
	//VideoCapture videoCapture(0);
public:
	//CamCapture& videoSave();
	CamCapture & capture(int camSeq, int width, int heigth, double fps, string savePath);		//������ͷ�ɼ���Ƶ savePathΪ�����·��
	CamCapture & capture(int camNum, int width, int heigth, double fps, char saveDirName[]);	//������ͷͬʱ�ɼ���Ƶ saveDirPathΪ������ļ���·��
};

void stitch(vector<Mat> imgs, Mat& resultMat);
void stitch2(Mat& srcImage1, Mat& srcImage2, Mat& panorama);
