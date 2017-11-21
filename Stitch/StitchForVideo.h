#pragma once


#include <opencv2\highgui.hpp>
#include <opencv2\calib3d.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <time.h>
#include <iostream>


//#include "timedetection.h"

#define STITCH_SURF 0
#define STITCH_SIFT 1
#define IS_REBUILD_H false

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

class StitchForVideo {
private:
	clock_t start, finish;
	Mat src_L, src_R, gray_R, gray_L, c, d, outimg, H, result;
	Ptr<SURF> surf = SURF::create(800);
	Ptr<SIFT> sift = SIFT::create(1800);
	BFMatcher matcher;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches, good_matches;
	int ptsPairs = 0;
	vector<Point2f> obj, scene;

public:

	StitchForVideo& setSRC_L(string path);					//从某个路径导入图片到Mat src_L中，导入成功则返回true
	StitchForVideo& setSRC_L(Mat image);					//将某个Mat复制到Mat src_L中
	StitchForVideo& setSRC_R(string path);					//从某个路径导入图片到Mat src_R中
	StitchForVideo& setSRC_R(Mat image);					//将某个Mat复制到Mat src_R中
	bool setH(string path);						//从某个路径导入 单应性矩阵 H
	Mat getH();									//获取当前对象的H，并返回该Mat
	Mat findH(string path, int flag,bool isRebuild=true);						//计算src_L和src_R的单应性矩阵 H，并返回H
	void show(string winName);					//显示两图的匹配点对 和 对应的区域 
	Mat stitch(int width, int flag = 0);		//水平拼接、融合对象中的两幅图片(src_L & src_R) flag==1时，打印出处理所用的总时间
	Mat stitch_v(int width, int flag = 0);		//竖直拼接、融合对象中的两幅图片(src_L & src_R) flag==1时，打印出处理所用的总时间
	Mat getResult();
	void makeVideo(string pathL,string pathR);	//两段视频的拼接
};
