#ifndef COLORVALIDATOR_HPP_
#define COLORVALIDATOR_HPP_

#include "Validator.hpp"
#include "../libs/rapidxml/rapidxml.hpp"

class ColorValidator : Validator
{
public:
	ColorValidator( rapidxml::xml_node<> *node, std::string name, bool debug );

	virtual bool validate( cv::Mat pImage );

	~ColorValidator();

protected:
	int _conversionFlag;
	double _minAreaThershold;
	double _maxAreaThershold;
	std::vector< std::pair<double*, double*> > _ranges;
};

#endif
