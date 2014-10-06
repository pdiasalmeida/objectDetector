#include "SVMClassifier.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

SVMClassifier::SVMClassifier( std::string name, std::string path, int width, int height, bool debug ):
	Classifier(name,debug)
{
	_width = width;
	_height = height;
	_path = path;

	init();
}

SVMClassifier::SVMClassifier( rapidxml::xml_node<> *node, std::string name, bool debug ):
		Classifier(name, debug)
{
	_width = atof(node->first_node("width")->value());
	_height = atof(node->first_node("height")->value());
	_path = std::string(node->first_node("path")->value());

	init();
}

void SVMClassifier::init()
{
	_svmClassifier.load( _path.c_str() );
	_hog.winSize = cv::Size(_width, _height);

	_trainingPadding = cv::Size(0, 0);
	_winStride = cv::Size(8, 8);
}

void SVMClassifier::addClass( std::string name, int id )
{
	_classes.insert( std::make_pair(id, name) );
}

std::string SVMClassifier::getClass( cv::Mat roi )
{
	std::string classe = "";

	cv::Mat pImageResize;
	cv::resize( roi, pImageResize, cv::Size(_width, _height) );

	std::vector<float> featureVector;
	calculateFeaturesFromInput(pImageResize, featureVector, _hog);

	cv::Mat vectorImage( featureVector, true );
	cv::Mat p = vectorImage.reshape(1,1);

	p.convertTo( p, CV_32FC1 );
	int response = (int) _svmClassifier.predict( p );

	if(_classes.find(response)!=_classes.end())
		classe = _classes.find(response)->second;

	return classe;
}

void SVMClassifier::calculateFeaturesFromInput( cv::Mat imageData, std::vector< float >& featureVector,
		cv::HOGDescriptor& hog )
{
    std::vector< cv::Point > locations;
    hog.compute( imageData, featureVector, _winStride, _trainingPadding, locations );

    imageData.release();
}

SVMClassifier::~SVMClassifier(){}
