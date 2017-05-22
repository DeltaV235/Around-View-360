//ARV_0.1.cpp

#include "ARV.h"






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
	start = clock();

	cvtColor(src_R, gray_R, COLOR_BGR2GRAY);
	cvtColor(src_C, gray_C, COLOR_BGR2GRAY);
	cvtColor(src_L, gray_L, COLOR_BGR2GRAY);

	Ptr<SURF> surf;
	surf = SURF::create(800);

	BFMatcher matcher;
	Mat c, d, e;
	vector<KeyPoint>key1, key2,key3;
	vector<DMatch> matches_CR ,matches_LC;

	
	surf->detectAndCompute(gray_R, Mat(), key1, c);
	surf->detectAndCompute(gray_C, Mat(), key2, d);
	surf->detectAndCompute(gray_L, Mat(), key3, e);

	matcher.match(c, d, matches_CR);       //匹配
	matcher.match(d, e, matches_LC);

	sort(matches_CR.begin(), matches_CR.end());  //筛选匹配点
	sort(matches_LC .begin(), matches_LC.end());


	vector< DMatch > good_matches_CR, good_matches_LC;


	int ptsPairs = min(50, (int)(matches_CR.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches_CR.push_back(matches_CR[i]);
	}


	ptsPairs = min(50, (int)(matches_LC.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches_LC.push_back(matches_LC[i]);
	}




	Mat outimg_CR;
	drawMatches(gray_R, key1, gray_C, key2, good_matches_CR, outimg_CR, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点

	Mat outimg_LC;
	drawMatches(gray_C, key2, gray_L, key3, good_matches_LC, outimg_LC, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点



	vector<Point2f> obj;
	vector<Point2f> scene;

	for (size_t i = 0; i < good_matches_CR.size(); i++)
	{
		obj.push_back(key1[good_matches_CR[i].queryIdx].pt);
		scene.push_back(key2[good_matches_CR[i].trainIdx].pt);
	}
	
	

	Mat H_CR = findHomography(obj, scene, RANSAC);      //寻找匹配的图像

	obj.clear();
	scene.clear();
	for (size_t i = 0; i < good_matches_LC.size(); i++)
	{
		obj.push_back(key2[good_matches_LC[i].queryIdx].pt);
		scene.push_back(key3[good_matches_LC[i].trainIdx].pt);
	}
	Mat H_LC = findHomography(obj, scene, RANSAC);
	finish = clock();
	duration = (double)(finish - start);
	printf("%f ms\n", duration);
	findH(src_L, src_C);
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
	//imshow("SRC_CR", outimg_CR);
	imshow("SRC_LC", outimg_LC);
	/*cout << "H=  (PYTHON)" << endl << format(H, Formatter::FMT_PYTHON) << endl << endl;*/

	

	//拼接


	int width = 30;//渐入渐出融合宽度
	Mat result,result_LC;
	/*while (1) {*/
		
		//cols　列 rows 行
		warpPerspective(src_R, result,H_CR, Size(3 * src_R.cols - width, src_R.rows));//Size设置结果图像宽度，宽度裁去一部分，e可调
		//imshow("result", result);
		Mat half(result, Rect(0, 0, src_C.cols - width, src_C.rows));
		//imshow("half", half);
		src_C(Range::all(), Range(0, src_C.cols - width)).copyTo(half);
		//imshow("half2", half);
		for (int i = 0; i < width; i++)
		{
			result.col(src_C.cols - width + i) = (width - i) / (float)width*src_C.col(src_C.cols - width + i) + i / (float)width*result.col(src_C.cols - width + i);  //权重
		}




		warpPerspective(result, result_LC, H_LC, Size(4 * src_L.cols - width, src_L.rows));//Size设置结果图像宽度，宽度裁去一部分，e可调
		//imshow("result_LC", result_LC);
		Mat half_LC(result_LC, Rect(0, 0, src_L.cols - width, src_L.rows));
		//imshow("half-LC", half_LC);
		src_L(Range::all(), Range(0, src_L.cols - width)).copyTo(half_LC);
		//imshow("half2_LC", half_LC);
		for (int i = 0; i < width; i++)
		{
			result_LC.col(src_L.cols - width + i) = (width - i) / (float)width*src_L.col(src_L.cols - width + i) + i / (float)width*result_LC.col(src_L.cols - width + i);  //权重
		}


		//imshow("stitch", result);
		imshow("stitch", result_LC);
		
		
		waitKey(20);
	/*}*/



	cvWaitKey(0);
}