#ifndef SVMCLASSIFIER_HPP_
#define SVMCLASSIFIER_HPP_

#include "Classifier.hpp"
#include "../libs/rapidxml/rapidxml.hpp"

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

class SVMClassifier : Classifier
{
public:
	SVMClassifier( std::string name, std::string path, int width, int height, bool debug );

	SVMClassifier( rapidxml::xml_node<> *node, std::string name, bool debug );

	void addClass( std::string name, int id );

	virtual std::string getClass( cv::Mat pImage );

	~SVMClassifier();

private:
	void init();
	void calculateFeaturesFromInput( cv::Mat imageData, std::vector< float >& featureVector,
			cv::HOGDescriptor& hog );

protected:
	std::string _path;
	int _width;
	int _height;
	std::map< int, std::string > _classes;

	CvSVM _svmClassifier;

	cv::HOGDescriptor _hog;
	cv::Size _trainingPadding;
	cv::Size _winStride;
};

#endif
