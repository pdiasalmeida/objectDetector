#ifndef TEMPLATECLASSIFIER_HPP_
#define TEMPLATECLASSIFIER_HPP_

#include "Classifier.hpp"
#include "../libs/rapidxml/rapidxml.hpp"

class TemplateClassifier : Classifier
{
public:
	TemplateClassifier( std::string name, int method, float thresh, bool debug );

	TemplateClassifier( rapidxml::xml_node<> *node, std::string name, bool debug );

	void addClass( std::string name, std::string templPath );
	float getScore( cv::Mat pImage, cv::Mat templ );

	virtual std::string getClass( cv::Mat pImage );

	~TemplateClassifier();

protected:
	int _method;
	float _thresh;
	std::vector< std::pair< std::string, std::string > > _classes;
};

#endif
