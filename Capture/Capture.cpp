// Capture.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CamCapture.h"

int main(int argc, char *argv[])
{	
	CamCapture capture;
	capture.capture(0, 1280, 720, 25.0, "camVideo.avi");
    return 0;
}

