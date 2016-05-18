#include <iostream>
#include <cstdio>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <fstream>

using namespace cv;
using namespace std;

// The type must be unsigned, otherwise error may occur while reading number.
struct labelHeader {
    unsigned char magicNumber[4];
    unsigned char number[4];
};

// Image header, which is the first 16 bytes of the file.
struct imageHeader {
    unsigned char magicNumber[4];
    unsigned char number[4];
    unsigned char rows[4];
    unsigned char cols[4];
};

// Magic number
const int labelMagicNumber = 2049;
const int imageMagicNumber = 2051;

// Path length
const int pathLength = 100;

// Convert char array of size 4 to integer.
int charToInt(const unsigned char number[4]) {
    int ret = number[0];
    for ( int i = 1; i < 4; ++i ) {
        ret = (ret << 8) + number[i];
    }
    return ret;
}

// Read in raw data and save to Mat
Mat readData(fstream& fs, int number, int size) {
    int totalSize = number * size;
    unsigned char* tmp = new unsigned char[totalSize];
    fs.read((char*)tmp, totalSize);

    Mat ret(number, size, CV_8UC1, tmp);
    delete [] tmp;

    return ret;
}

// Read in all the labels
void readLabel(const char* filePath, Mat& ret) {
    fstream fs;
    fs.open(filePath);

    if ( !fs.is_open() ) {
        cout << "Failed to open label file" << endl;
        exit(1);
    }

    labelHeader header;
    fs.read((char*)(&header), sizeof(header));

    if ( charToInt(header.magicNumber) != labelMagicNumber ) {
        cout << "Error! The magic number of label file is not correct!" << endl;
        exit(1);
    }

    int number = charToInt(header.number);
    
    // Each label occupies only one byte
    ret = readData(fs, number, 1);
}

// Read in all the images
void readImage(const char* filePath, Mat& ret) {
    fstream fs;
    fs.open(filePath);

    if ( !fs.is_open() ) {
        cout << "Failed to open image file" << endl;
        exit(1);
    }

    imageHeader header;
    fs.read((char*)(&header), sizeof(header));

    if ( charToInt(header.magicNumber) != imageMagicNumber ) {
        cout << "Error! The magic number of image file is not correct!" << endl;
        exit(1);
    }

    int number = charToInt(header.number);
    int rows = charToInt(header.rows);
    int cols = charToInt(header.cols);

    // Each image occupies rows * cols bytes
    ret = readData(fs, number, rows * cols);
}

// Ref: http://yann.lecun.com/exdb/mnist/
// Ref: http://blog.csdn.net/sheng_ai/article/details/23267039
int main() {
    char trainingLabel[pathLength] = "MNIST/train-labels.idx1-ubyte";
    char testLabel[pathLength] = "MNIST/t10k-labels.idx1-ubyte";
    char trainingImage[pathLength] = "MNIST/train-images.idx3-ubyte";
    char testImage[pathLength] = "MNIST/t10k-images.idx3-ubyte";

    Mat trainingLabelMat, testLabelMat, trainingImageMat, testImageMat;

    readLabel(trainingLabel, trainingLabelMat);
    readLabel(testLabel, testLabelMat);
    readImage(trainingImage, trainingImageMat);
    readImage(testImage, testImageMat);

    /* 
    // Ref: http://stackoverflow.com/questions/16312904/how-to-write-a-float-mat-to-a-file-in-opencv
    FileStorage trainingImgFile("trainingImgFile.csv", FileStorage::WRITE);
    trainingImgFile << "Mat" << trainingImageMat;
    */
    
    return 0;
}
