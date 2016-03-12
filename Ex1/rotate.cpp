/* Ref
http://blog.csdn.net/stephen_yin/article/details/7762069
https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html
http://techhao.blogspot.sg
*/
#include "CImg.h"
using namespace cimg_library;

int main() {
    CImg<unsigned char> srcImg("1.bmp");
    srcImg.display("new");
    return 0;
}