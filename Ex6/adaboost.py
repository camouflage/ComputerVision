import scipy.io as sio
import numpy as np
import sys
from sklearn.ensemble import AdaBoostClassifier
from sklearn.tree import DecisionTreeClassifier

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
    
    # http://scikit-learn.org/stable/modules/generated/sklearn.ensemble.AdaBoostClassifier.html
    bdt = AdaBoostClassifier(DecisionTreeClassifier(criterion="entropy"))
    bdt.fit(trainingImg, trainingLabel.ravel())

    predict = bdt.predict(testImg).reshape(numberOfTestData, 1)
    score = bdt.score(testImg, testLabel)
    #predictProba = bdt.predict_proba(testImg)
    wrongCount = numberOfTestData - np.count_nonzero(predict == testLabel)

    with open('adaboostAns.csv', 'w') as file:
        file.write("Error Rate: %f\n" %(wrongCount / 10000))
        file.write("Score: %f\n" %(score))
        for i in range(0, numberOfTestData):
            file.write("%d %d\n" %(predict[i], testLabel[i]))
