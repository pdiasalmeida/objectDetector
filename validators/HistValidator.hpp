#ifndef HISTVALIDATOR_HPP_
#define HISTVALIDATOR_HPP_

#include "Validator.hpp"
#include "../libs/rapidxml/rapidxml.hpp"

class HistValidator : Validator
{
public:
	HistValidator( rapidxml::xml_node<> *node, std::string name, bool debug );

	virtual bool validate( cv::Mat pImage );

	~HistValidator();

private:
	void getDataSetHSVHists( std::string path );
	cv::MatND getHistHSVS( cv::Mat imgHSV );
	cv::MatND getHistHSVH( cv::Mat imgHSV );
	bool validateHSVHist( cv::Mat hist1, cv::Mat hist2, int channel, int method );
	bool validateHSVHist( cv::Mat image );

	void getDataSetBGRHists( std::string path );
	cv::MatND getHistBGRC( cv::Mat imgBGR, int channel );
	bool validateBGRHist( cv::Mat hist1, cv::Mat hist2, int channel, int method );
	bool validateBGRHist( cv::Mat image );

	void init();

protected:
	std::string _path;
	int _colorSpace;
	double _correl[3];
	double _inter[3];
	double _batta[3];
	double _chiSqr[3];

	cv::Mat histH, histS;
	cv::Mat histB, histG, histR;
};

#endif
