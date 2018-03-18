#pragma once

#include "stdafx.h"

using namespace cv;

#define SHOW_TIME 1
#define TIME_ARR_SIZE 10


class TimeDetection {												//���ն�����ʾʱ����ص�ֵ��������ʹ�õ�ʱ�䡢��Ƶƴ�ӵ�֡�ʵȵ�
private:
	double start, end, data[TIME_ARR_SIZE] = { 0 }, sum;
	int count=0, i;
public:
	TimeDetection& setStartPos();									//���ÿ�ʼ��ʱ��λ��
	TimeDetection& setEndPos();										//���ý�����ʱ��λ��
	TimeDetection& getAvgTime();									//���� TIME_ARR_SIZE �μ�ʱ��ƽ��ֵ
	TimeDetection& getAvgFps();										//���ƽ��֡��
	TimeDetection& getCurTime();									//��õ�ǰ�������õ�ʱ��
	double getSum(double data[]);									//���� TIME_ARR_SIZE ��Ԫ�� �� ����data[] �� ��
};
