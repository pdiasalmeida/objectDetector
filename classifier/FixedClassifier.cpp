#include "FixedClassifier.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

FixedClassifier::FixedClassifier( rapidxml::xml_node<> *node, std::string name, bool debug ):
		Classifier(name, debug)
{}

void FixedClassifier::addClass( std::string name )
{
	_classe = name;
}

std::string FixedClassifier::getClass( cv::Mat roi )
{
	return _classe;
}

FixedClassifier::~FixedClassifier(){}
