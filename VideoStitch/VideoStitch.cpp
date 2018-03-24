#include "stdafx.h"
#include "VideoStitch.h"

bool VideoStitch::readVideo(string dirName)
{
	bool flag = true;
	fileName = dir.GetListFiles(dirName, "Cam*.avi", true);							//���� dirName �ļ����µ� �ļ��� Ϊ Cam*.avi ���ļ��������ļ��� push��  vector<String> fileName�� 
	for (int i = 0; i < (int)fileName.size(); i++)									//¼�˼�·��Ƶ����ô fileName.size() ���Ƕ��� ����ô�ʹ� ���� ��Ӧ���ļ�
	{
		videoCapture[i].open(fileName[i]);											//ѭ������Ƶ
		if (!videoCapture[i].isOpened())
		{
			cout << "Fail to open video " << fileName[i] << " !	" << endl;			//�����ʧ�������������Ϣ�������� false
			flag = false;
		}
		else
		{
			cout << fileName[i] << " has opened." << endl;							//���λ�ȡ��·��Ƶ�� �ֱ��� �� ֡��
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_WIDTH) << " * ";
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_HEIGHT) << "	";
			cout << videoCapture[i].get(CV_CAP_PROP_FRAME_COUNT) << " frames" << endl;
			videoCapture[i] >> src[i];												//����Ƶ�еĵ�һ֡�������Ա���� src[i] ��
		}
	}
	return flag;																	//���� true
}

bool VideoStitch::nextFrame()														//��ȡ��һ֡��Ƶ
{
	for (int i = 0; i < (int)fileName.size(); i++)
		videoCapture[i] >> src[i];
	// TODO: �ڴ˴����� return ���
	for (int i = 0; i < (int)fileName.size(); i++)
	{
		if (src[i].empty())															//�����һ֡Ϊ�գ�����Ƶ�Ѿ��������򷵻� false
			return false;
		else
			return true;
	}
}

VideoStitch & VideoStitch::release()												//�ͷ�������Ƶ
{
	for (int i = 0; i < (int)fileName.size(); i++)
	{
		videoCapture[i].release();
	}
	return *this;
	// TODO: �ڴ˴����� return ���
}

bool VideoStitch::stitchVideo(string savePath, bool isRebuild)								//ƴ�ӵ�֡���� �� �������Ƶ�����У��γ�һ����������Ƶ ���� Stitch ��Ŀ �е�StitchFrame��
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
				hasntRebuild = true; //��ʱ�޸�
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
		if (isOpened)														//������Ƶ�ļ�
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
		outPut << stitchFrame[(int)fileName.size() - 2].getResult();			//д�봦�����ͼƬ
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
	// TODO: �ڴ˴����� return ���
}

VideoStitch & VideoStitch::stitchVideo2(string savePath)						//��ȡ��Ƶ��ƴ�ӵ�֡ͼƬ������Stitcher���ƴ�ӷ������޷��������Ƶ
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
	// TODO: �ڴ˴����� return ���
}



//VideoStitch & VideoStitch::setResult(Mat mat)
//{
//	result[0] = mat;
//	return *this;
//	// TODO: �ڴ˴����� return ���
//}
