#include "CascadeClassifierDetector.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <sys/stat.h>

CascadeClassifierDetector::CascadeClassifierDetector( std::string name, std::string procDir, std::string outDir, int output,
		bool aggr, bool ann, std::string path, double scale, int minNeighb, cv::Size minSize, cv::Size maxSize, bool debug,
		bool saveThumbs ):
	Detector(name, procDir, outDir, output, aggr, ann, debug, saveThumbs)
{
	_path = std::string(path);
	_scale = scale;
	_minNeighbours = minNeighb;

	_minSize = cv::Size(minSize);
	_maxSize = cv::Size(maxSize);

	init();
}

CascadeClassifierDetector::CascadeClassifierDetector( rapidxml::xml_node<> *node, std::string name,
		std::string procDir, std::string outDir, int output, bool aggregate, bool annotate, bool debug, bool saveThumbs ):
		Detector(name, procDir, outDir, output, aggregate, annotate, debug, saveThumbs)
{
	_path = std::string(node->first_node("path")->value());
	_scale = atof(node->first_node("scale")->value());
	_minNeighbours = atoi(node->first_node("minNeighbours")->value());

	rapidxml::xml_node<> *minSize = node->first_node("minSize");
	_minSize = cv::Size(atoi(minSize->first_node("width")->value()),
			atoi(minSize->first_node("height")->value()));

	rapidxml::xml_node<> *maxSize = node->first_node("maxSize");
	_maxSize = cv::Size(atoi(maxSize->first_node("width")->value()),
			atoi(maxSize->first_node("height")->value()));

	init();
}

void CascadeClassifierDetector::init()
{
	if( !cascade.load( _path ) )
	{
		std::cout << "--(!)Error loading cascade\n" << std::endl;
	};
}

std::vector< cv::Rect > CascadeClassifierDetector::detect( cv::Mat image, std::string name )
{
	std::vector< cv::Rect > objects;
	std::vector< cv::Rect > validObjects;
	cv::Mat frame_gray;

	cv::cvtColor( image, frame_gray, cv::COLOR_BGR2GRAY );

	std::cout << "\tDetecting object with " << getName() << "..." << std::endl;

	cascade.detectMultiScale( frame_gray, objects, _scale, _minNeighbours, 0, _minSize, _maxSize );

	std::cout << "\tFinished detecting objects." << std::endl;

	std::vector< cv::Rect >::iterator pit = objects.begin();
	for( int n = 0; pit != objects.end(); pit++, n++ )
	{
		cv::Mat pImage = cv::Mat( image, *pit );

		bool valid = true;
		std::vector< Validator* >::iterator vit = _validators.begin();
		for( ; valid == true && vit != _validators.end(); vit++ )
		{
			valid = valid && (*vit)->validate(pImage);
		}

		if( valid )
		{
			validObjects.push_back(*pit);
		}

		if(saveThumbs)
		{
			mkdir(std::string(_outDir+"/rejs").c_str(), 0777);
			mkdir(std::string(_outDir+"/appr").c_str(), 0777);

			std::stringstream ss;
			ss << _outDir;
			if(!valid)
			{
				ss << "/rejs/";
				Validator* rej = *(vit-1);
				ss << rej->getName();
				ss << "_";
			}
			else ss << "/appr/";

			ss << name << "_" << getName() << "_";
			ss << n << ".jpg";
			cv::imwrite(ss.str(),pImage);
		}
	}

	return validObjects;
}

CascadeClassifierDetector::~CascadeClassifierDetector(){}
