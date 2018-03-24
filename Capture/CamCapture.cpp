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
	Size videoResolution = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));		//��ȡ��Ƶ�ֱ���
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
	// TODO: �ڴ˴����� return ���
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

	for (int i = 0; i < camNum; i++)					//��������ͷ��������������ͷ
	{

		cam[i] = new VideoCapture(i);
		cam[i]->set(CV_CAP_PROP_FRAME_WIDTH, width);
		cam[i]->set(CV_CAP_PROP_FRAME_HEIGHT, heigth);
		cam[i]->set(CV_CAP_PROP_FPS, fps);

		//cam[i]->set(CV_CAP_PROP_BRIGHTNESS, 128);			//���� 1
		//cam[i]->set(CV_CAP_PROP_CONTRAST, 50);			//�Աȶ� 40
		//cam[i]->set(CV_CAP_PROP_SATURATION, 50);			//���Ͷ� 50
		//cam[i]->set(CV_CAP_PROP_HUE, 50);					//ɫ�� 50
		//cam[i]->set(CV_CAP_PROP_EXPOSURE, -2);			//�ع� 50
		//cam[i]->set(CV_CAP_PROP_FOCUS, 50);

		videoResolution[i] = Size((int)cam[i]->get(CV_CAP_PROP_FRAME_WIDTH), (int)cam[i]->get(CV_CAP_PROP_FRAME_HEIGHT));		//��ȡ��Ƶ�ֱ���

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
		if (char(waitKey(1)) == ' ')				//����<sp> ��ʼ¼��
		{
			isStart = true;
			cout << "Start Record !" << endl;
		}
		for (int i = 0; i < camNum; i++)					//��ʾʵʱ�ɼ����Ļ���
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

					//cam[i]->set(CV_CAP_PROP_BRIGHTNESS, 128);			//���� 1
					//cam[i]->set(CV_CAP_PROP_CONTRAST, 50);			//�Աȶ� 40
					//cam[i]->set(CV_CAP_PROP_SATURATION, 50);			//���Ͷ� 50
					//cam[i]->set(CV_CAP_PROP_HUE, 50);					//ɫ�� 50
					//cam[i]->set(CV_CAP_PROP_EXPOSURE, -2);			//�ع� 50
					//cam[i]->set(CV_CAP_PROP_FOCUS, 50);

					videoResolution[*iter] = Size((int)cam[*iter]->get(CV_CAP_PROP_FRAME_WIDTH), (int)cam[*iter]->get(CV_CAP_PROP_FRAME_HEIGHT));		//��ȡ��Ƶ�ֱ���
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

			if (!frameImg[i].empty() && isStart)			//����Ѿ��ɼ������� ���� ��ʼ¼����ʼ�����汣�浽ָ����Ŀ¼��
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
		if (count[0] == 48)							//��������X֡��ֹͣ¼�񣬲�����
		{
			for (int i = 0; i < camNum; i++)
			{
				cam[i]->release();
				sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
				destroyWindow(fileName);
			}
			break;
		}

		if (char(waitKey(1)) == 'a')						//������� a �����ҵ����е�����ͼ���ӳ�����ƴ�ӣ�����ʾƴ�Ӻ�ͼ��ӳ����󱣴�Ϊ preViewH.xml
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
					hasntRebuild = true; //��ʱ�޸�
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
		if (char(waitKey(1)) == 'q')					//���� q ,�˳�¼���ͷ�������������ͷ���ر���������ͷ�Ĵ���
		{
			for (int i = 0; i < camNum; i++)
			{
				cam[i]->release();
				sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
				destroyWindow(fileName);
			}

			for (int i = 0; i < camNum; i++)			//���һ��д��Ӳ�̵�֡��
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
	int Num = int(Images.size());				//�õ�Vector������ͼƬ����
											//�趨������ЩͼƬ�Ĵ��ڴ�С�����ﶼ��BGR3ͨ����������Ҷȵ�ͨ������΢��һ���������д���Ϳ���
	//Mat Window(1280 * ((Num - 1) / rows + 1), 720 * rows, CV_8UC3, Scalar(0, 0, 0));
	Mat Window(Size(1280 * 3, 720 * 2), CV_8UC3, Scalar(0, 0, 0));
	Mat Std_Image;										//��ű�׼��С��ͼƬ
	Mat imageROI;										//ͼƬ��������
	Size Std_Size = Size(1280, 720);						//ÿ��ͼƬ��ʾ��С300*300
	int x_Begin = 0;
	int y_Begin = 0;
	for (int i = 0; i < Num; i++)
	{
		x_Begin = (i % rows)*Std_Size.width;			//ÿ��ͼƬ��ʼ����
		y_Begin = (i / rows)*Std_Size.height;
		//resize(Images[i], Std_Image, Std_Size, 0, 0, INTER_LINEAR);						//��ͼ����Ϊ��׼��С
																						//��������Window��
		imageROI = Window(Rect(x_Begin, y_Begin, Std_Size.width, Std_Size.height));
		Images[i].copyTo(imageROI);
	}
	dst = Window;
}




void stitch(vector<Mat> imgs, Mat& resultMat)
{
	bool Flag = true;
	// ����Stitcher��
	Stitcher stitcher = Stitcher::createDefault(Flag);					//OpenCV�Դ���Stitch����
	Stitcher::Status status = stitcher.stitch(imgs, resultMat);
	if (status != Stitcher::OK) {
		std::cout << "error" << std::endl;
	}
}
void stitch2(Mat& srcImage1, Mat& srcImage2, Mat& panorama)				//��һ�����Ƶ�ƴ�ӷ���
{
	// SURF����������
	Mat srcImage2Warped;
	// ��ʼ��SURF���������
	cv::Ptr<Feature2D> surf = xfeatures2d::SURF::create();

	// �ؼ��㼰����������������
	vector<cv::KeyPoint> kPointMat, kPointMat2;;
	cv::Mat desMat1, desMat2;

	surf->detectAndCompute(srcImage1, Mat(), kPointMat, desMat1);
	surf->detectAndCompute(srcImage2, Mat(), kPointMat2, desMat2);

	// FLANN �ؼ���ƥ��
	// FLANN ��ʼ�� RANSACһ���� ���������
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(desMat1, desMat2, matches);
	double max_dist = 0, min_dist = 100;
	// �������������С�������
	for (int i = 0; i < desMat1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	// ʹ��good �������������޶� 
	std::vector< DMatch > good_matches;
	for (int i = 0; i < desMat1.rows; i++)
	{
		if (matches[i].distance < 2 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	// ͼ��Ĺؼ���ƥ��
	std::vector<Point2f> srcImage1_matchedKPs;
	std::vector<Point2f> srcImage2_matchedKPs;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		srcImage1_matchedKPs.push_back(kPointMat[good_matches[i].queryIdx].pt);
		srcImage2_matchedKPs.push_back(kPointMat2[good_matches[i].trainIdx].pt);
	}
	// ����ͼ��1��ͼ��2��ӳ��
	Mat H = findHomography(Mat(srcImage2_matchedKPs),
		Mat(srcImage1_matchedKPs), CV_RANSAC);
	// ����任
	warpPerspective(srcImage2, srcImage2Warped, H, Size(srcImage2.cols * 2, srcImage2.rows), INTER_CUBIC);
	panorama = srcImage2Warped.clone();
	// ������
	Mat roi(panorama, Rect(0, 0, srcImage1.cols, srcImage1.rows));
	srcImage1.copyTo(roi);
}