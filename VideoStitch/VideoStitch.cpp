#include "stdafx.h"
#include "VideoStitch.h"

VideoStitch & VideoStitch::readVideo(char dirName[],int videoNum)
{
	VideoCapture videoCapture[camMaxNum];
	StitchFrame stitchFrame[camMaxNum+1];
	VideoWriter output;

	for (int i = 0; i < videoNum; i++)
	{
		videoCapture[i].open(dirName);
		if (!videoCapture[i].isOpened())
		{
			cout << "Fail to open video " << i << " !	" << endl;
			return *this;
		}
	}
	while (true)
	{
		for (int i = 0; i < videoNum; i++) 
		{
			videoCapture[i] >> src[i];
			stitchFrame[0].setSRC_L(src[0]);
			stitchFrame[0].setSRC_R(src[1]).findH("H.xml", STITCH_SURF, true);
			stitchFrame[0].stitch(20);
			setResult(stitchFrame[0].getResult());
			if (!src[i].empty())
				imshow("i", src[i]);
			else
				return *this;
		}
		
		output.open(savePath, CV_FOURCC('D', 'I', 'V', 'X'), 24, size(result[0].get(),result[0].height()), true);
		output << frameImg;
		imshow(savePath, frameImg);
		if (char(waitKey(1)) == 'q')
			break;
	}

	return *this;
}

VideoStitch & VideoStitch::setResult(Mat mat)
{
	result[0] = mat;
	return *this;
	// TODO: 在此处插入 return 语句
}
