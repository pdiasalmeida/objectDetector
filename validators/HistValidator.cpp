#include "HistValidator.hpp"

#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

HistValidator::HistValidator( std::string name, int colorSpace, std::string path, double correl[3],
		double inter[3], double batta[3], bool debug ):
	Validator(name, debug)
{
	_path = std::string(path);
	_colorSpace = colorSpace;

	std::copy(correl, correl + 3, _correl);
	std::copy(inter, inter + 3, _inter);
	std::copy(batta, batta + 3, _batta);

	init();
}

HistValidator::HistValidator( rapidxml::xml_node<> *node, std::string name, bool debug ):
		Validator(name, debug)
{
	_path = std::string(node->first_node("path")->value());

	if( std::string(node->first_node("colorspace")->value()) == "hsv" )
	{
		_colorSpace = 0;

		rapidxml::xml_node<> *corr = node->first_node("Correlation");
		_correl[0] = atof(corr->first_node("ch0")->value());
		_correl[1] = atof(corr->first_node("ch1")->value());
		_correl[2] = -1;

		rapidxml::xml_node<> *inter = node->first_node("Intersection");
		_inter[0] = atof(inter->first_node("ch0")->value());
		_inter[1] = atof(inter->first_node("ch1")->value());
		_inter[2] = -1;

		rapidxml::xml_node<> *batt = node->first_node("Batta");
		_batta[0] = atof(batt->first_node("ch0")->value());
		_batta[1] = atof(batt->first_node("ch1")->value());
		_batta[2] = -1;
	}
	else
	{
		_colorSpace = 1;

		rapidxml::xml_node<> *corr = node->first_node("Correlation");
		_correl[0] = atof(corr->first_node("ch0")->value());
		_correl[1] = atof(corr->first_node("ch1")->value());
		_correl[2] = atof(corr->first_node("ch2")->value());

		rapidxml::xml_node<> *inter = node->first_node("Intersection");
		_inter[0] = atof(inter->first_node("ch0")->value());
		_inter[1] = atof(inter->first_node("ch1")->value());
		_inter[2] = atof(inter->first_node("ch2")->value());

		rapidxml::xml_node<> *batt = node->first_node("Batta");
		_batta[0] = atof(batt->first_node("ch0")->value());
		_batta[1] = atof(batt->first_node("ch1")->value());
		_batta[2] = atof(batt->first_node("ch2")->value());
	}

	init();
}

void HistValidator::init()
{
	switch(_colorSpace)
	{
		case 0:
			getDataSetHSVHists( _path );
			break;
		case 1:
			getDataSetBGRHists( _path );
			break;
	}
}

bool HistValidator::validate( cv::Mat pImage )
{
	bool result = false;

	switch(_colorSpace)
	{
	case 0:
		result = validateHSVHist(pImage);
		break;
	case 1:
		result = validateBGRHist(pImage);
		break;
	}

	return result;
}

bool HistValidator::validateHSVHist( cv::Mat image )
{
	bool result = false;

	cv::Mat imgHSV;
	cv::cvtColor( image, imgHSV, CV_BGR2HSV );

	cv::Mat imageH = getHistHSVH(imgHSV);
	cv::Mat imageS = getHistHSVS(imgHSV);

	result = validateHSVHist(imageH, histH, 0, CV_COMP_CORREL) && validateHSVHist(imageS, histS, 1, CV_COMP_CORREL)
			&& validateHSVHist(imageH, histH, 0, CV_COMP_INTERSECT) && validateHSVHist(imageS, histS, 1, CV_COMP_INTERSECT)
			&& validateHSVHist(imageH, histH, 0, CV_COMP_BHATTACHARYYA) && validateHSVHist(imageS, histS, 1, CV_COMP_BHATTACHARYYA);

	return result;
}

bool HistValidator::validateBGRHist( cv::Mat image )
{
	bool result = false;

	cv::Mat imageB = getHistBGRC( image, 0 );
	cv::Mat imageG = getHistBGRC( image, 1 );
	cv::Mat imageR = getHistBGRC( image, 2 );

	result = validateBGRHist(imageB, histB, 0, CV_COMP_CORREL) && validateBGRHist(imageG, histG, 1, CV_COMP_CORREL)
			&& validateBGRHist(imageR, histR, 2, CV_COMP_CORREL) && validateBGRHist(imageB, histB, 0, CV_COMP_INTERSECT)
			&& validateBGRHist(imageG, histG, 1, CV_COMP_INTERSECT) && validateBGRHist(imageR, histR, 2, CV_COMP_INTERSECT)
			&& validateBGRHist(imageB, histB, 0, CV_COMP_BHATTACHARYYA) && validateBGRHist(imageG, histG, 1, CV_COMP_BHATTACHARYYA)
			&& validateBGRHist(imageR, histR, 2, CV_COMP_BHATTACHARYYA);

	return result;
}

void HistValidator::getDataSetHSVHists( std::string path )
{
	cv::FileStorage fs;
	fs.open( path, cv::FileStorage::READ );

	fs["Hhist"] >> histH;
	fs["Shist"] >> histS;

	fs.release();
}

cv::MatND HistValidator::getHistHSVH( cv::Mat imgHSV )
{
	int h_bins = 50;
	int histSize = h_bins;

	float h_ranges[] = { 0, 180 };

	const float* ranges = { h_ranges };

	cv::MatND hist_base;
	std::vector<cv::Mat> hsv_planes;
	split( imgHSV, hsv_planes );

	cv::calcHist( &hsv_planes[0], 1, 0, cv::Mat(), hist_base, 1, &histSize, &ranges, true, true );
	cv::normalize( hist_base, hist_base, 0, hist_base.rows, cv::NORM_MINMAX, -1, cv::Mat() );

	return hist_base;
}

cv::MatND HistValidator::getHistHSVS( cv::Mat imgHSV )
{
	int s_bins = 60;
	int histSize = s_bins;

	float s_ranges[] = { 0, 256 };

	const float* ranges = { s_ranges };

	cv::MatND hist_base;
	std::vector<cv::Mat> hsv_planes;
	split( imgHSV, hsv_planes );

	cv::calcHist( &hsv_planes[1], 1, 0, cv::Mat(), hist_base, 1, &histSize, &ranges, true, true );
	cv::normalize( hist_base, hist_base, 0, hist_base.rows, cv::NORM_MINMAX, -1, cv::Mat() );

	return hist_base;
}

bool HistValidator::validateHSVHist( cv::Mat hist1, cv::Mat hist2, int channel, int method )
{
	bool result = false;
	double value = compareHist( hist1, hist2, method );

	switch(method)
	{
	case CV_COMP_CORREL:
		if( isnan(value) || value >= _correl[channel] )
			result = true;
		else
			if(debug) std::cout << "Plate rejected by hsv histogram correlation in channel " << channel << ". whith value "
			<< value << "." << std::endl;
		break;
	case CV_COMP_INTERSECT:
		if( isnan(value) || value >= _inter[channel] )
			result = true;
		else
			if(debug) std::cout << "Plate rejected by hsv histogram intersection in channel " << channel << ". whith value "
			<< value << "." << std::endl;
		break;
	case CV_COMP_BHATTACHARYYA:
		if( isnan(value) || value <= _batta[channel] )
			result = true;
		else
			if(debug) std::cout << "Plate rejected by hsv histogram batta in channel " << channel << ". whith value "
			<< value << "." << std::endl;
		break;
	}

	return result;
}

void HistValidator::getDataSetBGRHists( std::string path )
{
	cv::FileStorage fs;
	fs.open( path, cv::FileStorage::READ );

	fs["Bhist"] >> histB;
	fs["Ghist"] >> histG;
	fs["Rhist"] >> histR;

	fs.release();
}

cv::MatND HistValidator::getHistBGRC( cv::Mat imgBGR, int channel )
{
	int histSize = 256;

	float range[] = { 0, 256 };

	const float* ranges = { range };

	cv::MatND hist_base;
	std::vector<cv::Mat> bgr_planes;
	split( imgBGR, bgr_planes );

	cv::calcHist( &bgr_planes[channel], 1, 0, cv::Mat(), hist_base, 1, &histSize, &ranges, true, true );
	cv::normalize( hist_base, hist_base, 0, hist_base.rows, cv::NORM_MINMAX, -1, cv::Mat() );

	return hist_base;
}

bool HistValidator::validateBGRHist( cv::Mat hist1, cv::Mat hist2, int channel, int method )
{
	bool result = false;
	double value = compareHist( hist1, hist2, method );

	switch(method)
	{
	case CV_COMP_CORREL:
		if( isnan(value) || value >= _correl[channel] )
			result = true;
		else
			if(debug) std::cout << "Plate rejected by bgr histogram correlation in channel " << channel << ". whith value "
			<< value << "." << std::endl;
		break;
	case CV_COMP_INTERSECT:
		if( isnan(value) || value >= _inter[channel] )
			result = true;
		else
			if(debug) std::cout << "Plate rejected by bgr histogram intersection in channel " << channel << ". whith value "
			<< value << "." << std::endl;
		break;
	case CV_COMP_BHATTACHARYYA:
		if( isnan(value) || value <= _batta[channel] )
			result = true;
		else
			if(debug) std::cout << "Plate rejected by bgr histogram batta in channel " << channel << ". whith value "
			<< value << "." << std::endl;
		break;
	}

	return result;
}

HistValidator::~HistValidator(){}