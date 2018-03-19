#include "stdafx.h"
#include "CamCapture.h"


CamCapture & CamCapture::capture(int camSeq, int width, int heigth, double fps, string savePath)
{
	VideoCapture capture(camSeq);
	VideoWriter output;
	int count = 0;
	Mat frameImg;
	bool isStart = false;
	capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, heigth);
	capture.set(CV_CAP_PROP_FPS, fps);
	Size videoResolution = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));		//获取视频分辨率
	if (!capture.isOpened())
		cout << "Cam hasnt opened !" << endl;
	output.open(savePath, CV_FOURCC('D', 'I', 'V', 'X'), fps, videoResolution, true);
	if (!output.isOpened())
		cout << "Fail to open file !" << endl;
	waitKey(1000);
	while (true)
	{
		if (char(waitKey(1)) == ' ')
			isStart = true;
		capture >> frameImg;
		imshow(savePath, frameImg);
		if (!frameImg.empty() && isStart)
		{
			count++;
			output << frameImg;
		}
		/*else
		{
			cout << "Fail to save frame !" << endl;
			break;
		}*/
		if (char(waitKey(1)) == 'q')
		{
			cout << "writeTotalFrame: " << count << endl;
			break;
		}
	}
	return *this;
	// TODO: 在此处插入 return 语句
}


bool CamCapture::capture(int  camNum, int width, int heigth, double fps, char saveDirName[])
{
	VideoCapture *cam[camMaxNum];
	Size videoResolution[camMaxNum];
	VideoWriter output[camMaxNum];
	StitchFrame stitchFrame;
	char fileName[80];
	int count[camMaxNum] = { 0 }, openNum = 0;
	bool isStart = false;
	Mat frameImg[camMaxNum];
	for (int i = 0; i < camNum; i++)					//设置摄像头参数，并打开摄像头
	{
		
		cam[i] = new VideoCapture(i);
		cam[i]->set(CV_CAP_PROP_FRAME_WIDTH, width);
		cam[i]->set(CV_CAP_PROP_FRAME_HEIGHT, heigth);
		cam[i]->set(CV_CAP_PROP_FPS, fps);

		//cam[i]->set(CV_CAP_PROP_BRIGHTNESS, 128);			//亮度 1
		//cam[i]->set(CV_CAP_PROP_CONTRAST, 50);			//对比度 40
		//cam[i]->set(CV_CAP_PROP_SATURATION, 50);			//饱和度 50
		//cam[i]->set(CV_CAP_PROP_HUE, 50);					//色调 50
		//cam[i]->set(CV_CAP_PROP_EXPOSURE, -2);			//曝光 50
		//cam[i]->set(CV_CAP_PROP_FOCUS, 50);

		videoResolution[i] = Size((int)cam[i]->get(CV_CAP_PROP_FRAME_WIDTH), (int)cam[i]->get(CV_CAP_PROP_FRAME_HEIGHT));		//获取视频分辨率
		
		if (cam[i]->isOpened() == false)
		{
			cout << "Cam " << i << " hasnt opened !" << endl;
			cout << "Exit! " << endl;
			for (int index = 0; index < i; index++)
			{
				cam[index]->release();
			}
			return false;
		}
	}
	waitKey(1000);

	while (true)
	{
		if (char(waitKey(1)) == ' ')				//输入<sp> 开始录像
		{
			isStart = true;
			cout << "Start Record !" << endl;
		}
		for (int i = 0; i < camNum; i++)					//显示实时采集到的画面
		{
			*cam[i] >> frameImg[i];
			sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
			namedWindow(fileName, WINDOW_KEEPRATIO);
			imshow(fileName, frameImg[i]);
			if (!frameImg[i].empty() && isStart)			//如果已经采集到画面 并且 开始录像，则开始将画面保存到指定的目录下
			{
				if (openNum < camNum)
				{
					sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
					output[i].open(fileName, CV_FOURCC('D', 'I', 'V', 'X'), fps, videoResolution[i], true);
					if (!output[i].isOpened())
						cout << "Fail to open file " << i << " !" << endl;
					openNum++;
				}
				count[i]++;
				output[i] << frameImg[i];
			}
			/*else
			{
				cout << "Fail to save frame for Cam " << i << " !" << endl;
				break;
			}*/
		}
		if (count[0] == 150)							//当保存了X帧后，停止录像，并返回
		{
			for (int i = 0; i < camNum; i++)
			{
				cam[i]->release();
				sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
				destroyWindow(fileName);
			}
			break;
		}

		if (char(waitKey(1)) == 'a')						//如果按下 a ，则找到其中的两幅图像的映射矩阵，拼接，并显示拼接后图像，映射矩阵保存为 preViewH.xml
		{
			stitchFrame.setSRC_L(frameImg[0]);
			stitchFrame.setSRC_R(frameImg[1]);
			stitchFrame.findH("preViewH.xml", STITCH_SIFT, true);
			namedWindow("outimg", WINDOW_KEEPRATIO);
			stitchFrame.show("outimg");
			namedWindow("preView", WINDOW_KEEPRATIO);
			imshow("preView", stitchFrame.stitch(20));


			/*Mat result, result2;
			vector<Mat> imgs;
			imgs.push_back(frameImg[0]);
			imgs.push_back(frameImg[1]);
			stitch(imgs, result);
			if (!result.empty())
			{
				namedWindow("test stitch", WINDOW_KEEPRATIO);
				imshow("test stitch", result);
			}


			stitch2(frameImg[0], frameImg[1], result2);
			if (!result2.empty())
			{
				namedWindow("test stitch2", WINDOW_KEEPRATIO);
				imshow("test stitch2", result2);
			}*/

		}
		if (char(waitKey(1)) == 'q')					//按下 q ,退出录像，释放所有网络摄像头，关闭所有摄像头的窗口
		{
			for (int i = 0; i < camNum; i++)
			{
				cam[i]->release();
				sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
				destroyWindow(fileName);
			}

			for (int i = 0; i < camNum; i++)			//输出一共写入硬盘的帧数
				cout << "writeTotalFrame for Cam " << i << " : " << count[i] << endl;
			break;
		}
	}
	return true;
}


void stitch(vector<Mat> imgs, Mat& resultMat)
{
	bool Flag = true;
	// 定义Stitcher类
	Stitcher stitcher = Stitcher::createDefault(Flag);					//OpenCV自带的Stitch方法
	Stitcher::Status status = stitcher.stitch(imgs, resultMat);
	if (status != Stitcher::OK) {
		std::cout << "error" << std::endl;
	}
}
void stitch2(Mat& srcImage1, Mat& srcImage2, Mat& panorama)				//另一种相似的拼接方法
{
	// SURF特征点描述
	Mat srcImage2Warped;
	// 初始化SURF检测描述子
	cv::Ptr<Feature2D> surf = xfeatures2d::SURF::create();

	// 关键点及特征描述矩阵声明
	vector<cv::KeyPoint> kPointMat, kPointMat2;;
	cv::Mat desMat1, desMat2;

	surf->detectAndCompute(srcImage1, Mat(), kPointMat, desMat1);
	surf->detectAndCompute(srcImage2, Mat(), kPointMat2, desMat2);

	// FLANN 关键点匹配
	// FLANN 初始化 RANSAC一致性 最近邻特征
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(desMat1, desMat2, matches);
	double max_dist = 0, min_dist = 100;
	// 特征点最大与最小距离查找
	for (int i = 0; i < desMat1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	// 使用good 特征构建距离限定 
	std::vector< DMatch > good_matches;
	for (int i = 0; i < desMat1.rows; i++)
	{
		if (matches[i].distance < 2 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	// 图像的关键点匹配
	std::vector<Point2f> srcImage1_matchedKPs;
	std::vector<Point2f> srcImage2_matchedKPs;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		srcImage1_matchedKPs.push_back(kPointMat[good_matches[i].queryIdx].pt);
		srcImage2_matchedKPs.push_back(kPointMat2[good_matches[i].trainIdx].pt);
	}
	// 计算图像1与图像2的映射
	Mat H = findHomography(Mat(srcImage2_matchedKPs),
		Mat(srcImage1_matchedKPs), CV_RANSAC);
	// 仿射变换
	warpPerspective(srcImage2, srcImage2Warped, H, Size(srcImage2.cols * 2, srcImage2.rows), INTER_CUBIC);
	panorama = srcImage2Warped.clone();
	// 结果输出
	Mat roi(panorama, Rect(0, 0,srcImage1.cols, srcImage1.rows));
	srcImage1.copyTo(roi);
}