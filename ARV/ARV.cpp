#include "ARV.h"

void Time::getAvgTime()
{
	printf("\rAvgTime: %.3f ms",getSum(data) / TIME_ARR_SIZE);
}

void Time::getAvgFps()
{
}

void Time::getCurTime()
{
	printf("\tCurTime: %.3f ms", data[count-1]);
}

double Time::getSum(double data[])
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

void Time::setStartPos()
{
	start = getTickCount();
}
void Time::setEndPos()
{
	end = getTickCount();
	if (count == TIME_ARR_SIZE)
		count = 0;
	data[count] = (end - start)/getTickFrequency() * 1000;
	count++;
}