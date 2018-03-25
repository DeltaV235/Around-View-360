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


bool CamCapture::capture(int camNum, int width, int heigth, int cols, int rows, double fps, char saveDirName[], bool isReleaseCam = true)
{
	VideoCapture *cam[camMaxNum];														//指向VideoCapture对象的指针
	Size videoResolution[camMaxNum];													//Size对象，表示视频的分辨率
	VideoWriter output[camMaxNum];														//VideoWriter对象，保存视频的对象
	StitchFrame stitchFrame[camMaxNum];
	TimeDetection camTimeOut, showFps;
	char fileName[80];
	int count[camMaxNum] = { 0 }, openNum = 0, totalFrames = 0;
	bool isStart = false, isFirstFrame[camMaxNum] = { true,true,true,true,true,true }, hasntRebuild = true, continueReconnect = true;
	double frameTime;
	Mat frameImg[camMaxNum], showImg, noSignalImg(Size(width, heigth), CV_8UC3, Scalar(0, 0, 0));
	vector<Mat> imgs;
	vector<int> errCamNum;																//其中保存 未连接 和 超时 的 摄像头 的 index
	vector<int>::iterator iter;															//指向vector<int>的迭代器

	printStringOnImgCenter(noSignalImg, "No Signal", FONT_HERSHEY_SIMPLEX, 4, Scalar(255, 255, 255), 10);
	namedWindow("No signal", WINDOW_KEEPRATIO);
	imshow("No signal", noSignalImg);
	resizeWindow("No signal", 640, 320);
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

		if (cam[i]->isOpened() == false)								//如果当前摄像头未打开，输出错误信息，并将该摄像头index压入errCamNum中
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
	namedWindow("Cam", WINDOW_KEEPRATIO);
	resizeWindow("Cam", width*cols / 2, heigth*rows / 2);

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
			if (find(errCamNum.begin(), errCamNum.end(), i) != errCamNum.end())			//如果errCamNum中有此次循环对应的index，则说明该摄像头故障，则将对应的frameImg改为noSignalImg或noSignalImg
			{
				frameImg[i] = noSignalImg;
			}
			else																		//否则按正常情况 输出监视画面
			{
				camTimeOut.setStartPos();
				*cam[i] >> frameImg[i];
				camTimeOut.setEndPos();
				frameTime = camTimeOut.getCurTime(true);
				//cout << frameTime << endl;
				if (frameTime > 800 && !isFirstFrame[i])				//在不是采集的第一帧的情况下，如果写入一帧的时间超过800ms，则判定该摄像头超时、故障，并将其压入errCamNum中，并输出错误信息、释放该摄像头
				{
					errCamNum.push_back(i);
					cam[i]->release();
					cout << endl << "Cam" << i << " Timeout!" << endl;
				}
				isFirstFrame[i] = false;
			}
			imgs.push_back(frameImg[i]);					//将frameImg压入imgs中
			if (imgs.size() == camNum)
			{
				this->showImgsOneWindow(imgs, showImg, width, heigth, cols, rows);
				imgs.clear();
				imshow("Cam", showImg);
			}
			if (totalFrames == 48 && continueReconnect)												//48帧后重新连接故障的摄像头
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
				continueReconnect = false;
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
			switch (camNum)
			{
			case 2:
			{
				stitchFrame[0].setSRC_L(frameImg[0]);
				stitchFrame[0].setSRC_R(frameImg[1]);
				if (hasntRebuild)
				{
					stitchFrame[0].findH("Homography\\H.xml", STITCH_SIFT, true);
					hasntRebuild = true; //临时修改
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
					hasntRebuild = true; //临时修改
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
		if (_rmdir(saveDirName) == 0)
			printf("\nNone video has been captured.\nSuccessfully delete the folder : .\\%s \n", saveDirName);
		else
			printf("\nCan not delete folder, the folder may occupied.\n");
	}
	return true;
}


void CamCapture::showImgsOneWindow(vector<Mat>& Images, Mat& dst, int width, int height, int cols, int rows)
{
	const int imgNum = Images.size();				//得到Vector容器中图片个数
											//设定包含这些图片的窗口大小，这里都是BGR3通道，如果做灰度单通道，稍微改一下下面这行代码就可以
	//Mat Window(1280 * ((Num - 1) / rows + 1), 720 * rows, CV_8UC3, Scalar(0, 0, 0));
	Mat Window(Size(width * cols, height * rows), CV_8UC3, Scalar(0, 0, 0));
	//Mat Std_Image;										//存放标准大小的图片
	Mat imageROI;										//图片放置区域
	//Size Std_Size = Size(1280, 720);						//每个图片显示大小300*300
	int x_Begin = 0;
	int y_Begin = 0;
	for (int i = 0; i < imgNum; i++)
	{
		x_Begin = (i % cols)*width;			//每张图片起始坐标
		y_Begin = (i / cols)*height;
		//resize(Images[i], Std_Image, Std_Size, 0, 0, INTER_LINEAR);						//将图像设为标准大小
																						//将其贴在Window上
		imageROI = Window(Rect(x_Begin, y_Begin, width, height));
		Images[i].copyTo(imageROI);
	}
	dst = Window;
}

void CamCapture::printStringOnImgCenter(Mat& img, const String& text, int fontFace, double fontScale, Scalar color,
	int thickness, int lineType, bool bottomLeftOrigin)
{
	Size textSize = getTextSize(text, fontFace, fontScale, thickness, 0);
	Point origin;
	origin.x = img.cols / 2.0 - textSize.width / 2.0;
	origin.y = img.rows / 2.0 + textSize.height / 2.0;
	putText(img, text, origin, fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin);
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

//int listDevices(vector<string>& list) {
//
//	//COM Library Intialization
//	//comInit();
//
//	//if (!silent)printf("\nVIDEOINPUT SPY MODE!\n\n");
//
//
//	ICreateDevEnum *pDevEnum = NULL;
//	IEnumMoniker *pEnum = NULL;
//	int deviceCounter = 0;
//	CoInitialize(NULL);
//	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
//		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
//		reinterpret_cast<void**>(&pDevEnum));
//
//
//	if (SUCCEEDED(hr))
//	{
//		// Create an enumerator for the video capture category.
//		hr = pDevEnum->CreateClassEnumerator(
//			CLSID_VideoInputDeviceCategory,
//			&pEnum, 0);
//
//		if (hr == S_OK) {
//
//			//if (!silent)printf("SETUP: Looking For Capture Devices\n");
//			IMoniker *pMoniker = NULL;
//
//			while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
//
//				IPropertyBag *pPropBag;
//				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
//					(void**)(&pPropBag));
//
//				if (FAILED(hr)) {
//					pMoniker->Release();
//					continue;  // Skip this one, maybe the next one will work.
//				}
//
//
//				// Find the description or friendly name.
//				VARIANT varName;
//				VariantInit(&varName);
//				hr = pPropBag->Read(L"Description", &varName, 0);
//
//				if (FAILED(hr)) hr = pPropBag->Read(L"FriendlyName", &varName, 0);
//
//				if (SUCCEEDED(hr)) {
//
//					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
//
//					int count = 0;
//					char tmp[255] = { 0 };
//					//int maxLen = sizeof(deviceNames[0]) / sizeof(deviceNames[0][0]) - 2;
//					while (varName.bstrVal[count] != 0x00 && count < 255) {
//						tmp[count] = (char)varName.bstrVal[count];
//						count++;
//					}
//					list.push_back(tmp);
//					//deviceNames[deviceCounter][count] = 0;
//
//					//if (!silent)printf("SETUP: %i) %s \n", deviceCounter, deviceNames[deviceCounter]);
//				}
//
//				pPropBag->Release();
//				pPropBag = NULL;
//
//				pMoniker->Release();
//				pMoniker = NULL;
//
//				deviceCounter++;
//			}
//
//			pDevEnum->Release();
//			pDevEnum = NULL;
//
//			pEnum->Release();
//			pEnum = NULL;
//		}
//
//		//if (!silent)printf("SETUP: %i Device(s) found\n\n", deviceCounter);
//	}
//
//	//comUnInit();
//
//	return deviceCounter;
//}
