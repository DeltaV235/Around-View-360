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


bool CamCapture::capture(int camNum, int width, int heigth, double fps, char saveDirName[], bool isReleaseCam = true)
{
	VideoCapture *cam[camMaxNum];
	Size videoResolution[camMaxNum];
	VideoWriter output[camMaxNum];
	StitchFrame stitchFrame[camMaxNum];
	TimeDetection camTimeOut,showFps;
	char fileName[80];
	int count[camMaxNum] = { 0 }, openNum = 0, totalFrames=0;
	bool isStart = false, isFirstFrame[camMaxNum] = { true,true,true,true,true,true }, hasntRebuild = true;
	double frameTime;
	Mat frameImg[camMaxNum], showImg, blackImg(Size(width,heigth),CV_8UC3, Scalar(0,0,0));
	vector<Mat> imgs;
	vector<int> errCamNum;
	vector<int>::iterator iter;

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
			/*cout << "Exit! " << endl;
			for (int index = 0; index < i; index++)
			{
				cam[index]->release();
			}
			return false;*/
			errCamNum.push_back(i);
		}
	}
	waitKey(1000);

	while (true)
	{
		showFps.setStartPos();
		if (char(waitKey(1)) == ' ')				//输入<sp> 开始录像
		{
			isStart = true;
			cout << "Start Record !" << endl;
		}
		for (int i = 0; i < camNum; i++)					//显示实时采集到的画面
		{
			if (find(errCamNum.begin(), errCamNum.end(), i) != errCamNum.end())
			{
				frameImg[i] = blackImg;
			}
			else
			{
				camTimeOut.setStartPos();
				*cam[i] >> frameImg[i];
				camTimeOut.setEndPos();
				frameTime = camTimeOut.getCurTime(true);
				//cout << frameTime << endl;
				if (frameTime > 800 && !isFirstFrame[i])
				{
					errCamNum.push_back(i);
					cam[i]->release();
					cout << endl << "Cam" << i << " Timeout!" << endl;
				}
				isFirstFrame[i] = false;
			}
			imgs.push_back(frameImg[i]);
			if (imgs.size() == camNum)
			{
				this->showImgsOneWindow(imgs, showImg, 3);
				imgs.clear();
				namedWindow("Cam", WINDOW_KEEPRATIO);
				resizeWindow("Cam", 1920, 720);
				imshow("Cam", showImg);
			}
			if (totalFrames == 48)
			{
				for (iter = errCamNum.begin(); iter != errCamNum.end(); )
				{
					cam[*iter] = new VideoCapture(*iter);
					cam[*iter]->set(CV_CAP_PROP_FRAME_WIDTH, width);
					cam[*iter]->set(CV_CAP_PROP_FRAME_HEIGHT, heigth);
					cam[*iter]->set(CV_CAP_PROP_FPS, fps);

					//cam[i]->set(CV_CAP_PROP_BRIGHTNESS, 128);			//亮度 1
					//cam[i]->set(CV_CAP_PROP_CONTRAST, 50);			//对比度 40
					//cam[i]->set(CV_CAP_PROP_SATURATION, 50);			//饱和度 50
					//cam[i]->set(CV_CAP_PROP_HUE, 50);					//色调 50
					//cam[i]->set(CV_CAP_PROP_EXPOSURE, -2);			//曝光 50
					//cam[i]->set(CV_CAP_PROP_FOCUS, 50);

					videoResolution[*iter] = Size((int)cam[*iter]->get(CV_CAP_PROP_FRAME_WIDTH), (int)cam[*iter]->get(CV_CAP_PROP_FRAME_HEIGHT));		//获取视频分辨率
					if (cam[*iter]->isOpened() == true)
					{
						iter = errCamNum.erase(iter++);
					}
					else
					{
						iter++;
						cout << "Cam " << *iter << " hasnt opened !" << endl;
					}
				}
				totalFrames = 0;
			}
			/*sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
			namedWindow(fileName, WINDOW_KEEPRATIO);
			resizeWindow(fileName, 1280, 720);
			imshow(fileName, frameImg[i]);*/

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
		totalFrames++;
		if (count[0] == 48)							//当保存了X帧后，停止录像，并返回
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
			/*stitchFrame.setSRC_L(frameImg[0]);
			stitchFrame.setSRC_R(frameImg[1]);
			stitchFrame.findH("preViewH.xml", STITCH_SIFT, true);
			namedWindow("outimg", WINDOW_KEEPRATIO);
			stitchFrame.show("outimg");
			namedWindow("preView", WINDOW_KEEPRATIO);
			imshow("preView", stitchFrame.stitch(20));*/
			namedWindow("previewImg", WINDOW_KEEPRATIO);


			switch (camNum )
			{
			case 2:
			{
				stitchFrame[0].setSRC_L(frameImg[0]);
				stitchFrame[0].setSRC_R(frameImg[1]);
				if (hasntRebuild)
				{
					stitchFrame[0].findH("Homography\\H.xml", STITCH_SIFT, true);
					hasntRebuild = false;
				}
				stitchFrame[0].stitch(50);
				resizeWindow("previewImg", stitchFrame[0].getResult().cols, stitchFrame[0].getResult().rows);
				imshow("previewImg", stitchFrame[0].getResult());

				break;
			}
			case 4:
			{
				stitchFrame[0].setSRC_L(frameImg[0]);
				stitchFrame[0].setSRC_R(frameImg[1]);
				stitchFrame[1].setSRC_L(frameImg[2]);
				stitchFrame[1].setSRC_R(frameImg[3]);
				if (hasntRebuild)
				{
					stitchFrame[0].findH("Homography\\H_U.xml", STITCH_SIFT, true);
					stitchFrame[1].findH("Homography\\H_D.xml", STITCH_SIFT, true);
					//stitchFrame[0].show("up");
					//stitchFrame[1].show("down");
				}

				stitchFrame[2].setSRC_L(stitchFrame[0].stitch(50));
				stitchFrame[2].setSRC_R(stitchFrame[1].stitch(50));
				if (hasntRebuild)
				{
					stitchFrame[2].findH("Homography\\H_UD.xml", STITCH_SIFT, true);
					stitchFrame[2].show("whole");
					hasntRebuild = false;
				}
				stitchFrame[2].stitch_v(50);
				resizeWindow("previewImg", stitchFrame[2].getResult().cols / 1.5, stitchFrame[2].getResult().rows / 1.5);
				imshow("previewImg", stitchFrame[2].getResult());

				break;
			}
			case 6:
			{
				stitchFrame[0].setSRC_L(frameImg[0]).setSRC_R(frameImg[1]);
				stitchFrame[1].setSRC_L(frameImg[1]).setSRC_R(frameImg[2]);
				stitchFrame[2].setSRC_L(frameImg[3]).setSRC_R(frameImg[4]);
				stitchFrame[3].setSRC_L(frameImg[4]).setSRC_R(frameImg[5]);
				if (hasntRebuild)
				{
					stitchFrame[0].findH("Homography\\H_1LC.xml", STITCH_SIFT, true);
					stitchFrame[1].findH("Homography\\H_1CR.xml", STITCH_SIFT, true);
					stitchFrame[2].findH("Homography\\H_2LC.xml", STITCH_SIFT, true);
					stitchFrame[3].findH("Homography\\H_2CR.xml", STITCH_SIFT, true);
					//stitchFrame[0].show("up");
					//stitchFrame[1].show("down");
				}

				if (hasntRebuild)
				{
					stitchFrame[4].setSRC_L(stitchFrame[0].setSRC_R(stitchFrame[1].stitch(50)).stitch(50)).setSRC_R(stitchFrame[2]
						.setSRC_R(stitchFrame[3].stitch(50)).stitch(50)).findH("Homography\\H_WHOLE.xml", STITCH_SIFT, true);
					//stitchFrame[2].show("whole");
					hasntRebuild = true; //临时修改
				}
				stitchFrame[4].stitch_v(50);
				resizeWindow("previewImg", stitchFrame[4].getResult().cols / 3, stitchFrame[4].getResult().rows / 3);
				imshow("previewImg", stitchFrame[4].getResult());

				break;
			}
			default:
			{
				cout << "arg error" << endl;
				return false;
			}
			}

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
		showFps.setEndPos().getAvgFps().getAvgTime().getCurTime();
	}
	if (count[0] == 0)
	{
		if(_rmdir(saveDirName)==0)
			printf("\nNone video has been captured.\nSuccessfully delete the folder : .\\%s \n", saveDirName);
		else
			printf("\nCan not delete folder, the folder may occupied.\n");
	}
	return true;
}


void CamCapture::showImgsOneWindow(vector<Mat>& Images, Mat& dst, int rows)
{
	int Num = int(Images.size());				//得到Vector容器中图片个数
											//设定包含这些图片的窗口大小，这里都是BGR3通道，如果做灰度单通道，稍微改一下下面这行代码就可以
	//Mat Window(1280 * ((Num - 1) / rows + 1), 720 * rows, CV_8UC3, Scalar(0, 0, 0));
	Mat Window(Size(1280 * 3, 720 * 2), CV_8UC3, Scalar(0, 0, 0));
	Mat Std_Image;										//存放标准大小的图片
	Mat imageROI;										//图片放置区域
	Size Std_Size = Size(1280, 720);						//每个图片显示大小300*300
	int x_Begin = 0;
	int y_Begin = 0;
	for (int i = 0; i < Num; i++)
	{
		x_Begin = (i % rows)*Std_Size.width;			//每张图片起始坐标
		y_Begin = (i / rows)*Std_Size.height;
		//resize(Images[i], Std_Image, Std_Size, 0, 0, INTER_LINEAR);						//将图像设为标准大小
																						//将其贴在Window上
		imageROI = Window(Rect(x_Begin, y_Begin, Std_Size.width, Std_Size.height));
		Images[i].copyTo(imageROI);
	}
	dst = Window;
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
	Mat roi(panorama, Rect(0, 0, srcImage1.cols, srcImage1.rows));
	srcImage1.copyTo(roi);
}