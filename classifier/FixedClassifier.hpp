#ifndef FIXEDCLASSIFIER_HPP_
#define FIXEDCLASSIFIER_HPP_

#include "Classifier.hpp"
#include "../libs/rapidxml/rapidxml.hpp"

class FixedClassifier : Classifier
{
public:
	FixedClassifier( rapidxml::xml_node<> *node, std::string name, bool debug );

	void addClass( std::string name );

	virtual std::string getClass( cv::Mat pImage );

	~FixedClassifier();

protected:
	std::string _classe;
};

#endif
