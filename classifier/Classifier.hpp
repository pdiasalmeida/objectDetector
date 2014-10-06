#ifndef CLASSIFIER_HPP_
#define CLASSIFIER_HPP_

#include <opencv2/core/core.hpp>

class Classifier
{
public:
	virtual std::string getClass( cv::Mat roi ) = 0;

	std::string getName(){return _name;};

	virtual ~Classifier(){}

protected:
	std::string _name;
	bool debug;

	Classifier(std::string strName, bool db):
			_name(strName){debug = db;}
};

#endif
