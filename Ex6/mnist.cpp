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

// Magic number
const int labelMagicNumber = 2049;
const int imageMagicNumber = 2051;

// Convert char array of size 4 to integer.
int charToInt(const unsigned char number[4]) {
    int ret = number[0];
    for ( int i = 1; i < 4; ++i ) {
        ret = (ret << 8) + number[i];
    }
    return ret;
}

// Read raw data into Mat
Mat readData(fstream& fs, int number, int size) {
    int totalSize = number * size;
    unsigned char* tmp = new unsigned char[totalSize];
    fs.read((char*)tmp, totalSize);

    Mat ret(number, size, CV_8UC1, tmp);
    delete [] tmp;

    return ret;
}

// Read all the labels
int readLabel(const char* filePath, Mat& ret) {
    fstream fs;
    fs.open(filePath);

    if ( !fs.is_open() ) {
        cout << "Failed to open label file" << endl;
        return -1;
    }

    labelHeader header;
    fs.read((char*)(&header), sizeof(header));

    if ( charToInt(header.magicNumber) != labelMagicNumber ) {
        cout << "Error! The magic number of label file is not correct!" << endl;
        return -1;
    }

    int number = charToInt(header.number);
    
    // Labels occupy only one byte
    ret = readData(fs, number, 1);

    return 0;
}

// Ref: http://yann.lecun.com/exdb/mnist/
// Ref: http://blog.csdn.net/sheng_ai/article/details/23267039
int main() {
    char trainingLabel[100] = "MNIST/train-labels.idx1-ubyte";

    Mat tlMat;
    readLabel(trainingLabel, tlMat);
    cout << tlMat;
    
    return 0;
}
