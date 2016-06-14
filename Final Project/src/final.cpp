#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <fstream>
#include <iomanip>
#include "skewCorrection.cpp"
#include "segmentation.cpp"

using namespace cv;
using namespace std;

// Ref: http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_gcc_cmake/linux_gcc_cmake.html#linux-gcc-usage
int main(int argc, char* argv[]) {
    if ( argc != 2 ) {
        cout << "Please specify the path of the photo you want to test!" << endl;
        return -1;
    }

    Mat srcImg, deskewedImg, grayImg;
    srcImg = imread(argv[1]);
    if ( !srcImg.data ) {
        cout << "No image data" << endl;
        return -1;
    }

    // Ex3 skewCorrection.
    deskewedImg = skewCorrection(srcImg);

    // Convert to CV_8UC1
    cvtColor(deskewedImg, grayImg, CV_BGR2GRAY);

    // Segmentation
    vector<vector<Mat> > segmentedImg;
    segmentation(grayImg, segmentedImg);

    // Other processings
    vector<vector<Mat> > retImg;
    retImg = addBorder(segmentedImg);
    
    adjustStyle(retImg);

    const bool outputImg = 0;
    if ( outputImg ) {
        fstream fs("./result/pixels.csv", ios::app);
        for ( int i = 0; i < retImg.size(); ++i ) {
            for ( int j = 0; j < retImg[i].size(); ++j ) {

                char imgName[40];
                char imageNumber = argv[1][strlen(argv[1]) - 5];
                // http://stackoverflow.com/questions/347132/append-an-int-to-char
                sprintf(imgName, "./result/croppedImg/Image%c %d %d.jpg", imageNumber, i, j);

                cout << imgName << endl;
                //imshow(imgName, retImg[i][j]);
                imwrite(imgName, retImg[i][j]);
                fs << setfill('0') << setw(2) << imageNumber 
                   << setfill('0') << setw(2) << i 
                   << setfill('0') << setw(2) << j << " ";
                   
                // Write raw data.
                for ( int k = 0; k < retImg[i][j].rows; ++k ) {
                    for ( int l = 0; l < retImg[i][j].cols; ++l ) {
                        fs << (int) retImg[i][j].at<uchar>(k, l) << " ";
                    }
                }
                fs << endl;
            }
        }
    }

    return 0;
}
