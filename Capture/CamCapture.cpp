#include "stdafx.h"
#include "CamCapture.h"

#define camMaxNum 6

CamCapture & CamCapture::capture(int camSeq, int width, int heigth, double fps, string savePath)
{
	VideoCapture capture(camSeq);
	VideoWriter output;
	int count = 0;
	Mat frameImg;

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
		capture >> frameImg;
		if (!frameImg.empty())
		{
			count++;
			output << frameImg;
			imshow(savePath, frameImg);
		}
		else
		{
			cout << "Fail to save frame !" << endl;
			break;
		}
		if (char(waitKey(1)) == 'q')
		{
			cout << "writeTotalFrame: " << count << endl;
			break;
		}
	}
	return *this;
	// TODO: 在此处插入 return 语句
}


CamCapture & CamCapture::capture(unsigned int  camNum, int width, int heigth, double fps, char saveDirName[])
{
	VideoCapture *cam[camMaxNum];
	Size videoResolution[camMaxNum];
	VideoWriter output[camMaxNum];
	char fileName[80];
	int count[camMaxNum] = { 0 };
	Mat frameImg;
	for (int i = 0; i < camNum; i++)
	{
		cam[i] = new VideoCapture(i);
		cam[i]->set(CV_CAP_PROP_FRAME_WIDTH, width);
		cam[i]->set(CV_CAP_PROP_FRAME_HEIGHT, heigth);
		cam[i]->set(CV_CAP_PROP_FPS, fps);
		videoResolution[i] = Size((int)cam[i]->get(CV_CAP_PROP_FRAME_WIDTH), (int)cam[i]->get(CV_CAP_PROP_FRAME_HEIGHT));		//获取视频分辨率
		if (!cam[i]->isOpened())
			cout << "Cam " << i << " hasnt opened !" << endl;
		sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
		output[i].open(fileName, CV_FOURCC('D', 'I', 'V', 'X'), fps, videoResolution[i], true);
		if (!output[i].isOpened())
			cout << "Fail to open file " << i << " !" << endl;
	}
	waitKey(1000);

	while (true)
	{
		for (int i = 0; i < camNum; i++) 
		{
			*cam[i] >> frameImg;
			if (!frameImg.empty())
			{
				count[i]++;
				output[i] << frameImg;
				sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
				imshow(fileName, frameImg);
			}
			else
			{
				cout << "Fail to save frame for Cam " << i << " !" << endl;
				break;
			}
		}
		if (char(waitKey(1)) == 'q')
		{
			for (int i = 0; i < camNum; i++)
				cout << "writeTotalFrame for Cam " << i << " : " << count[i] << endl;
			break;
		}
	}
	return *this;
}