#pragma once

#include "stdafx.h"


class CamCapture
{

private:
	//VideoCapture videoCapture(0);
public:
	CamCapture& videoSave();
	CamCapture& capture(int camNum, int width, int heigth, double fps, string savePath);

};
