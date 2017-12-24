#include "stdafx.h"
#include "StitchFrame.h"


	StitchFrame& StitchFrame::setSRC_L(string path)
	{
		src_L = imread(path, 1);
		return *this;
	}


	StitchFrame& StitchFrame::setSRC_L(Mat image)
	{
		src_L = image;
		return *this;
	}


	StitchFrame& StitchFrame::setSRC_R(string path)
	{
		src_R = imread(path, 1);
		return *this;
	}


	StitchFrame& StitchFrame::setSRC_R(Mat image)
	{
		src_R = image;
		return *this;
	}


	bool StitchFrame::setH(string path)
	{
		FileStorage fs;
		fs.open(path, FileStorage::READ);
		fs["H_MAT_DATA"] >> H;
		bool flag = fs.isOpened();
		fs.release();
		return flag;
	}


	Mat StitchFrame::getH()
	{
		return H;
	}


	Mat StitchFrame::findH(string path,int flag,bool isRebuild)
	{
		vector<DMatch> matches, good_matches;
		vector<Point2f> obj, scene;
		vector<KeyPoint>key1, key2;
		BFMatcher matcher;
		int ptsPairs = 0;
		Ptr<SURF> surf = SURF::create(800);
		Ptr<SIFT> sift = SIFT::create(1800);

		if (setH(path) == true && isRebuild==false) {
			return H;
		}
		else {
			start = clock();		//计时
									//将原图转化成灰度图
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

			//匹配
			matcher.match(c, d, matches);
			//筛选匹配点/特征点
			sort(matches.begin(), matches.end());

			ptsPairs = min(60, (int)(matches.size() * 0.15));
			cout << ptsPairs << endl;
			for (int i = 0; i < ptsPairs; i++)
			{
				good_matches.push_back(matches[i]);
			}
			//绘制匹配点/特征点
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
			cout << "findH: " << (double)(finish - start) << " ms" << endl;			//输出所使用的时间

			//输出计算出的H到XML文件中
			FileStorage fs(path, FileStorage::WRITE);
			fs << "H_MAT_DATA" << H;
			fs.release();

			return H;
		}
		
	}


	void StitchFrame::show(string winName)
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
		imshow(winName, outimg);

	}


	Mat StitchFrame::stitch(int width, int flag)
	{
		start = clock();

		//cols　列 rows 行
		//Size设置结果图像宽度，长度裁去一部分，width可调
		
		/*

		生成一个长度为1.5src_R.cols-width,宽度为src_L/src_R.rows的Mat（因为假设src_L.rows==src_R.rows），src_R.cols前的系数1.5与两幅
		图片相重叠的区域大小有关，重叠区域越大，该系数学校，反之亦然
		
		基于之前计算的单应性矩阵，将src_R放入result中

		width的愿意为：当系数为2时，两图中重叠的像素的列数，也可以指两图中需要融合的长度（列数）
		
		*/
		warpPerspective(src_R, result, H, Size(1.5 * src_R.cols - width, src_R.rows));
		//half与result指向同一片内存，大小为src_L去掉重叠区域列数后的长度 * src_L的宽度
		
		Mat half(result, Rect(0, 0, src_L.cols - width, src_L.rows));

		//将src_L的Mat 复制到half （即覆盖result中相应区域的图像）
		src_L(Range::all(), Range(0, src_L.cols - width)).copyTo(half);
		for (int i = 0; i < width; i++)
		{
			result.col(src_L.cols - width + i) = (width - i) / (float)width*src_L.col(src_L.cols - width + i) + i / (float)width*result.col(src_L.cols - width + i); 
			//权重	重叠区域中的像素点位置越靠近src_L,src_L原图中的像素占的权重越大，src_R中的像素点权重越小，反之亦然
			//带权重相加后便得到融合后的像素点
		}
		finish = clock();
		if (flag)		//flag==1时，打印出处理所用的总时间
		{
			printf("stitch: %.2f ms\n", (double)(finish - start));
		}
		return result;
	}


	Mat StitchFrame::stitch_v(int width, int flag)
	{
		start = clock();

		//cols　列 rows 行
		//Size设置结果图像宽度，宽度裁去一部分，width可调，方法如上 函数stitch(...);
		warpPerspective(src_R, result, H, Size(src_R.cols, 1.5*src_R.rows - width));
		
		Mat half(result, Rect(0, 0, src_L.cols, src_L.rows - width));
		src_L(Range(0, src_L.rows - width), Range::all()).copyTo(half);
		for (int i = 0; i < width; i++)
		{
			result.row(src_L.rows - width + i) = (width - i) / (float)width*src_L.row(src_L.rows - width + i) + i / (float)width*result.row(src_L.rows - width + i);  //权重
		}
		finish = clock();
		if (flag)
		{
			printf("stitch: %.2f ms\n", (double)(finish - start));
		}
		return result;
	}

	Mat StitchFrame::getResult()
	{
		return result;
	}

	void StitchFrame::makeVideo(string pathL, string pathR)
	{	
		//定义相关的VideoCapture对象
		VideoCapture captureL, captureR;
		//读取视频文件
		captureL.open(pathL);
		captureR.open(pathR);
		//判断视频流读取是否正确
		if (!(captureL.isOpened() && captureR.isOpened())) 
			cout << "Fail to open the video!" << endl;
		//获取视频相关信息  总帧数
		unsigned int totalFrameL = captureL.get(CV_CAP_PROP_FRAME_COUNT);
		unsigned int totalFrameR = captureR.get(CV_CAP_PROP_FRAME_COUNT);
		cout << "Left Video has " << totalFrameL << " frames." << endl;
		cout << "Right Video has " << totalFrameR << " frames" << endl;	
		//获取视频相关信息  帧像素宽/高
		int frameHeightL, frameHeightR, frameWidthL, frameWidthR;
		frameHeightL = captureL.get(CV_CAP_PROP_FRAME_HEIGHT);
		frameHeightR = captureR.get(CV_CAP_PROP_FRAME_HEIGHT);
		frameWidthL = captureL.get(CV_CAP_PROP_FRAME_WIDTH);
		frameWidthR = captureR.get(CV_CAP_PROP_FRAME_WIDTH);
		cout << "frameHeigthL = " << frameHeightL << endl;
		cout << "frameHeigthR = " << frameHeightR << endl;
		cout << "frameWidthL = " << frameWidthL << endl;
		cout << "frameWidthR = " << frameWidthR << endl;
		//获取视频相关信息  帧率
		double frameRateL = captureL.get(CV_CAP_PROP_FPS);
		double frameRateR = captureR.get(CV_CAP_PROP_FPS);
		cout << "FPS_L = " << frameRateL << endl;
		cout << "FPS_R = " << frameRateR << endl;
		//read方法获取显示帧
		long nCount = 1;
		Mat frameImage;
		while (true) {
			cout << "		Current frame:" << nCount << endl;
			captureL >> frameImage;
			//判断文件是否读取完
			if (!frameImage.empty())
				imshow("frameImg", frameImage);
			else
				break;
			if (char(waitKey(1)) == 'q')
				break;
			nCount++;
		}
		captureL.release();
		captureR.release();
	}
