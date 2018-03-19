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


bool CamCapture::capture(int  camNum, int width, int heigth, double fps, char saveDirName[])
{
	VideoCapture *cam[camMaxNum];
	Size videoResolution[camMaxNum];
	VideoWriter output[camMaxNum];
	StitchFrame stitchFrame;
	char fileName[80];
	int count[camMaxNum] = { 0 }, openNum = 0;
	bool isStart = false;
	Mat frameImg[camMaxNum];
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
			cout << "Exit! " << endl;
			for (int index = 0; index < i; index++)
			{
				cam[index]->release();
			}
			return false;
		}
	}
	waitKey(1000);

	while (true)
	{
		if (char(waitKey(1)) == ' ')				//����<sp> ��ʼ¼��
		{
			isStart = true;
			cout << "Start Record !" << endl;
		}
		for (int i = 0; i < camNum; i++)					//��ʾʵʱ�ɼ����Ļ���
		{
			*cam[i] >> frameImg[i];
			sprintf(fileName, "%s\\Cam%d.avi", saveDirName, i);
			namedWindow(fileName, WINDOW_KEEPRATIO);
			imshow(fileName, frameImg[i]);
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
		if (count[0] == 150)							//��������X֡��ֹͣ¼�񣬲�����
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
			stitchFrame.setSRC_L(frameImg[0]);
			stitchFrame.setSRC_R(frameImg[1]);
			stitchFrame.findH("preViewH.xml", STITCH_SIFT, true);
			namedWindow("outimg", WINDOW_KEEPRATIO);
			stitchFrame.show("outimg");
			namedWindow("preView", WINDOW_KEEPRATIO);
			imshow("preView", stitchFrame.stitch(20));


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
	}
	return true;
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
	Mat roi(panorama, Rect(0, 0,srcImage1.cols, srcImage1.rows));
	srcImage1.copyTo(roi);
}