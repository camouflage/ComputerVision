#ifndef SHEADER
#define SHEADER

#include "otherProcessing.h"
#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;

void stripSegmentation(Mat grayImg, Mat* retImg, int& size);
bool cmp(Rect a, Rect b);
void characterSegmentation(Mat* srcImg, int numberOfStrips, vector<vector<Mat> >& retImg);
void segmentation(Mat grayImg, vector<vector<Mat> >& retImg);

#endif