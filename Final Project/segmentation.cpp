#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat segmentation(Mat grayImg) {
    Mat binarizedImg;
    
    adaptiveThreshold(grayImg, binarizedImg, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 35, 27);

    int frequency[binarizedImg.cols];
    memset(frequency, 0, sizeof(frequency));

    // Calculate the frequency of white pixels for each col.
    cout << binarizedImg.rows << " " << binarizedImg.cols << endl;

    for ( int j = 0; j < binarizedImg.rows; ++j ) {
        for ( int i = 0; i < binarizedImg.cols; ++i ) {
            if ( binarizedImg.at<uchar>(j, i) == 255 ) {
                ++frequency[i];
            }
        }
    }

    for ( int i = 0; i < binarizedImg.cols; ++i ) { 
        cout << frequency[i] << " ";
    }
    cout << endl << endl;


    // Region segmentation
    const int valueThreshold = 2;
    const int timesThreshold = 5;

    int contLowTimes[binarizedImg.cols];
    memset(contLowTimes, 0, sizeof(contLowTimes));
    contLowTimes[0] = 1;
    for ( int i = 1; i < binarizedImg.cols; ++i ) { 
        if ( frequency[i] <= valueThreshold ) {
            contLowTimes[i] = contLowTimes[i - 1] + 1;
        } else {
            contLowTimes[i] = 0;
        }
    }

    for ( int i = 0; i < binarizedImg.cols - 1; ++i ) {
        if ( contLowTimes[i] <= timesThreshold && contLowTimes[i + 1] == 0 ) {
            for ( int j = i; j >= 0; --j ) {
                if ( contLowTimes[j] == 0 ) {
                    break;
                }
                contLowTimes[j] = 0;
            }
        }
    }

    for ( int i = 0; i < binarizedImg.cols; ++i ) { 
        cout << contLowTimes[i] << " ";
    }
    cout << endl << endl;

    vector<int> pos;
    for ( int i = 1; i < binarizedImg.cols; ++i ) {
        if ( contLowTimes[i] == 0 && contLowTimes[i - 1] != 0 ) {
            pos.push_back(i);
        } else if ( contLowTimes[i] != 0 && contLowTimes[i - 1] == 0 ) {
            pos.push_back(i - 1);
        }
    }

    for ( int i = 0; i < pos.size(); ++i ) {
        cout << pos[i] << " ";
    }
    cout << endl << endl;
    
    return binarizedImg;
}