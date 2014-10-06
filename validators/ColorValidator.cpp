#include "ColorValidator.hpp"

#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

ColorValidator::ColorValidator( rapidxml::xml_node<> *node, std::string name, bool debug ):
		Validator(name, debug)
{
	std::string colorspace( node->first_node("colorspace")->value() );
	_conversionFlag = ( colorspace=="hsv" )? CV_BGR2HSV : -1;

	_areaThershold = atof(node->first_node("areaPercentage")->value());

	rapidxml::xml_node<> *ranges = node->first_node("range");
	for( ; ranges; ranges = ranges->next_sibling("range") )
	{
		double* minRange = new double[3]();
		rapidxml::xml_node<> *minR = ranges->first_node("minRange");
		minRange[0] = atof(minR->first_node("ch0")->value());
		minRange[1] = atof(minR->first_node("ch1")->value());
		minRange[2] = atof(minR->first_node("ch2")->value());

		double* maxRange = new double[3]();
		rapidxml::xml_node<> *maxR = ranges->first_node("maxRange");
		maxRange[0] = atof(maxR->first_node("ch0")->value());
		maxRange[1] = atof(maxR->first_node("ch1")->value());
		maxRange[2] = atof(maxR->first_node("ch2")->value());

		_ranges.push_back(std::make_pair(minRange, maxRange));
	}
}

bool ColorValidator::validate( cv::Mat pImage )
{
	bool result = true;

	cv::Mat convImg;
	if( _conversionFlag != -1 )
	{
		cv::cvtColor( pImage, convImg, _conversionFlag );
	}
	else
	{
		convImg = pImage.clone();
	}

	int colorp = 0;
	std::vector< std::pair<double*, double*> >::iterator it = _ranges.begin();;
	for( ; it != _ranges.end(); it++ )
	{
		cv::Mat color;
		cv::inRange( convImg, cv::Scalar(it->first[0], it->first[1], it->first[2]),
				cv::Scalar( it->second[0], it->second[1], it->second[2] ), color );

		colorp += cv::countNonZero(color);
	}
	int areaT = (_areaThershold*convImg.size().area());

	if( colorp < areaT )
	{
		result = false;
		if(debug) std::cout << "Plate rejected by " << getName() << " filter. with result " << colorp
				<< "." << std::endl;
	}

	return result;
}

ColorValidator::~ColorValidator(){}
