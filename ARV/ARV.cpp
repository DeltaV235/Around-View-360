#include "ARV.h"

Mat findH(Mat src_L, Mat src_R)
{
	clock_t start, finish;
	double duration;
	start = clock();
	Mat gray_R, gray_L;
	cvtColor(src_R, gray_R, COLOR_BGR2GRAY);
	cvtColor(src_L, gray_L, COLOR_BGR2GRAY);
	Ptr<SURF> surf;
	surf = SURF::create(800);
	BFMatcher matcher;
	Mat c, d;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches;
	surf->detectAndCompute(gray_R, Mat(), key1, c);
	surf->detectAndCompute(gray_L, Mat(), key2, d);
	matcher.match(c, d, matches);
	sort(matches.begin(), matches.end());
	vector< DMatch > good_matches;
	int ptsPairs = min(50, (int)(matches.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);
	}
	Mat outimg;
	drawMatches(gray_R, key1, gray_L, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	vector<Point2f> obj;
	vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(key1[good_matches[i].queryIdx].pt);
		scene.push_back(key2[good_matches[i].trainIdx].pt);
	}
	Mat H = findHomography(obj, scene, RANSAC);
	finish = clock();
	duration = finish - start;
	cout << duration <<" ms"<< endl;
	imshow("test for findH", outimg);
	return H;
}
