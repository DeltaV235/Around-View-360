#pragma once

#include "stdafx.h"


//#include "timedetection.h"

#define STITCH_SURF 0
#define STITCH_SIFT 1
#define IS_REBUILD_H false

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

class StitchFrame {
private:
	clock_t start, finish;
	Mat src_L, src_R, gray_R, gray_L, c, d, outimg, H, result;
	Ptr<SURF> surf = SURF::create(800);
	Ptr<SIFT> sift = SIFT::create(1800);
	BFMatcher matcher;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches, good_matches;
	int ptsPairs = 0;
	vector<Point2f> obj, scene;

public:

	StitchFrame& setSRC_L(string path);					//��ĳ��·������ͼƬ��Mat src_L�У�����ɹ��򷵻�true
	StitchFrame& setSRC_L(Mat image);					//��ĳ��Mat���Ƶ�Mat src_L��
	StitchFrame& setSRC_R(string path);					//��ĳ��·������ͼƬ��Mat src_R��
	StitchFrame& setSRC_R(Mat image);					//��ĳ��Mat���Ƶ�Mat src_R��
	bool setH(string path);						//��ĳ��·������ ��Ӧ�Ծ��� H
	Mat getH();									//��ȡ��ǰ�����H�������ظ�Mat
	Mat findH(string path, int flag,bool isRebuild=true);						//����src_L��src_R�ĵ�Ӧ�Ծ��� H��������H
	void show(string winName);					//��ʾ��ͼ��ƥ���� �� ��Ӧ������ 
	Mat stitch(int width, int flag = 0);		//ˮƽƴ�ӡ��ں϶����е�����ͼƬ(src_L & src_R) flag==1ʱ����ӡ���������õ���ʱ��
	Mat stitch_v(int width, int flag = 0);		//��ֱƴ�ӡ��ں϶����е�����ͼƬ(src_L & src_R) flag==1ʱ����ӡ���������õ���ʱ��
	Mat getResult();
	void makeVideo(string pathL,string pathR);	//������Ƶ��ƴ��
};