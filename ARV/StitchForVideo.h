#pragma once


#include <opencv2\highgui.hpp>
#include <opencv2\calib3d.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\xfeatures2d.hpp>

#include "timedetection.h"


using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

class StitchForVideo {
private:
	clock_t start, finish;
	Mat src_L, src_R, gray_R, gray_L, c, d, outimg, H, result;
	Ptr<SURF> surf = SURF::create(800);
	BFMatcher matcher;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches, good_matches;
	int ptsPairs = 0;
	vector<Point2f> obj, scene;

public:

	int setSRC_L(string path);
	int setSRC_L(Mat image);
	int setSRC_R(string path);
	int setSRC_R(Mat image);
	int setH(string path);
	Mat getH();
	Mat findH(string path);
	void show(string winName);
	Mat stitch(int width, int flag = 0);
	Mat stitch_v(int width, int flag = 0);
};
