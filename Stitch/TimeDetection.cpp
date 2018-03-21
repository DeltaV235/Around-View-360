#include "stdafx.h"
#include "TimeDetection.h"			// 头文件名不区分大小写



TimeDetection& TimeDetection::getAvgTime()				//获取平均时间 或每次操作所需要的时间
{
	printf("\rAvgTime: %.3f ms",getSum(data) / TIME_ARR_SIZE);	//使用\r退回当前行最前端，是之后的输出覆盖前一次操作时的输出
	return *this;
}

TimeDetection& TimeDetection::getAvgFps()
{
	printf("\tAvgFPS: %.2f",1000.0/(getSum(data) / TIME_ARR_SIZE));
	return *this;
}

TimeDetection& TimeDetection::getCurTime()				//获取当前一次操作所使用的时间
{
	printf("\tCurTime: %.3f ms", data[count-1]);		//因为setEndPos()中将count++，所以这里 count-- 便代表上次操作的时间
	return *this;
}

double TimeDetection::getSum(double data[])		//计算data数组的总和
{
	sum = 0;
	i =0;
	while (i != TIME_ARR_SIZE)
	{
		sum += data[i];
		i++;
	}
	return sum;
}

TimeDetection& TimeDetection::setStartPos()			//获取一次操作开始的时间
{
	start = getTickCount();
	return *this;
}
TimeDetection& TimeDetection::setEndPos()					//获取一次操作中结束的时间，并计算该次操作所使用的时间，写入到data[]中
{
	end = getTickCount();
	if (count == TIME_ARR_SIZE)									//环形，替换之前的数据
		count = 0;
	data[count] = (end - start)/getTickFrequency() * 1000;		// *1000的作用是将时间单位转化成ms
	count++;
	return *this;
}

double TimeDetection::getCurTime(const bool overloaded)		//获取当前一次操作所使用的时间,返回值为浮点数
{
	return data[count - 1];
}
