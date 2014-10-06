#ifndef VALIDATOR_HPP_
#define VALIDATOR_HPP_

#include <opencv2/core/core.hpp>

class Validator
{
public:
	virtual bool validate( cv::Mat roi ) = 0;

	std::string getName(){return _name;};

	virtual ~Validator(){}

protected:
	std::string _name;
	bool debug;

	Validator(std::string strName, bool db):
			_name(strName){debug = db;}
};

#endif
