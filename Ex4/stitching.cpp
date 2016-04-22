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
        srcImg = imread(argv[i + 1], CV_LOAD_IMAGE_GRAYSCALE);
        if ( srcImg.empty() ) {
            cout << "Error! Failed to load " << argv[i + 1] << endl;
            return -1;
        }
        vsrcImg.push_back(srcImg);
    }


    /* SIFT */
    const int numberOfKp = 500;
    SiftFeatureDetector detector(numberOfKp);
    Mat outImg1;
    Mat outImg2;
    vector<KeyPoint> kp1;
    vector<Point2f> pt1;
    vector<KeyPoint> kp2;
    vector<Point2f> pt2;

    detector.detect(vsrcImg[0], kp1);
    drawKeypoints(vsrcImg[0], kp1, outImg1);
    detector.detect(vsrcImg[1], kp2);
    drawKeypoints(vsrcImg[1], kp2, outImg2);

    // Ref: http://docs.opencv.org/trunk/d2/d29/classcv_1_1KeyPoint.html#gsc.tab=0
    vector<KeyPoint>::iterator it;
    for( it = kp1.begin(); it != kp1.end(); ++it ) {
        pt1.push_back(it->pt);
    }
    for( it = kp2.begin(); it != kp2.end(); ++it ) {
        pt2.push_back(it->pt);
    }


    /* Descriptors */
    SiftDescriptorExtractor extractor;
    Mat descriptors1, descriptors2;

    extractor.compute(vsrcImg[0], kp1, descriptors1);
    extractor.compute(vsrcImg[1], kp2, descriptors2);


    /* Match descriptors */
    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);


    /* Filter out good matches */
    double max_dist = 0;
    double min_dist = 100;
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


    Mat matchedImg;
    drawMatches(vsrcImg[0], kp1, vsrcImg[1], kp2, good_matches, matchedImg,
                Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    /*
    Mat homography = findHomography(pt2, pt1, CV_RANSAC);
    //vector<Point2f> transformedPt;
    //perspectiveTransform(pt2, transformedPt, homography);

    Mat perspectiveImg;
    warpPerspective(outImg2, perspectiveImg, homography, perspectiveImg.size());
    */

    imshow("Img", matchedImg);
    waitKey(0);
    
    return 0;
}
