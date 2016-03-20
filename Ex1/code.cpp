#include "CImg.h"
#include<iostream>
using namespace std;
using namespace cimg_library;

int main() {
    // Rotate and Transfrom
    CImg<unsigned char> srcImg("1.bmp");

    float width = srcImg.width();
    float height = srcImg.height();

    float angle, zoom;
    cout << "Please enter the degree you want to rotate(clockwise) as well as the zoom factor: ";
    cin >> angle >> zoom;
    
    /* If interpolation_type is 2, namely cubic, when the zoom factor is less than 1, the image rendered
     * will be quite strange.
     */
    CImg<unsigned char> rotatedImg = srcImg.get_rotate(angle, width / 2, height / 2, zoom, 2, 1);
    // rotatedImg.resize(20, 20);

    rotatedImg.display("Rotate & Transfrom");

    // Draw
    CImg<unsigned char> drawImg("3.bmp");

    const unsigned char blue[] = {0, 0, 255};
    drawImg.draw_rectangle(100, 100, 0, 300, 400, 0, blue, 1);
    drawImg.draw_triangle(500, 500, 600, 600, 700, 500, blue, 0.5);
    drawImg.draw_circle(800, 200, 70, blue, 0.75);
    drawImg.display("Draw");

    return 0;
}
