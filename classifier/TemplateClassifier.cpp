#include "TemplateClassifier.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

TemplateClassifier::TemplateClassifier( std::string name, int method, float thresh, bool debug ):
	Classifier(name,debug)
{
	_method = method;
	_thresh = thresh;
}

TemplateClassifier::TemplateClassifier( rapidxml::xml_node<> *node, std::string name, bool debug ):
		Classifier(name, debug)
{
	std::string method = std::string(node->first_node("method")->value());
	_method = (method == "SQRD") ? CV_TM_SQDIFF :
			(method == "CORR") ? CV_TM_CCORR : CV_TM_CCOEFF;

	_thresh = atof(node->first_node("thresh")->value());
}

void TemplateClassifier::addClass( std::string name, std::string templPath )
{
	_classes.push_back( std::make_pair(name, templPath) );
}

float TemplateClassifier::getScore( cv::Mat roi, cv::Mat templ )
{
	float score = -1.0f;

	cv::Mat imgResize;
	cv::resize( roi, imgResize, cv::Size(templ.cols, templ.rows) );

	cv::Mat result;
	result.create( 1, 1, CV_32FC1 );

	matchTemplate( imgResize, templ, result, _method );

	if(_method == CV_TM_SQDIFF)
		score = (result.at<float>(0,0)<_thresh)? result.at<float>(0,0):-1.0f;
	else
		score = (result.at<float>(0,0)>_thresh)? result.at<float>(0,0):-1.0f;

	return score;
}

std::string TemplateClassifier::getClass( cv::Mat roi )
{
	float bestScore = (_method==CV_TM_SQDIFF) ? 999999:0;
	std::string classe = "";

	std::vector< std::pair< std::string, std::string > >::iterator itc = _classes.begin();
	for( ; itc != _classes.end(); itc++ )
	{
		cv::Mat templ = cv::imread(itc->second);
		float score = getScore(roi, templ);
		std::ostringstream ss;
		if( (score!=-1) && (_method==CV_TM_SQDIFF) && (score < bestScore) )
		{
			bestScore = score;
			ss << itc->first << '|' << score;
			classe = ss.str();
		}
		else if( (score!=-1) && (score > bestScore) )
		{
			bestScore = score;
			ss << itc->first << '|' << score;
			classe = ss.str();
		}
	}

	return classe;
}

TemplateClassifier::~TemplateClassifier(){}
