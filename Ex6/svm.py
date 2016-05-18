import scipy.io as sio
import numpy as np
import sys
from sklearn.svm import SVC

if __name__ == '__main__':
    # Read in .mat
    trainingImgMat = sio.loadmat('MNIST/mnist_train.mat')
    trainingLabelMat = sio.loadmat('MNIST/mnist_train_labels.mat')
    testImgMat = sio.loadmat('MNIST/mnist_test.mat')
    testLabelMat = sio.loadmat('MNIST/mnist_test_labels.mat')

    trainingImg = trainingImgMat["mnist_train"]
    trainingLabel = trainingLabelMat["mnist_train_labels"]
    testImg = testImgMat["mnist_test"]
    testLabel = testLabelMat["mnist_test_labels"]

    numberOfTrainingData = trainingImg.shape[0]
    numberOFFeatures = trainingImg.shape[1]
    numberOfTestData = testImg.shape[0]
    
    # Ref: http://scikit-learn.org/stable/modules/generated/sklearn.svm.SVC.html
    clf = SVC()
    clf.fit(trainingImg, trainingLabel.ravel()) 

    predict = clf.predict(testImg).reshape
    score = clf.score(testImg, testLabel)
    #predictProba = clf.predict_proba(testImg)
    wrongCount = np.count_nonzero(predict == testLabel)

    with open('svmAns.csv', 'w') as file:
        file.write("Error Rate: %f\n" %(wrongCount / 10000))
        file.write("Score: %f" %(score))
        for i in range(0, numberOfTestData):
            file.write("%d %d\n" %(predict[i], testLabel[i]))

