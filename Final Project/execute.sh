g++ src/final.cpp `pkg-config --cflags --libs opencv`
./a.out ./Dataset/1.jpg
python3 src/predict.py