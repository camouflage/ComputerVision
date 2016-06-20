g++ src/final.cpp src/segmentation.cpp src/skewCorrection.cpp src/otherProcessing.cpp `pkg-config --cflags --libs opencv`
./a.out ./Dataset/1.jpg
python3 src/predict.py