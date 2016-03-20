#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Ref: http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_gcc_cmake/linux_gcc_cmake.html#linux-gcc-usage
int main(int argc, char* argv[]) {
    if ( argc != 2 ) {
        cout << "Please specify the path of the photo you want to test!" << endl;
        return -1;
    }

    Mat srcImg, dstImg, srcGray, edge;
    srcImg = imread(argv[1]);
    if ( !srcImg.data ) {
        cout << "No image data" << endl;
        return -1;
    }

    // Ref: http://docs.opencv.org/2.4/modules/imgproc/doc/geometric_transformations.html#resize
    resize(srcImg, srcImg, Size(), 0.2, 0.2);

    // Ref: http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html
    cvtColor(srcImg, srcGray, CV_BGR2GRAY);

    blur(srcGray, edge, Size(3, 3));

    int lowThreshold = 125;
    int ratio = 3;
    int kernel_size = 3;
    Canny(edge, edge, lowThreshold, lowThreshold * ratio, kernel_size);

    dstImg.create(srcImg.size(), srcImg.type());
    dstImg = Scalar::all(0);

    srcImg.copyTo(dstImg, edge);

    namedWindow("Edge", CV_WINDOW_AUTOSIZE);
    imshow("Display Image", dstImg);

    waitKey(0);
    return 0;
}