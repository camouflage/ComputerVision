#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Store line info
struct myLine {
    double m; // Slope of the line
    double c; // Intercept of the line
    bool vertical; // True if the line is parallel to y-axis
};

/*
void myHoughLines(const Mat& img, vector<Vec2f>& lines, float rho, float theta, int threshold) {
    float irho = 1 / rho;
    int width = img.size().width;
    int height = img.size().height;  
}
*/

// Compare function based on theta
bool cmp(const Vec2f& a, const Vec2f& b) {
    return a[1] < b[1];
}

// Ref: http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_gcc_cmake/linux_gcc_cmake.html#linux-gcc-usage
int main(int argc, char* argv[]) {
    if ( argc != 2 ) {
        cout << "Please specify the path of the photo you want to test!" << endl;
        return -1;
    }

    Mat srcImg, dstImg, srcGray, srcBlur, edge;
    srcImg = imread(argv[1]);
    if ( !srcImg.data ) {
        cout << "No image data" << endl;
        return -1;
    }

    // Ref: http://docs.opencv.org/2.4/modules/imgproc/doc/geometric_transformations.html#resize
    resize(srcImg, srcImg, Size(), 0.2, 0.2);

    // Ref: http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html
    // Transfrom to gray scale.
    cvtColor(srcImg, srcGray, CV_BGR2GRAY);

    // Blur the image to erase some noise
    blur(srcGray, srcBlur, Size(3, 3));

    // Set threshold for Canny
    int lowThreshold = 125;
    int highThreshold = 500;
    int kernel_size = 3;
    Canny(srcBlur, edge, lowThreshold, highThreshold, kernel_size);

    // Ref: http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html
    // Set parameters for hough transform
    float rho = 1;
    float theta = CV_PI / 100;
    int threshold = 97;
    vector<Vec2f> lines;
    vector<myLine> mls;

    HoughLines(edge, lines, rho, theta, threshold, 0, 0);
    // myHoughLines(edge, lines, rho, theta, threshold);
    
    // Sort lines based on theta
    sort(lines.begin(), lines.end(), cmp);

    dstImg.create(srcImg.size(), srcImg.type());
    dstImg = Scalar::all(0);

    for ( size_t i = 0; i < lines.size(); i++ ) {
        float rho = lines[i][0], theta = lines[i][1];

        // Eliminate duplicates.
        float deltaRho = 50;
        float deltaTheta = 0.25;
        // cout << rho << " " << theta << endl;
        if ( i > 0 && abs(rho - lines[i - 1][0]) < deltaRho && abs(theta - lines[i - 1][1]) < deltaTheta ) {
            continue;
        }

        // Produce two lines on the line
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        line(dstImg, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);

        struct myLine ml;
        // Deal with lines parallel to y-axis.
        if ( b == 0 ) {
            ml.vertical = 1;
            ml.c = rho;
            printf("Line equation: x = %lf\n", rho);
        } else {
            ml.vertical = 0;
            // m for slope and c for y-intercept
            double m = -a / b, c = rho / b;
            ml.m = m;
            ml.c = c;
            printf("Line equation: y = %lfx + %lf\n", m ,c);
        }
        
        mls.push_back(ml);
    }

    // Calculate line-line intersection.
    // Ref: https://en.wikipedia.org/wiki/Line–line_intersection
    for ( int i = 0; i < 2; ++i ) {
        for ( int j = 2; j < 4; ++j ) {
            double a = mls[i].m, b = mls[j].m;
            double c = mls[i].c, d = mls[j].c;
            double x, y;
            if ( mls[i].vertical == 1 ) {
                x = c;
                y = b * c + d;
            } else {
                x = (d - c) / (a - b);
                y = (a * d - b * c) / (a - b);
            }
            printf("Point: (%lf, %lf)\n", x, y);
        }
    }

    // namedWindow("Edge", CV_WINDOW_AUTOSIZE);
    // imshow("Display Image", edge);
    imshow("Display Image", dstImg);

    waitKey(0);
    return 0;
}
