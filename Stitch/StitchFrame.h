#pragma once

#include "stdafx.h"


//#include "timedetection.h"

#define STITCH_SURF 0
#define STITCH_SIFT 1
#define STITCH_SURF_FLANN 2
#define IS_REBUILD_H false

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

class StitchFrame {													//��������ƴ������ͼƬ(��������㡢���������㡢ƥ�������㡢Ѱ��ӳ����󡢷���任��ƴ�Ӵ��ں�)
private:
	clock_t start, finish;
	Mat src_L, src_R, gray_R, gray_L, outimg, H, result;

public:

	StitchFrame& setSRC_L(string path);								//��ĳ��·������ͼƬ��Mat src_L�У�����ɹ��򷵻�true
	StitchFrame& setSRC_L(Mat image);								//��ĳ��Mat���Ƶ�Mat src_L��
	StitchFrame& setSRC_R(string path);								//��ĳ��·������ͼƬ��Mat src_R��
	StitchFrame& setSRC_R(Mat image);								//��ĳ��Mat���Ƶ�Mat src_R��
	bool setH(string path);											//��ĳ��·������ ��Ӧ�Ծ��� H
	Mat getH();														//��ȡ��ǰ�����H�������ظ�Mat
	Mat findH(string path, int flag, bool isRebuild = true);		//����src_L��src_R�ĵ�Ӧ�Ծ��� H��������H
	void show(string winName);										//��ʾ��ͼ��ƥ���� �� ��Ӧ������ 
	Mat stitch(int width, int flag = 0);							//ˮƽƴ�ӡ��ں϶����е�����ͼƬ(src_L & src_R) flag==1ʱ����ӡ���������õ���ʱ��
	Mat stitch_v(int width, int flag = 0);							//��ֱƴ�ӡ��ں϶����е�����ͼƬ(src_L & src_R) flag==1ʱ����ӡ���������õ���ʱ��
	Mat getResult();
	void makeVideo(string pathL, string pathR);						//������Ƶ��ƴ��
};