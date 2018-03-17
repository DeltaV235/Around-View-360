#include "stdafx.h"
#include "VideoStitch.h"

bool VideoStitch::readVideo(string dirName)
{
	bool flag = true;
	fileName = dir.GetListFiles(dirName, "Cam*.avi", true);
	for (int i = 0; i < (int)fileName.size(); i++)
	{
		videoCapture[i].open(fileName[i]);
		if (!videoCapture[i].isOpened())
		{
			cout << "Fail to open video " << fileName[i] << " !	" << endl;
			flag = false;
		}
		else
		{
			cout << fileName[i] << " has opened." << endl;
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_WIDTH) << " * ";
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_HEIGHT) << "	";
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_COUNT) << " frames" << endl;
			videoCapture[i] >> src[i];
		}
	}
	return flag;
}

bool VideoStitch::nextFrame()
{
	for (int i = 0; i < (int)fileName.size(); i++)
		videoCapture[i] >> src[i];
	// TODO: 在此处插入 return 语句
	for (int i = 0; i < (int)fileName.size(); i++)
	{
		if (src[i].empty())
			return false;
		else
			return true;
	}
}

VideoStitch & VideoStitch::release()
{
	for (int i = 0; i < (int)fileName.size(); i++)
	{
		videoCapture[i].release();
	}
	return *this;
	// TODO: 在此处插入 return 语句
}

VideoStitch & VideoStitch::stitchVideo(string savePath)
{
	bool flag = true, isOpened = true;
	int count = 0;
	while (true)
	{
		for (int i = 0; i < (int)fileName.size() - 1; i++)
		{
			stitchFrame[i].setSRC_L(src[i]);
			stitchFrame[i].setSRC_R(src[i + 1]);
			if (flag)
			{
				stitchFrame[i].findH("H.xml", STITCH_SURF_FLANN, true);
				flag = false;
			}
			stitchFrame[i].stitch(20);
			namedWindow(savePath, WINDOW_KEEPRATIO);
			imshow(savePath, stitchFrame[i].getResult());

			if (isOpened)
			{
				outPut.open(savePath, CV_FOURCC('D', 'I', 'V', 'X'), 24, Size(stitchFrame[i].getResult().cols, stitchFrame[i].getResult().rows), true);
				if (!outPut.isOpened())
					cout << "Fail to save the Final Video !" << endl;
				else
					isOpened = false;
			}
			outPut << stitchFrame[i].getResult();
			count++;
		}
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

VideoStitch & VideoStitch::stitchVideo2(string savePath)
{
	vector<Mat> imgs;
	Mat result;


	bool isOpened = true;
	int count = 0;
	while (true)
	{
		for (int i = 0; i < (int)fileName.size() - 1; i++)
		{
			resize(src[i], src[i], Size(640, 360), 0, 0, CV_INTER_LINEAR);
			resize(src[i+1], src[i+1], Size(640, 360), 0, 0, CV_INTER_LINEAR);
			imgs.push_back(src[i]); 
			imgs.push_back(src[i + 1]);
			stitch(imgs, result);
			cout << "output "<<count+1<<" frame" << endl;
			if (!result.empty())
			{
				namedWindow(savePath, WINDOW_KEEPRATIO);
				imshow(savePath, result);
				outPut << result;
				count++;
			}
			if (isOpened)
			{
				outPut.open(savePath, CV_FOURCC('D', 'I', 'V', 'X'), 24, Size(stitchFrame[i].getResult().cols*1.5, stitchFrame[i].getResult().rows*1.5), true);
				if (!outPut.isOpened())
					cout << "Fail to save the Final Video !" << endl;
				else
					isOpened = false;
			}
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
