#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <opencv2/ml/ml.hpp>

#include <iostream>
#include <vector>

#include "../../auxiliar/FilesHelper.hpp"

std::vector< std::string > validExtensions;
std::vector< std::string > pfileNames;
std::vector< std::string > nfileNames;

cv::Size plateSize;
int plate_width = 64;
int plate_height = 128;
static const cv::Size trainingPadding = cv::Size(0, 0);
static const cv::Size winStride = cv::Size(8, 8);

cv::HOGDescriptor hog;

static void calculateFeaturesFromInput( cv::Mat imageData, std::vector< float >& featureVector, cv::HOGDescriptor& hog )
{
	std::vector< cv::Point > locations;
	hog.compute( imageData, featureVector, winStride, trainingPadding, locations );

	imageData.release();
}

void trainValidator( std::string pathPositives, std::string pathNegatives, std::string outDir )
{
	cv::Mat classes;
	cv::Mat trainingData;

	cv::Mat trainingImages;
	std::vector< int > trainingLabels;

	FilesHelper::getFilesInDirectory( pathPositives, pfileNames, validExtensions );
	FilesHelper::getFilesInDirectory( pathNegatives, nfileNames, validExtensions );

	std::vector< std::string >::iterator pit = pfileNames.begin();
	for( ; pit != pfileNames.end(); pit++ )
	{
		cv::Mat img = cv::imread( pathPositives + "/" + *pit);
		cv::Mat imgResize;
		cv::resize( img, imgResize, plateSize );

		std::vector<float> featureVector;
		calculateFeaturesFromInput(imgResize, featureVector, hog);

		cv::Mat vectorImage( featureVector, true );
		cv::Mat vectorImageResize = vectorImage.reshape(1,1);

		trainingImages.push_back(vectorImageResize);
		trainingLabels.push_back(1);
	}

	std::vector< std::string >::iterator nit = nfileNames.begin();
	for( ; nit != nfileNames.end(); nit++ )
	{
		cv::Mat img = cv::imread( pathNegatives + "/" + *nit);
		cv::Mat imgResize;
		cv::resize( img, imgResize, plateSize );

		std::vector<float> featureVector;
		calculateFeaturesFromInput(imgResize, featureVector, hog);

		cv::Mat vectorImage( featureVector, true );
		cv::Mat vectorImageResize = vectorImage.reshape(1,1);

		trainingImages.push_back(vectorImageResize);
		trainingLabels.push_back(0);
	}

	cv::Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	cv::Mat(trainingLabels).copyTo(classes);

	std::cout << "Begin training SVM ..." << std::endl;

	CvSVMParams SVM_params;
	SVM_params.kernel_type = CvSVM::LINEAR;

	CvSVM svmClassifier;
	svmClassifier.train( trainingData, classes, cv::Mat(), cv::Mat(), SVM_params );

	std::cout << "Finished training SVM." << std::endl;

	std::string path = outDir + "/" +"SVMValidator.xml";
	svmClassifier.save(path.c_str());
}

void trainClassifier( std::string outDir, std::vector< std::string > classesPaths )
{
	cv::Mat classes;
	cv::Mat trainingData;

	cv::Mat trainingImages;
	std::vector< int > trainingLabels;

	std::vector< std::string >::iterator it = classesPaths.begin();
	for( int i = 0; it != classesPaths.end(); it++, i++ )
	{
		std::vector< std::string > imageFileNames;
		FilesHelper::getFilesInDirectory( *it, imageFileNames, validExtensions );

		std::vector< std::string >::iterator pit = imageFileNames.begin();
		for( ; pit != imageFileNames.end(); pit++ )
		{
			cv::Mat img = cv::imread( *it + "/" + *pit);
			cv::Mat imgResize;
			cv::resize( img, imgResize, plateSize );

			std::vector<float> featureVector;
			calculateFeaturesFromInput(imgResize, featureVector, hog);

			cv::Mat vectorImage( featureVector, true );
			cv::Mat vectorImageResize = vectorImage.reshape(1,1);

			trainingImages.push_back(vectorImageResize);
			trainingLabels.push_back(i);
		}
	}

	cv::Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	cv::Mat(trainingLabels).copyTo(classes);

	std::cout << "Begin training SVM ..." << std::endl;

	CvSVMParams SVM_params;
	SVM_params.kernel_type = CvSVM::LINEAR;

	CvSVM svmClassifier;
	svmClassifier.train( trainingData, classes, cv::Mat(), cv::Mat(), SVM_params );

	std::cout << "Finished training SVM." << std::endl;

	std::string path = outDir + "/" +"SVMClassifier.xml";
	svmClassifier.save(path.c_str());
}

int main ( int argc, char** argv )
{
	validExtensions.push_back("jpg");
	validExtensions.push_back("ppm");
	validExtensions.push_back("png");

	plateSize = cv::Size( plate_width, plate_height );
	hog.winSize = cv::Size(64, 128);

	if( argc >= 2 )
	{
		for(int i = 1; i < argc; i++ )
		{
			std::string opt = std::string(argv[i]);
			if( opt == "-v" )
			{
				trainValidator( std::string(argv[i+1]), std::string(argv[i+2]), std::string(argv[i+3]) );
				break;
			}
			if( opt == "-c" )
			{
				std::vector< std::string > classesPaths;
				for( int n = i+2; n < argc; n++ )
				{
					classesPaths.push_back(std::string(argv[n]));
				}
				trainClassifier( std::string(argv[i+1]), classesPaths );
				break;
			}
		}
	}
	else
	{
		std::cout <<
				"Usage:\n" << argv[0] <<
				"To train a validator:" << std::endl <<
						"\t -v <path to positive objects images> <path to negative objects images> <path for output files> \n" <<
				"To train a classifier:" << std::endl <<
						"\t -c <path for output files> <path to class 0 images> <path to class 1 images> ... <path to class n images> \n";

	}

	return 0;
}
