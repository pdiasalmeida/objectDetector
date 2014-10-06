#ifndef CASCADECLASSIFIERDETECTOR_HPP_
#define CASCADECLASSIFIERDETECTOR_HPP_

#include "Detector.hpp"
#include "../libs/rapidxml/rapidxml.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

class CascadeClassifierDetector : Detector
{
public:
	CascadeClassifierDetector( std::string name, std::string procDir, std::string outDir, int output, bool aggr, bool ann,
			std::string path, double scale, int minNeighb, cv::Size minSize, cv::Size maxSize, bool debug, bool saveThumbs );
	CascadeClassifierDetector( rapidxml::xml_node<> *node, std::string name, std::string procDir, std::string outDir,
			int output, bool aggregate, bool annotate, bool debug, bool saveThumbs );

	virtual std::vector< cv::Rect > detect( cv::Mat image, std::string name );

	~CascadeClassifierDetector();

private:
	void init();

protected:
	std::string _path;
	double _scale;
	int _minNeighbours;
	cv::Size _minSize;
	cv::Size _maxSize;

	cv::CascadeClassifier cascade;
};

#endif
