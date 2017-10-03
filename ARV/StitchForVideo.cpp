#include "StitchForVideo.h"


	StitchForVideo& StitchForVideo::setSRC_L(string path)
	{
		src_L = imread(path, 1);
		return *this;
	}


	StitchForVideo& StitchForVideo::setSRC_L(Mat image)
	{
		src_L = image;
		return *this;
	}


	StitchForVideo& StitchForVideo::setSRC_R(string path)
	{
		src_R = imread(path, 1);
		return *this;
	}


	StitchForVideo& StitchForVideo::setSRC_R(Mat image)
	{
		src_R = image;
		return *this;
	}


	bool StitchForVideo::setH(string path)
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


	Mat StitchForVideo::findH(string path,int flag,bool isRebuild)
	{
		if (setH(path) == true && isRebuild==false) {
			return H;
		}
		else {
			start = clock();		//��ʱ
									//��ԭͼת���ɻҶ�ͼ
			cvtColor(src_R, gray_R, COLOR_BGR2GRAY);
			cvtColor(src_L, gray_L, COLOR_BGR2GRAY);
			if (flag == 0) {
				surf->detectAndCompute(gray_R, Mat(), key1, c);
				surf->detectAndCompute(gray_L, Mat(), key2, d);

			}
			else if (flag == 1)
			{
				sift->detectAndCompute(gray_R, Mat(), key1, c);
				sift->detectAndCompute(gray_L, Mat(), key2, d);
			}

			//ƥ��
			matcher.match(c, d, matches);
			//ɸѡƥ���
			sort(matches.begin(), matches.end());

			ptsPairs = min(150, (int)(matches.size() * 0.15));
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


	Mat StitchForVideo::stitch(int width, int flag)
	{
		start = clock();

		//cols���� rows ��
		//Size���ý��ͼ���ȣ����Ȳ�ȥһ���֣�width�ɵ�
		
		/*

		����һ������Ϊ1.5src_R.cols-width,���Ϊsrc_L/src_R.rows��Mat����Ϊ����src_L.rows==src_R.rows����src_R.colsǰ��ϵ��1.5������
		ͼƬ���ص��������С�йأ��ص�����Խ�󣬸�ϵ��ѧУ����֮��Ȼ
		
		����֮ǰ����ĵ�Ӧ�Ծ��󣬽�src_R����result��

		width��Ը��Ϊ����ϵ��Ϊ2ʱ����ͼ���ص������ص�������Ҳ����ָ��ͼ����Ҫ�ںϵĳ��ȣ�������
		
		*/
		warpPerspective(src_R, result, H, Size(1.5 * src_R.cols - width, src_R.rows));
		//half��resultָ��ͬһƬ�ڴ棬��СΪsrc_Lȥ���ص�����������ĳ��� * src_L�Ŀ��
		
		Mat half(result, Rect(0, 0, src_L.cols - width, src_L.rows));

		//��src_L��Mat ���Ƶ�half ��������result����Ӧ�����ͼ��
		src_L(Range::all(), Range(0, src_L.cols - width)).copyTo(half);
		for (int i = 0; i < width; i++)
		{
			result.col(src_L.cols - width + i) = (width - i) / (float)width*src_L.col(src_L.cols - width + i) + i / (float)width*result.col(src_L.cols - width + i); 
			//Ȩ��	�ص������е����ص�λ��Խ����src_L,src_Lԭͼ�е�����ռ��Ȩ��Խ��src_R�е����ص�Ȩ��ԽС����֮��Ȼ
			//��Ȩ����Ӻ��õ��ںϺ�����ص�
		}
		finish = clock();
		if (flag)		//flag==1ʱ����ӡ���������õ���ʱ��
		{
			printf("stitch: %.2f ms\n", (double)(finish - start));
		}
		return result;
	}


	Mat StitchForVideo::stitch_v(int width, int flag)
	{
		start = clock();

		//cols���� rows ��
		//Size���ý��ͼ���ȣ���Ȳ�ȥһ���֣�width�ɵ����������� ����stitch(...);
		warpPerspective(src_R, result, H, Size(src_R.cols, 1.5*src_R.rows - width));
		
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

	Mat StitchForVideo::getResult()
	{
		return result;
	}
