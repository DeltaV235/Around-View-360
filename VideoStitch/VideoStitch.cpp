#include "stdafx.h"
#include "VideoStitch.h"

bool VideoStitch::readVideo(string dirName)
{
	bool flag = true;
	fileName = dir.GetListFiles(dirName, "*.avi", true);
	for (int i = 0; i < fileName.size(); i++)
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
	for (int i = 0; i < fileName.size(); i++)
	{
		videoCapture[i] >> src[i];
		if (src[i].empty())
			return false;
		else
			return true;
	}
	// TODO: 在此处插入 return 语句
	return false;
}

VideoStitch & VideoStitch::release()
{
	for (int i = 0; i < fileName.size(); i++)
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
				stitchFrame[i].findH("H.xml", STITCH_SURF, true);
				flag = false;
			}
			stitchFrame[i].stitch(20);
			imshow(savePath, stitchFrame[i].getResult());
		}
		for (int i = 0; i < (int)fileName.size() - 1; i++)
		{
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
			cout << "writeTotalFrame(result): " << count << endl;
			break;
		}
		if (!this->nextFrame())
		{
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
