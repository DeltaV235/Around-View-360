#pragma once

#include "stdafx.h"

using namespace cv;

#define SHOW_TIME 1
#define TIME_ARR_SIZE 10


class TimeDetection {												//在终端中显示时间相关的值，如程序段使用的时间、视频拼接的帧率等等
private:
	double start, end, data[TIME_ARR_SIZE] = { 0 }, sum;
	int count=0, i;
public:
	TimeDetection& setStartPos();									//设置开始计时的位置
	TimeDetection& setEndPos();										//设置结束计时的位置
	TimeDetection& getAvgTime();									//计算 TIME_ARR_SIZE 次计时的平均值
	TimeDetection& getAvgFps();										//获得平均帧率
	TimeDetection& getCurTime();									//获得当前处理所用的时间
	double getSum(double data[]);									//计算 TIME_ARR_SIZE 个元素 的 数组data[] 的 和
};
