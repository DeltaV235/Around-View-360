#include "StitchForVideo.h"


	int StitchForVideo::setSRC_L(string path)
	{
		src_L = imread(path, 1);
		return true;
	}
	int StitchForVideo::setSRC_L(Mat image)
	{
		src_L = image;
		return true;
	}

	int StitchForVideo::setSRC_R(string path)
	{
		src_R = imread(path, 1);
		return true;
	}
	int StitchForVideo::setSRC_R(Mat image)
	{
		src_R = image;
		return true;
	}
	int StitchForVideo::setH(string path)
	{
		FileStorage fs;
		fs.open(path, FileStorage::READ);
		fs["H_MAT_DATA"] >> H;
		bool flag = fs.isOpened();
		fs.release();
		return flag;
	}
	Mat StitchForVideo::getH()
	{
		return H;
	}

	Mat StitchForVideo::findH(string path)
	{
		start = clock();
		//��ԭͼת���ɻҶ�ͼ
		cvtColor(src_R, gray_R, COLOR_BGR2GRAY);
		cvtColor(src_L, gray_L, COLOR_BGR2GRAY);
		surf->detectAndCompute(gray_R, Mat(), key1, c);
		surf->detectAndCompute(gray_L, Mat(), key2, d);
		//ƥ��
		matcher.match(c, d, matches);
		//ɸѡƥ���
		sort(matches.begin(), matches.end());

		ptsPairs = min(50, (int)(matches.size() * 0.15));
		cout << ptsPairs << endl;
		for (int i = 0; i < ptsPairs; i++)
		{
			good_matches.push_back(matches[i]);
		}
		//����ƥ���
		drawMatches(gray_R, key1, gray_L, key2, good_matches, outimg,
			Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		for (size_t i = 0; i < good_matches.size(); i++)
		{
			obj.push_back(key1[good_matches[i].queryIdx].pt);
			scene.push_back(key2[good_matches[i].trainIdx].pt);
		}
		//Ѱ��ƥ���ͼ��
		H = findHomography(obj, scene, RANSAC);
		finish = clock();
		cout << "findH: " << (double)(finish - start) << " ms" << endl;

		FileStorage fs(path, FileStorage::WRITE);
		fs << "H_MAT_DATA" << H;
		fs.release();

		return H;
	}

	void StitchForVideo::show(string winName)
	{
		vector<Point2f> obj_corners(4);
		obj_corners[0] = Point(0, 0);
		obj_corners[1] = Point(gray_R.cols, 0);
		obj_corners[2] = Point(gray_R.cols, gray_R.rows);
		obj_corners[3] = Point(0, gray_R.rows);
		vector<Point2f> scene_corners(4);

		perspectiveTransform(obj_corners, scene_corners, H);

		line(outimg, scene_corners[0] + Point2f((float)gray_R.cols, 0), scene_corners[1] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);       //����
		line(outimg, scene_corners[1] + Point2f((float)gray_R.cols, 0), scene_corners[2] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
		line(outimg, scene_corners[2] + Point2f((float)gray_R.cols, 0), scene_corners[3] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
		line(outimg, scene_corners[3] + Point2f((float)gray_R.cols, 0), scene_corners[0] + Point2f((float)gray_R.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
		imshow(winName, outimg);

	}

	Mat StitchForVideo::stitch(int width, int flag )
	{
		start = clock();

		//cols���� rows ��
		warpPerspective(src_R, result, H, Size(1.5 * src_R.cols - width, src_R.rows));//Size���ý��ͼ���ȣ���Ȳ�ȥһ���֣�e�ɵ�

		Mat half(result, Rect(0, 0, src_L.cols - width, src_L.rows));
		src_L(Range::all(), Range(0, src_L.cols - width)).copyTo(half);
		for (int i = 0; i < width; i++)
		{
			result.col(src_L.cols - width + i) = (width - i) / (float)width*src_L.col(src_L.cols - width + i) + i / (float)width*result.col(src_L.cols - width + i);  //Ȩ��
		}
		finish = clock();
		if (flag)
		{
			printf("stitch: %.2f ms\n", (double)(finish - start));
		}
		return result;
	}

	Mat StitchForVideo::stitch_v(int width, int flag )
	{
		start = clock();

		//cols���� rows ��
		warpPerspective(src_R, result, H, Size(src_R.cols, 1.5*src_R.rows - width));//Size���ý��ͼ���ȣ���Ȳ�ȥһ���֣�e�ɵ�

		Mat half(result, Rect(0, 0, src_L.cols, src_L.rows - width));
		src_L(Range(0, src_L.rows - width), Range::all()).copyTo(half);
		for (int i = 0; i < width; i++)
		{
			result.row(src_L.rows - width + i) = (width - i) / (float)width*src_L.row(src_L.rows - width + i) + i / (float)width*result.row(src_L.rows - width + i);  //Ȩ��
		}
		finish = clock();
		if (flag)
		{
			printf("stitch: %.2f ms\n", (double)(finish - start));
		}
		return result;
	}
