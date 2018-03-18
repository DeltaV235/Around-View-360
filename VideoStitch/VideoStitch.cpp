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
		//for (int i = 0; i < (int)fileName.size() - 1; i++)
		//{
		namedWindow(savePath, WINDOW_FULLSCREEN);
		switch ((int)fileName.size() - 1)
		{
			case 1:
			{
				stitchFrame[0].setSRC_L(src[0]);
				stitchFrame[0].setSRC_R(src[1]);
				if (flag)
				{
					stitchFrame[0].findH("H.xml", STITCH_SIFT, true);
					flag = false;
				}
				stitchFrame[0].stitch(50);
				imshow(savePath, stitchFrame[0].getResult());
			}
			case 3:
			{
				stitchFrame[0].setSRC_L(src[0]);
				stitchFrame[0].setSRC_R(src[1]);
				stitchFrame[1].setSRC_L(src[2]);
				stitchFrame[1].setSRC_R(src[3]);
				if (flag)
				{
					stitchFrame[0].findH("H_U.xml", STITCH_SIFT, false);
					stitchFrame[1].findH("H_D.xml", STITCH_SIFT, false);
					stitchFrame[0].show("up");
					stitchFrame[1].show("down");
				}
				
				stitchFrame[2].setSRC_L(stitchFrame[0].stitch(50));
				stitchFrame[2].setSRC_R(stitchFrame[1].stitch(50));
				if (flag)
				{
					stitchFrame[2].findH("H_Whole.xml", STITCH_SIFT, false);
					stitchFrame[2].show("whole");
					flag = false;
				}
				stitchFrame[2].stitch_v(50);
				imshow(savePath, stitchFrame[2].getResult());
			}
			default:
				break;
		}	
		if (isOpened)
		{
			outPut.open(savePath, CV_FOURCC('D', 'I', 'V', 'X'), 24, Size(stitchFrame[(int)fileName.size() - 2].getResult().cols, stitchFrame[(int)fileName.size() - 2].getResult().rows), true);
			if (!outPut.isOpened())
				cout << "Fail to save the Final Video !" << endl;
			else
				isOpened = false;
		}
		outPut << stitchFrame[(int)fileName.size() - 2].getResult();
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
	return *this;
	// TODO: 在此处插入 return 语句
}

VideoStitch & VideoStitch::stitchVideo2(string savePath)
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
			cout << "output "<<count+1<<" frame" << endl;
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
