#ifndef SVMVALIDATOR_HPP_
#define SVMVALIDATOR_HPP_

#include "Validator.hpp"
#include "../libs/rapidxml/rapidxml.hpp"

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

class SVMValidator : Validator
{
public:
	SVMValidator( std::string name, int width, int height, std::string path, bool debug );

	SVMValidator( rapidxml::xml_node<> *node, std::string name, bool debug );

	virtual bool validate( cv::Mat pImage );

	~SVMValidator();

private:
	void init();
	void calculateFeaturesFromInput( cv::Mat imageData,
			std::vector< float >& featureVector, cv::HOGDescriptor& hog );

protected:
	std::string _path;
	int _width;
	int _height;

	CvSVM _svmClassifier;

	cv::HOGDescriptor _hog;
	cv::Size _trainingPadding;
	cv::Size _winStride;
};

#endif
