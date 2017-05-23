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

class StitchForVideo {
private:
	clock_t start, finish;
	Mat src_L, src_R, gray_R, gray_L, c, d, outimg, H;
	Ptr<SURF> surf = SURF::create(800);
	BFMatcher matcher;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches, good_matches;
	int ptsPairs = 0;
	vector<Point2f> obj, scene;

public:
	int setSRC_L(string path)
	{
		src_L = imread(path, 1);
		return true;
	}

	int setSRC_R(string path)
	{
		src_R = imread(path, 1);
		return true;
	}

	Mat findH(string path)
	{
		start = clock();
		//将原图转化成灰度图
		cvtColor(src_R, gray_R, COLOR_BGR2GRAY);
		cvtColor(src_L, gray_L, COLOR_BGR2GRAY);
		surf->detectAndCompute(gray_R, Mat(), key1, c);
		surf->detectAndCompute(gray_L, Mat(), key2, d);
		//匹配
		matcher.match(c, d, matches);
		//筛选匹配点
		sort(matches.begin(), matches.end());

		ptsPairs = min(50, (int)(matches.size() * 0.15));
		cout << ptsPairs << endl;
		for (int i = 0; i < ptsPairs; i++)
		{
			good_matches.push_back(matches[i]);
		}
		//绘制匹配点
		drawMatches(gray_R, key1, gray_L, key2, good_matches, outimg, 
			Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		for (size_t i = 0; i < good_matches.size(); i++)
		{
			obj.push_back(key1[good_matches[i].queryIdx].pt);
			scene.push_back(key2[good_matches[i].trainIdx].pt);
		}
		//寻找匹配的图像
		H = findHomography(obj, scene, RANSAC);
		finish = clock();
		cout << "findH: " << (double)(finish - start) << " ms" << endl;


		imwrite(path, H);
		return H;
	}

	void show()
	{
		vector<Point2f> obj_corners(4);
		obj_corners[0] = Point(0, 0);
		obj_corners[1] = Point(gray_R.cols, 0);
		obj_corners[2] = Point(gray_R.cols, gray_R.rows);
		obj_corners[3] = Point(0, gray_R.rows);
		vector<Point2f> scene_corners(4);

		perspectiveTransform(obj_corners, scene_corners, H);

		line(outimg, scene_corners[0] + Point2f((float)gray_R.cols, 0), scene_corners[1] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);       //绘制
		line(outimg, scene_corners[1] + Point2f((float)gray_R.cols, 0), scene_corners[2] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
		line(outimg, scene_corners[2] + Point2f((float)gray_R.cols, 0), scene_corners[3] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
		line(outimg, scene_corners[3] + Point2f((float)gray_R.cols, 0), scene_corners[0] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
		imshow("test for findH", outimg);

	}

	Mat stitch(int width)
	{
		clock_t start, finish;
		start = clock();
		Mat result;
		//cols　列 rows 行
		warpPerspective(src_R, result, H, Size(2 * src_R.cols - width, src_R.rows));//Size设置结果图像宽度，宽度裁去一部分，e可调

		Mat half(result, Rect(0, 0, src_L.cols - width, src_L.rows));
		src_L(Range::all(), Range(0, src_L.cols - width)).copyTo(half);
		for (int i = 0; i < width; i++)
		{
			result.col(src_L.cols - width + i) = (width - i) / (float)width*src_L.col(src_L.cols - width + i) + i / (float)width*result.col(src_L.cols - width + i);  //权重
		}
		finish = clock();
		printf("stitch: %.2f ms\n", (double)(finish - start));
		return result;
	}
};
