#include"otherProcessing.h"

// Rotate image
Mat rotation(Mat srcImg, double angle) {
    Mat tempImg;
    float radian = (float)(angle / 180.0 * CV_PI);

    int uniSize = (int)(max(srcImg.cols, srcImg.rows) * 1.414);
    int dx = (int)(uniSize - srcImg.cols) / 2;
    int dy = (int)(uniSize - srcImg.rows) / 2;
    copyMakeBorder(srcImg, tempImg, dy, dy, dx, dx, BORDER_CONSTANT);

    Point2f center((float)(tempImg.cols / 2), (float)(tempImg.rows / 2));
    Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);

    warpAffine(tempImg, tempImg, affine_matrix, tempImg.size());

    float sinVal = fabs(sin(radian));
    float cosVal = fabs(cos(radian));
    Size targetSize((int)(srcImg.cols * cosVal + srcImg.rows * sinVal),
        (int)(srcImg.cols * sinVal + srcImg.rows * cosVal));


    int x = (tempImg.cols - targetSize.width) / 2;
    int y = (tempImg.rows - targetSize.height) / 2;
    Rect rect(x, y, targetSize.width, targetSize.height);
    tempImg = Mat(tempImg, rect);

    return tempImg;
}

void adjustStyle(vector<vector<Mat> >& srcImg) {
    for ( int i = 0; i < srcImg.size(); ++i ) {
        // cout << "Width and Height " << srcImg[i][0].cols << " " << srcImg[i][0].rows << endl;
        for ( int j = 0; j < srcImg[i].size(); ++j ) {
            // Resize to 28 * 28.
            resize(srcImg[i][j], srcImg[i][j], Size(28, 28));
            
            // Dilate
            int size = 1;
            Mat element = getStructuringElement(MORPH_RECT,
                                       Size(2 * size + 1, 2 * size + 1),
                                       Point(size, size));
            dilate(srcImg[i][j], srcImg[i][j], element);
            
            
            /*
            // Smooth
            blur(srcImg[i][j], srcImg[i][j], Size(1, 1));
            */

            /*
            cout << "Image " << i << " " << j << endl;

            for ( int k = 0; k < srcImg[i][j].rows; ++k ) {
                for ( int l = 0; l < srcImg[i][j].cols; ++l ) {
                    printf("%d ", srcImg[i][j].at<uchar>(k, l));
                }
                cout << endl;
            }
            */
        }
    }
}

vector<vector<Mat> > addBorder(vector<vector<Mat> > srcImg) {
    vector<vector<Mat> > retImg;
    for ( int i = 0; i < srcImg.size(); ++i ) {
        vector<Mat> partialRetImg;
        for ( int j = 0; j < srcImg[i].size(); ++j ) {
            int width = srcImg[i][j].cols;
            int height = srcImg[i][j].rows;
            Mat dstImg;

            // Binarized
            adaptiveThreshold(srcImg[i][j], dstImg, 255,
                ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 35, 27);

            if ( width < height ) {
                copyMakeBorder(dstImg, dstImg, 0, 0, (height - width) / 2, (height - width) / 2,
                    BORDER_CONSTANT, Scalar(0, 0, 0));
                partialRetImg.push_back(dstImg);
            } else {
                partialRetImg.push_back(srcImg[i][j]);
            }
        }

        retImg.push_back(partialRetImg);
    }

    return retImg;
}
