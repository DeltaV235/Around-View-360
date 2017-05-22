//ARV_0.1.cpp

#include "ARV.h"



using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;


int main()
{
	clock_t start, finish;
	double duration;


	cout << "CV_VERSION: " << CV_VERSION << endl;

	//读取原图

	Mat src_R = imread("R.jpg", 1);
	Mat src_C = imread("C.jpg", 1);
	Mat src_L = imread("L.jpg", 1);
	Mat gray_R, gray_C,gray_L;

	//将原图转化成灰度图

	cvtColor(src_R, gray_R, COLOR_BGR2GRAY);
	cvtColor(src_C, gray_C, COLOR_BGR2GRAY);
	cvtColor(src_L, gray_L, COLOR_BGR2GRAY);

	Ptr<SURF> surf;
	surf = SURF::create(800);

	BFMatcher matcher;
	Mat c, d;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches;

	surf->detectAndCompute(gray_R, Mat(), key1, c);
	surf->detectAndCompute(gray_C, Mat(), key2, d);

	matcher.match(c, d, matches);       //匹配

	sort(matches.begin(), matches.end());  //筛选匹配点
	vector< DMatch > good_matches;
	int ptsPairs = min(50, (int)(matches.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);
	}
	Mat outimg;
	drawMatches(gray_R, key1, gray_C, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点

	vector<Point2f> obj;
	vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(key1[good_matches[i].queryIdx].pt);
		scene.push_back(key2[good_matches[i].trainIdx].pt);
	}

	

	Mat H = findHomography(obj, scene, RANSAC);      //寻找匹配的图像


	//vector<Point2f> obj_corners(4);
	//obj_corners[0] = Point(0, 0);
	//obj_corners[1] = Point(gray_R.cols, 0);
	//obj_corners[2] = Point(gray_R.cols, gray_R.rows);
	//obj_corners[3] = Point(0, gray_R.rows);
	//vector<Point2f> scene_corners(4);

	//perspectiveTransform(obj_corners, scene_corners, H);

	//line(outimg, scene_corners[0] + Point2f((float)gray_R.cols, 0), scene_corners[1] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);       //绘制
	//line(outimg, scene_corners[1] + Point2f((float)gray_R.cols, 0), scene_corners[2] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	//line(outimg, scene_corners[2] + Point2f((float)gray_R.cols, 0), scene_corners[3] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	//line(outimg, scene_corners[3] + Point2f((float)gray_R.cols, 0), scene_corners[0] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);


	/*namedWindow("H", WINDOW_NORMAL);
	imshow("H", H);*/
	imshow("SRC", outimg);
	/*cout << "H=  (PYTHON)" << endl << format(H, Formatter::FMT_PYTHON) << endl << endl;*/

	

	//拼接


	int e = 30;//渐入渐出融合宽度
	Mat result;
	/*while (1) {*/
		start = clock();
		//cols　列 rows 行
		warpPerspective(src_R, result, H, Size(2 * src_R.cols - e, src_R.rows));//Size设置结果图像宽度，宽度裁去一部分，e可调
		imshow("result", result);
		Mat half(result, Rect(0, 0, src_C.cols - e, src_C.rows));
		imshow("half", half);
		src_C(Range::all(), Range(0, src_C.cols - e)).copyTo(half);
		imshow("half2", half);
		for (int i = 0; i < e; i++)
		{
			result.col(src_C.cols - e + i) = (e - i) / (float)e*src_C.col(src_C.cols - e + i) + i / (float)e*result.col(src_C.cols - e + i);  //权重
		}

		imshow("stitch", result);

		finish = clock();
		duration = (double)(finish - start);

		printf("%f ms\n", duration);
		waitKey(20);
	/*}*/



	cvWaitKey(0);
}