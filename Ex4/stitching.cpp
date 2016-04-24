#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;


// Ref: http://blog.csdn.net/pi9nc/article/details/9251387
void blending(Mat left, Mat right, vector<Point2f>& leftPt) {
    int xLeft = min(leftPt[0].x, leftPt[1].x);
    double width = left.cols - xLeft;
    double alpha;
    const int blackThreshold = 10;

    for ( int i = 0; i < right.rows; ++i ) {
        for ( int j = xLeft; j < left.cols; ++j ) {
            int b = right.at<Vec3b>(j, i)[0];
            int g = right.at<Vec3b>(j, i)[1];
            int r = right.at<Vec3b>(j, i)[2];
            if ( b < blackThreshold && g < blackThreshold && r < blackThreshold ) {
                alpha = 1;
            } else {
                alpha = (width - (j - xLeft)) / width;
                //alpha = 0;
            }

            left.at<Vec3b>(j, i)[0] = left.at<Vec3b>(j, i)[0] * alpha + right.at<Vec3b>(j, i)[0] * (1 - alpha);
            left.at<Vec3b>(j, i)[1] = left.at<Vec3b>(j, i)[1] * alpha + right.at<Vec3b>(j, i)[1] * (1 - alpha);
            left.at<Vec3b>(j, i)[2] = left.at<Vec3b>(j, i)[2] * alpha + right.at<Vec3b>(j, i)[2] * (1 - alpha);
        }
    }
}

// Stitching
void stitch(vector<Point2f>& matchedPt0, vector<Point2f>& matchedPt1, Mat img0, Mat img1) {
    Mat homography = findHomography(matchedPt1, matchedPt0, CV_RANSAC);
    Mat perspectiveImg;
    warpPerspective(img1, perspectiveImg, homography, Size(img0.cols + img1.cols, img0.rows));
    
    // Left corner points
    vector<Point2f> leftCorner;
    vector<Point2f> transformedLeft;
    Point2f topLeft(0, 0);
    Point2f bottomLeft(0, img1.rows);
    leftCorner.push_back(topLeft);
    leftCorner.push_back(bottomLeft);

    perspectiveTransform(leftCorner, transformedLeft, homography);

    // blending(img0, perspectiveImg, transformedLeft);

    Mat final(img0.rows, img0.cols + img1.cols, CV_8UC3);
    Mat right(final, Rect(0, 0, perspectiveImg.cols, perspectiveImg.rows));
    perspectiveImg.copyTo(right);
    Mat left(final, Rect(0, 0, img0.cols, img0.rows));
    img0.copyTo(left);

    imshow("l", left);
    imshow("r", right);
    imshow("final", final);
}


int main(int argc, char* argv[]) {
    /* Read in image */
    if ( argc < 3 ) {
        cout << "Please specify the path of the photo you want to stitch!" << endl;
        return -1;
    }

    vector<Mat> vsrcImg;
    Mat srcImg;
    for ( int i = 0; i < argc - 1; ++i ) {
        // Load as gray scale.
        srcImg = imread(argv[i + 1]);
        if ( srcImg.empty() ) {
            cout << "Error! Failed to load " << argv[i + 1] << endl;
            return -1;
        }
        vsrcImg.push_back(srcImg);
    }


    /* SIFT */
    const int numberOfKp = 100;
    SiftFeatureDetector detector(numberOfKp);
    Mat outImg0;
    Mat outImg1;
    vector<KeyPoint> kp1;
    vector<Point2f> pt1;
    vector<KeyPoint> kp2;
    vector<Point2f> pt2;

    cvtColor(vsrcImg[0], outImg0, CV_BGR2GRAY);
    cvtColor(vsrcImg[1], outImg1, CV_BGR2GRAY);
    detector.detect(outImg0, kp1);
    //drawKeypoints(vsrcImg[0], kp1, outImg0);
    detector.detect(outImg1, kp2);
    //drawKeypoints(vsrcImg[1], kp2, outImg1);

    /*
    // Ref: http://docs.opencv.org/trunk/d2/d29/classcv_1_1KeyPoint.html#gsc.tab=0
    vector<KeyPoint>::iterator it;
    for( it = kp1.begin(); it != kp1.end(); ++it ) {
        pt1.push_back(it->pt);
    }
    for( it = kp2.begin(); it != kp2.end(); ++it ) {
        pt2.push_back(it->pt);
    }
    */


    /* Descriptors */
    SiftDescriptorExtractor extractor;
    Mat descriptors1, descriptors2;

    extractor.compute(outImg0, kp1, descriptors1);
    extractor.compute(outImg1, kp2, descriptors2);


    /* Match descriptors */
    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);


    /* Filter out good matches */
    double max_dist = 0;
    double min_dist = 200;
    for ( int i = 0; i < descriptors1.rows; i++ ) {
        double dist = matches[i].distance;
        if ( dist < min_dist ) {
            min_dist = dist;
        }
        if ( dist > max_dist ) {
            max_dist = dist;
        }
    }

    vector<DMatch> good_matches;
    for ( int i = 0; i < descriptors1.rows; i++ ) { 
        if ( matches[i].distance < 3 * min_dist ) { 
            good_matches.push_back(matches[i]);
        }
    }


    /* Get matched points */
    // Ref: http://stackoverflow.com/questions/8436647/opencv-getting-pixel-coordinates-from-feature-matching
    vector<Point2f> matchedPt0;
    vector<Point2f> matchedPt1;
    for ( int i = 0; i < good_matches.size(); ++i ) {
        int idx1 = good_matches[i].queryIdx;
        int idx2 = good_matches[i].trainIdx;
        matchedPt0.push_back(kp1[idx1].pt);
        matchedPt1.push_back(kp2[idx2].pt);
    }

    /*
    for ( int i = 0; i < matchedPt0.size(); ++i ) {
        cout << matchedPt0[i] << " " << matchedPt1[i] << endl;
    }
    */

    Mat matchedImg;
    drawMatches(vsrcImg[0], kp1, vsrcImg[1], kp2, good_matches, matchedImg,
                Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);


    /* Homography */
    int reverseCount = 0;
    for ( int i = 0; i < matchedPt0.size(); ++i ) {
        if ( matchedPt1[i].x > matchedPt0[i].x ) {
            ++reverseCount;
        }
    }

    
    const double reverseThreshold = 0.75;
    if ( reverseCount > reverseThreshold * matchedPt0.size() ) {
        cout << "Reverse: " << endl;
        stitch(matchedPt1, matchedPt0, vsrcImg[1], vsrcImg[0]);
    } else {
        stitch(matchedPt0, matchedPt1, vsrcImg[0], vsrcImg[1]);
    }

    //imshow("match", matchedImg);
    waitKey(0);
    return 0;
}
