#include "SVMValidator.hpp"

#include <opencv2/imgproc/imgproc.hpp>

SVMValidator::SVMValidator( std::string name, int width, int height, std::string path, bool debug ):
	Validator(name,debug)
{
	_width = width;
	_height = height;
	_path = path;

	init();
}

SVMValidator::SVMValidator( rapidxml::xml_node<> *node, std::string name, bool debug ):
		Validator(name, debug)
{
	_width = atof(node->first_node("width")->value());
	_height = atof(node->first_node("height")->value());

	_path = std::string(node->first_node("path")->value());

	init();
}

void SVMValidator::init()
{
	_svmClassifier.load( _path.c_str() );
	_hog.winSize = cv::Size(_width, _height);

	_trainingPadding = cv::Size(0, 0);
	_winStride = cv::Size(8, 8);
}

bool SVMValidator::validate( cv::Mat pImage )
{
	cv::Mat pImageResize;
	cv::resize( pImage, pImageResize, cv::Size(_width, _height) );

	std::vector<float> featureVector;
	calculateFeaturesFromInput(pImageResize, featureVector, _hog);

	cv::Mat vectorImage( featureVector, true );
	cv::Mat p = vectorImage.reshape(1,1);

	p.convertTo( p, CV_32FC1 );
	int response = (int) _svmClassifier.predict( p );

	if( response == 0 )
	{
		if(debug) std::cout << "Plate rejected by " << getName() << " filter." << std::endl;
	}

	return response;
}

void SVMValidator::calculateFeaturesFromInput( cv::Mat imageData, std::vector< float >& featureVector,
		cv::HOGDescriptor& hog )
{
    std::vector< cv::Point > locations;
    hog.compute( imageData, featureVector, _winStride, _trainingPadding, locations );

    imageData.release();
}

SVMValidator::~SVMValidator(){}
