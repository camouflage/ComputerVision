#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
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
    
    // ConverT to CV_8UC1
    cvtColor(deskewedImg, grayImg, CV_BGR2GRAY);

    // http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold
    

    /*
    int erosion_size = 1;  
    Mat element = getStructuringElement(cv::MORPH_CROSS,
              cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
              cv::Point(erosion_size, erosion_size) );
    //dilate(binarizedImg, binarizedImg, Mat());
    erode(binarizedImg, binarizedImg, element);
    */
    Mat segmentedImg[5];
    int numberOfSubRegions;
    segmentation(grayImg, segmentedImg, numberOfSubRegions);

    for ( int i = 0; i < numberOfSubRegions; ++i ) {
        char imgName[10];
        // http://stackoverflow.com/questions/347132/append-an-int-to-char
        sprintf(imgName, "Image%d", i);
        imshow(imgName, segmentedImg[i]);
    }

    waitKey(0);
    
    return 0;
}
