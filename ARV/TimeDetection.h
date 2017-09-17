#pragma once


#include <time.h>
#include <iostream>
#include <opencv2\core\cvstd.hpp>
#include <opencv2\core\utility.hpp>


using namespace cv;


#define SHOW_TIME 1
#define TIME_ARR_SIZE 5


class TimeDetection {
private:
	double start, end, data[TIME_ARR_SIZE] = { 0 }, sum;
	int count=0, i;
public:
	void setStartPos();
	void setEndPos();
	void getAvgTime();
	void getAvgFps();
	void getCurTime();
	double getSum(double data[]);
};
