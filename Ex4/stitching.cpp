#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <set>

using namespace cv;
using namespace std;


void ransac(const vector<Point2f>& matchedPt0, const vector<Point2f>& matchedPt1,
            vector<Point2f>& bestPt0, vector<Point2f>& bestPt1) {

    const int iterations = 200;
    const int samples = 4;
    const int numberOfMatches = matchedPt0.size();
    int maxInliners = 0;

    for ( int it = 0; it < iterations; ++it ) {
        // Randomly choose samples
        set<int> index;
        for ( int i = 0; i < samples; ) {
            int r = rand() % numberOfMatches;
            if ( index.count(r) == 0 ) {
                index.insert(r);
                ++i;
            }
        }

        vector<Point2f> filtered0;
        vector<Point2f> filtered1;
        set<int>::iterator ite = index.begin();
        for ( ; ite != index.end(); ++ite ) {
            filtered0.push_back(matchedPt0[*ite]);
            filtered1.push_back(matchedPt1[*ite]);
        }

        // Compute H
        Mat homography = findHomography(filtered0, filtered1, 0);

        vector<Point2f> transformedMatched;
        // Transform pt0 to pt1 based on H
        perspectiveTransform(matchedPt0, transformedMatched, homography);

        // Count inliners
        const int filterThreshold = 3;
        int inliners = 0;
        for ( int i = 0; i < numberOfMatches; ++i ) {
            if ( norm(transformedMatched[i] - matchedPt1[i]) < filterThreshold ) {
                ++inliners;
                // cout << transformedMatched[i] << " " << matchedPt1[i] << endl;
            }
        }

        // Choose greatest inliners
        if ( inliners > maxInliners ) {
            maxInliners = inliners;
            bestPt0.clear();
            bestPt1.clear();
            for ( int i = 0; i < numberOfMatches; ++i ) {
                if ( norm(transformedMatched[i] - matchedPt1[i]) < filterThreshold ) {
                    bestPt0.push_back(matchedPt0[i]);
                    bestPt1.push_back(matchedPt1[i]);
                }
            }
        }
    }

    cout << "Max inliner ratio: " << maxInliners << " / " << numberOfMatches << endl;
}

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
Mat merge(const vector<Point2f>& matchedPt0, const vector<Point2f>& matchedPt1, Mat img0, Mat img1) {
    Mat homography = findHomography(matchedPt1, matchedPt0, 0);
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

    //blending(img0, perspectiveImg, transformedLeft);

    Mat finalImg(perspectiveImg.rows, img0.cols + img1.cols, CV_8UC3);
    Mat right(finalImg, Rect(0, 0, perspectiveImg.cols, perspectiveImg.rows));
    perspectiveImg.copyTo(right);
    Mat left(finalImg, Rect(0, 0, img0.cols, img0.rows));
    img0.copyTo(left);

    //imshow("l", left);
    //imshow("r", perspectiveImg);
    //imshow("finalImg", finalImg);
    //imwrite("./dataset1/temp.bmp", finalImg);

    return finalImg;
}


void stitch(Mat& srcImg0, Mat srcImg1) {
    // SIFT
    const int numberOfKp = 500;
    SiftFeatureDetector detector(numberOfKp);
    Mat outImg0;
    Mat outImg1;
    vector<KeyPoint> kp0;
    vector<KeyPoint> kp1;

    cvtColor(srcImg0, outImg0, CV_BGR2GRAY);
    cvtColor(srcImg1, outImg1, CV_BGR2GRAY);
    detector.detect(outImg0, kp0);
    //drawKeypoints(srcImg0, kp0, outImg0);
    detector.detect(outImg1, kp1);
    //drawKeypoints(srcImg1, kp1, outImg1);


    // Descriptors
    SiftDescriptorExtractor extractor;
    Mat descriptors1, descriptors2;
    extractor.compute(outImg0, kp0, descriptors1);
    extractor.compute(outImg1, kp1, descriptors2);

    // Match descriptors
    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);


    // Filter out good matches
    double max_dist = 0;
    double min_dist = 300;
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


    // Get good matched points
    // Ref: http://stackoverflow.com/questions/8436647/opencv-getting-pixel-coordinates-from-feature-matching
    vector<Point2f> matchedPt0;
    vector<Point2f> matchedPt1;
    for ( int i = 0; i < good_matches.size(); ++i ) {
        int idx1 = good_matches[i].queryIdx;
        int idx2 = good_matches[i].trainIdx;
        matchedPt0.push_back(kp0[idx1].pt);
        matchedPt1.push_back(kp1[idx2].pt);
    }
    cout << "Good matches ratio: " << good_matches.size() << " / " << matches.size() << endl;


    Mat matchedImg;
    drawMatches(srcImg0, kp0, srcImg1, kp1, good_matches, matchedImg,
                Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    // imshow("match", matchedImg);


    // RANSAC
    vector<Point2f> filteredPt0;
    vector<Point2f> filteredPt1;
    ransac(matchedPt0, matchedPt1, filteredPt0, filteredPt1);


    // Decide whether to switch
    int reverseCount = 0;
    for ( int i = 0; i < filteredPt0.size(); ++i ) {
        if ( filteredPt0[i].x < filteredPt1[i].x ) {
            ++reverseCount;
        }
    }

    cout << reverseCount << endl;

    const double reverseThreshold = 0.75;
    if ( reverseCount > reverseThreshold * filteredPt0.size() ) {
        //cout << "Reverse: " << endl;
        //srcImg0 = merge(matchedPt1, matchedPt0, srcImg1, srcImg0);
        srcImg0 = merge(filteredPt1, filteredPt0, srcImg1, srcImg0);
    } else {
        //srcImg0 = merge(matchedPt0, matchedPt1, srcImg0, srcImg1);
        srcImg0 = merge(filteredPt0, filteredPt1, srcImg0, srcImg1);
    }
}


int main(int argc, char* argv[]) {
    // Read in image
    if ( argc < 3 ) {
        cout << "Please specify the path of the photo you want to stitch!" << endl;
        return -1;
    }

    vector<Mat> vsrcImg;
    Mat srcImg;
    for ( int i = 0; i < argc - 1; ++i ) {
        srcImg = imread(argv[i + 1]);
        if ( srcImg.empty() ) {
            cout << "Error! Failed to load " << argv[i + 1] << endl;
            return -1;
        }

        resize(srcImg, srcImg, Size(), 0.25, 0.25);
        vsrcImg.push_back(srcImg);
    }

    Mat temp = vsrcImg[0];
    for ( int i = 1; i < argc - 1; ++i ) {
        cout << "Stitching image " << i << ":\n";
        stitch(temp, vsrcImg[i]);
    }
    
    imshow("ret", temp);
    imwrite("temp.bmp", temp);
    waitKey(0);
    return 0;
}
