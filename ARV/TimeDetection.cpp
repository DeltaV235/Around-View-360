
#include "TimeDetection.h"			// ͷ�ļ��������ִ�Сд



void TimeDetection::getAvgTime()				//��ȡƽ��ʱ�� ��ÿ�β�������Ҫ��ʱ��
{
	printf("\rAvgTime: %.3f ms",getSum(data) / TIME_ARR_SIZE);	//ʹ��\r�˻ص�ǰ����ǰ�ˣ���֮����������ǰһ�β���ʱ�����
}

void TimeDetection::getAvgFps()
{
	printf("\tAvgFPS: %.2f",1000.0/(getSum(data) / TIME_ARR_SIZE));
}

void TimeDetection::getCurTime()				//��ȡ��ǰһ�β�����ʹ�õ�ʱ��
{
	printf("\tCurTime: %.3f ms", data[count-1]);		//��ΪsetEndPos()�н�count++���������� count-- ������ϴβ�����ʱ��
}

double TimeDetection::getSum(double data[])		//����data������ܺ�
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

void TimeDetection::setStartPos()			//��ȡһ�β�����ʼ��ʱ��
{
	start = getTickCount();
}
void TimeDetection::setEndPos()					//��ȡһ�β����н�����ʱ�䣬������ôβ�����ʹ�õ�ʱ�䣬д�뵽data[]��
{
	end = getTickCount();
	if (count == TIME_ARR_SIZE)									//���Σ��滻֮ǰ������
		count = 0;
	data[count] = (end - start)/getTickFrequency() * 1000;		// *1000�������ǽ�ʱ�䵥λת����ms
	count++;
}