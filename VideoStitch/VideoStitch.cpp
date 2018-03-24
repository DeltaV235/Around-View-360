#include "stdafx.h"
#include "VideoStitch.h"

bool VideoStitch::readVideo(string dirName)
{
	bool flag = true;
	fileName = dir.GetListFiles(dirName, "Cam*.avi", true);							//遍历 dirName 文件夹下的 文件名 为 Cam*.avi 的文件，将其文件名 push进  vector<String> fileName中 
	for (int i = 0; i < (int)fileName.size(); i++)									//录了几路视频，呢么 fileName.size() 就是多少 ，那么就打开 几个 对应的文件
	{
		videoCapture[i].open(fileName[i]);											//循环打开视频
		if (!videoCapture[i].isOpened())
		{
			cout << "Fail to open video " << fileName[i] << " !	" << endl;			//如果打开失败则输出错误信息，并返回 false
			flag = false;
		}
		else
		{
			cout << fileName[i] << " has opened." << endl;							//依次获取各路视频的 分辨率 和 帧率
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_WIDTH) << " * ";
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_HEIGHT) << "	";
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_COUNT) << " frames" << endl;
			videoCapture[i] >> src[i];												//将视频中的第一帧输出到成员变量 src[i] 中
		}
	}
	return flag;																	//返回 true
}

bool VideoStitch::nextFrame()														//读取下一帧视频
{
	for (int i = 0; i < (int)fileName.size(); i++)
		videoCapture[i] >> src[i];
	// TODO: 在此处插入 return 语句
	for (int i = 0; i < (int)fileName.size(); i++)
	{
		if (src[i].empty())															//如果下一帧为空，即视频已经结束，则返回 false
			return false;
		else
			return true;
	}
}

VideoStitch & VideoStitch::release()												//释放所有视频
{
	for (int i = 0; i < (int)fileName.size(); i++)
	{
		videoCapture[i].release();
	}
	return *this;
	// TODO: 在此处插入 return 语句
}

bool VideoStitch::stitchVideo(string savePath, bool isRebuild)								//拼接单帧画面 并 输出到视频容器中，形成一段完整的视频 调用 Stitch 项目 中的StitchFrame类
{
	bool hasntRebuild = true, isOpened = true;
	int count = 0;
	char imgName[10];
	int imgNum = 0;
	while (true)
	{
		//for (int i = 0; i < (int)fileName.size() - 1; i++)
		//{
		namedWindow(savePath, WINDOW_KEEPRATIO);
		switch ((int)fileName.size())
		{
		case 2:
		{
			stitchFrame[0].setSRC_L(src[0]);
			stitchFrame[0].setSRC_R(src[1]);
			if (hasntRebuild)
			{
				stitchFrame[0].findH("Homography\\H.xml", STITCH_SIFT, isRebuild);
				hasntRebuild = false;
			}
			stitchFrame[0].stitch(50);
			resizeWindow(savePath, stitchFrame[0].getResult().cols, stitchFrame[0].getResult().rows);
			imshow(savePath, stitchFrame[0].getResult());

			break;
		}
		case 4:
		{
			stitchFrame[0].setSRC_L(src[0]);
			stitchFrame[0].setSRC_R(src[1]);
			stitchFrame[1].setSRC_L(src[2]);
			stitchFrame[1].setSRC_R(src[3]);
			if (hasntRebuild)
			{
				stitchFrame[0].findH("Homography\\H_U.xml", STITCH_SIFT, isRebuild);
				stitchFrame[1].findH("Homography\\H_D.xml", STITCH_SIFT, isRebuild);
				//stitchFrame[0].show("up");
				//stitchFrame[1].show("down");
			}

			stitchFrame[2].setSRC_L(stitchFrame[0].stitch(50));
			stitchFrame[2].setSRC_R(stitchFrame[1].stitch(50));
			if (hasntRebuild)
			{
				stitchFrame[2].findH("Homography\\H_UD.xml", STITCH_SIFT, isRebuild);
				stitchFrame[2].show("whole");
				hasntRebuild = false;
			}
			stitchFrame[2].stitch_v(50);
			resizeWindow(savePath, stitchFrame[2].getResult().cols / 1.5, stitchFrame[2].getResult().rows / 1.5);
			imshow(savePath, stitchFrame[2].getResult());

			break;
		}
		case 6:
		{
			stitchFrame[0].setSRC_L(src[0]).setSRC_R(src[1]);
			stitchFrame[1].setSRC_L(src[1]).setSRC_R(src[2]);
			stitchFrame[2].setSRC_L(src[3]).setSRC_R(src[4]);
			stitchFrame[3].setSRC_L(src[4]).setSRC_R(src[5]);
			if (hasntRebuild)
			{
				stitchFrame[0].findH("Homography\\H_1LC.xml", STITCH_SIFT, isRebuild);
				stitchFrame[1].findH("Homography\\H_1CR.xml", STITCH_SIFT, isRebuild);
				stitchFrame[2].findH("Homography\\H_2LC.xml", STITCH_SIFT, isRebuild);
				stitchFrame[3].findH("Homography\\H_2CR.xml", STITCH_SIFT, isRebuild);
				//stitchFrame[0].show("up");
				//stitchFrame[1].show("down");
			}

			if (hasntRebuild)
			{
				stitchFrame[4].setSRC_L(stitchFrame[0].setSRC_R(stitchFrame[1].stitch(50)).stitch(50)).setSRC_R(stitchFrame[2]
					.setSRC_R(stitchFrame[3].stitch(50)).stitch(50)).findH("Homography\\H_WHOLE.xml", STITCH_SIFT, isRebuild);
				//stitchFrame[2].show("whole");
				hasntRebuild = true; //临时修改
			}
			stitchFrame[4].stitch_v(50);
			resizeWindow(savePath, stitchFrame[4].getResult().cols / 2, stitchFrame[4].getResult().rows / 2);
			imshow(savePath, stitchFrame[4].getResult());

			break;
		}
		default:
		{
			if (fileName.size() == 0)
			{
				
			}
			else
			{
				cout << "arg error" << endl;
			}
			return false;
		}
		}
		if (isOpened)														//创建视频文件
		{
			outPut.open(savePath, CV_FOURCC('D', 'I', 'V', 'X'), 24, Size(stitchFrame[(int)fileName.size() - 2].getResult().cols, stitchFrame[(int)fileName.size() - 2].getResult().rows), true);
			if (!outPut.isOpened())
				cout << "Fail to save the Final Video !" << endl;
			else
				isOpened = false;
		}
		
		/*sprintf(imgName,"%03d" ,imgNum);
		imgNum++;
		imwrite(imgName, stitchFrame[(int)fileName.size() - 2].getResult());*/
		outPut << stitchFrame[(int)fileName.size() - 2].getResult();			//写入处理完的图片
		count++;
		//}
		if (char(waitKey(1)) == 'q')
		{
			destroyWindow(savePath);
			cout << "writeTotalFrame(result): " << count << endl;
			break;
		}
		if (!this->nextFrame())
		{
			destroyWindow(savePath);
			cout << "writeTotalFrame(result): " << count << endl;
			break;
		}
	}
	this->release();
	return true;
	// TODO: 在此处插入 return 语句
}

VideoStitch & VideoStitch::stitchVideo2(string savePath)						//读取视频，拼接单帧图片，基于Stitcher类的拼接方法，无法输出成视频
{
	vector<Mat> imgs;
	Mat result;

	//char picName[80];
	bool isOpened = true;
	int count = 0;
	while (true)
	{
		for (int i = 0; i < (int)fileName.size() - 1; i++)
		{
			/*resize(src[i], src[i], Size(640, 360), 0, 0, CV_INTER_LINEAR);
			resize(src[i+1], src[i+1], Size(640, 360), 0, 0, CV_INTER_LINEAR);*/
			imgs.push_back(src[i]);
			imgs.push_back(src[i + 1]);
			stitch(imgs, result);
			cout << "output " << count + 1 << " frame" << endl;
			//sprintf(picName, "\\%04d.jpg", count+1);
			//imwrite(savePath+picName, result);

			if (!result.empty())
			{
				namedWindow(savePath, WINDOW_KEEPRATIO);
				imshow(savePath, result);
			}
			if (isOpened)
			{
				outPut.open(savePath, CV_FOURCC('D', 'I', 'V', 'X'), 24, Size(result.cols*1.2, result.rows*1.2), true);
				if (!outPut.isOpened())
					cout << "Fail to save the Final Video !" << endl;
				else
					isOpened = false;
			}
			outPut << result;


			count++;
		}
		imgs.clear();
		if (char(waitKey(1)) == 'q')
		{
			destroyWindow(savePath);
			cout << "writeTotalFrame(result): " << count << endl;
			break;
		}
		if (!this->nextFrame())
		{
			destroyWindow(savePath);
			cout << "writeTotalFrame(result): " << count << endl;
			break;
		}
	}
	this->release();
	return *this;
	// TODO: 在此处插入 return 语句
}



//VideoStitch & VideoStitch::setResult(Mat mat)
//{
//	result[0] = mat;
//	return *this;
//	// TODO: 在此处插入 return 语句
//}
