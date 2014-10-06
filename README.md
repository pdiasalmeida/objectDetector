# Object Detector #

Unix project for the automatic detection of several objects. It works on BGR images and has so far included the detection of European license plates and indication signs.

## Dependencies ##
* OpenCV 2.4.9
* tesseract-ocr

## Use ##

Build the main program by running 'make' in the base directory and run './plateDetector' to process all images placed in the benchmark folder, results will be saved in 'benchmark_out'. You can use options '-d' and '-o' to specify the directory with the pictures to process and where to save results, respectively.

You can set which detectors to use in the execution by editing the 'config.xml' file. In this file you can set several of the application's options as well as the detectors to use and the validators that will aid in the reduction of false positives.

At the moment the only detector implemented is a cascade classifier based detector. There are three validators implemented:  based on the color of positive objects; based on the histogram of positive objects; and a svm trained with the HOG features of the positive objects.

## Machine Learning Algorithms ##

* The detection phase in this project is based on a cascade classifier trained with Haar features.
* The detected regions can then be validated using a svm classifier trained with hog descriptors.

### Training ###
* Cascade
    
    To train a cascade classifier you need a dataset with positive and negative samples. In our case we use an "info" file to annotate the position of the object in the positive images.

    In order to create the '.vec' file for our positive samples the following command was called:

            opencv_createsamples -info info.dat -vec samples.vec -w 40 -h 16

    Then we trained the cascade classifier with:

             opencv_traincascade -data classifier -vec samples.vec -bg negatives.txt -numStages 20 -minHitRate 0.999 -maxFalseAlarmRate 0.5 -numPos 345 -numNeg 643 -w 40 -h 16 -mode ALL -precalcValBufSize 2048 -precalcIdxBufSize 2048
    

* SVM

    The SVM classifier also needs a dataset of positive and negative samples.
    To train a svm classifier build our svm trainer by running 'make' in the svm directory and execute the following command:

            ./trainSVM <path to positive images> <path to negative images> <path to save output files>
