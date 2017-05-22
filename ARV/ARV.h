#pragma once
#include "iostream"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "time.h"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

Mat findH(Mat src_L, Mat src_R);