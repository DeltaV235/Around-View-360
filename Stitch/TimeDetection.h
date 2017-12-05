#pragma once

#include "stdafx.h"

using namespace cv;

#define SHOW_TIME 1
#define TIME_ARR_SIZE 10


class TimeDetection {
private:
	double start, end, data[TIME_ARR_SIZE] = { 0 }, sum;
	int count=0, i;
public:
	TimeDetection& setStartPos();
	TimeDetection& setEndPos();
	TimeDetection& getAvgTime();
	TimeDetection& getAvgFps();
	TimeDetection& getCurTime();
	double getSum(double data[]);
};
