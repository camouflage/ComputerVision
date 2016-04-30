#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;


int main(int argc, char* argv[]) {
    // Read in image
    if ( argc < 2 ) {
        cout << "Please specify the path of the image" << endl;
        return -1;
    }

    Mat srcImg, grayImg;
    srcImg = imread(argv[1]);
    if ( srcImg.empty() ) {
        cout << "Error! Failed to load " << argv[1] << endl;
        return -1;
    }

    cvtColor(srcImg, grayImg, CV_BGR2GRAY);

    /*
    // Calculate grayscale histogram.
    // Ref: http://docs.opencv.org/2.4/modules/imgproc/doc/histograms.html?highlight=calchist#calchist
    // Ref: http://opencvexamples.blogspot.com/2013/10/histogram-calculation.html
    MatND hist;
    int nbins = 256;
    int hsize[] = {nbins};
    float range[] = {0, 256};
    const float* ranges[] = {range};
    calcHist(&grayImg, 1, 0, Mat(), hist, 1, hsize, ranges);

    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double) hist_w / nbins);
 
    Mat histImg(hist_h, hist_w, CV_8UC1, Scalar(0,0,0));
    normalize(hist, hist, 0, histImg.rows, NORM_MINMAX, -1, Mat() );
     
    for( int i = 1; i < nbins; ++i ) {
        line(histImg, Point( bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1)) ),
                       Point( bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
                       Scalar(255, 0, 0), 2, 8, 0);
    }
    imshow("final", histImg);
    */

    imshow("gray", grayImg);

    // Calculate the frequency of each value
    // Ref: http://stackoverflow.com/questions/21287082/accessing-certain-pixels-intensity-valuegrayscale-image-in-opencv
    const int L = 256;
    double frequency[L];
    memset(frequency, 0, sizeof(frequency));
    for ( int j = 0; j < grayImg.rows; ++j ) {
        for ( int i = 0; i < grayImg.cols; ++i ) {
            ++frequency[grayImg.at<uchar>(j, i)];
        }
    }

    const int total = grayImg.rows * grayImg.cols;
    for ( int i = 0; i < L; ++i ) {
        //cout << i << " " << frequency[i] << endl;
        frequency[i] /= total; 
    }

    // Calculate cdf for p and i * p
    double culp[L];
    double culip[L];
    culp[0] = frequency[0];
    culip[0] = 0;
    for ( int i = 1; i < L; ++i ) {
        culp[i] = culp[i - 1] + frequency[i];
        culip[i] = culip[i - 1] + i * frequency[i];
    }

    // Find k s.t. max sigmaB
    int maxSigma = 0;
    int atPos;
    for ( int k = 1; k < L - 1; ++k ) {
        // Calculate m1, m2, mg, sigmaB
        double m1 = culip[k] / culp[k];
        double m2 = (culip[L - 1] - culip[k]) / (culp[L - 1] - culp[k]);
        double mg = culip[L - 1];
        double sigmaB = pow(mg * culp[k] - culip[k], 2) / (culp[k] * (1 - culp[k]));
        if ( sigmaB > maxSigma ) {
            maxSigma = sigmaB;
            atPos = k;
        }
    }

    //cout << atPos << endl;
    for ( int j = 0; j < grayImg.rows; ++j ) {
        for ( int i = 0; i < grayImg.cols; ++i ) {
            if ( grayImg.at<uchar>(j, i) >= atPos ) {
                grayImg.at<uchar>(j, i) = 255;
            } else {
                grayImg.at<uchar>(j, i) = 0;
            }
        }
    }

    imshow("final", grayImg);
    waitKey(0);

    return 0;
}
