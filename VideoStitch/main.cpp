// main.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "VideoStitch.h"

int main()
{
	VideoStitch videoStitch;
	videoStitch.readVideo("../Capture/Video_2017_12_22_200623", 2);
    return 0;
}

