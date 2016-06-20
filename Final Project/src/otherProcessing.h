#ifndef OPHEADER
#define OPHEADER

#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;

Mat rotation(Mat srcImg, double angle);
void adjustStyle(vector<vector<Mat> >& srcImg);
vector<vector<Mat> > addBorder(vector<vector<Mat> > srcImg);

#endif