#include "stdafx.h"
#include "CamCapture.h"


CamCapture & CamCapture::capture(int camNum, int width, int heigth, double fps, string savePath)
{
	VideoCapture capture(camNum);
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