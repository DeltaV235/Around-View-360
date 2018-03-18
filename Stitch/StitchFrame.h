#pragma once

#include "stdafx.h"


//#include "timedetection.h"

#define STITCH_SURF 0
#define STITCH_SIFT 1
#define STITCH_SURF_FLANN 2
#define IS_REBUILD_H false

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

class StitchFrame {													//此类用来拼接两张图片(检测特征点、描述特征点、匹配特征点、寻找映射矩阵、仿射变换、拼接处融合)
private:
	clock_t start, finish;
	Mat src_L, src_R, gray_R, gray_L, outimg, H, result;

public:

	StitchFrame& setSRC_L(string path);								//从某个路径导入图片到Mat src_L中，导入成功则返回true
	StitchFrame& setSRC_L(Mat image);								//将某个Mat复制到Mat src_L中
	StitchFrame& setSRC_R(string path);								//从某个路径导入图片到Mat src_R中
	StitchFrame& setSRC_R(Mat image);								//将某个Mat复制到Mat src_R中
	bool setH(string path);											//从某个路径导入 单应性矩阵 H
	Mat getH();														//获取当前对象的H，并返回该Mat
	Mat findH(string path, int flag, bool isRebuild = true);		//计算src_L和src_R的单应性矩阵 H，并返回H
	void show(string winName);										//显示两图的匹配点对 和 对应的区域 
	Mat stitch(int width, int flag = 0);							//水平拼接、融合对象中的两幅图片(src_L & src_R) flag==1时，打印出处理所用的总时间
	Mat stitch_v(int width, int flag = 0);							//竖直拼接、融合对象中的两幅图片(src_L & src_R) flag==1时，打印出处理所用的总时间
	Mat getResult();
	void makeVideo(string pathL, string pathR);						//两段视频的拼接
};