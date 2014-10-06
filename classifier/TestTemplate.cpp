#include "../auxiliar/FilesHelper.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

std::string filesDir;
std::vector<std::string> positives;
std::vector<std::string> validExtensions;
cv::Mat templ;

int main( int argc, char** argv )
{
	int status = EXIT_SUCCESS;

	float max_sqdiff = 0;
	float min_coeff = 999999999;
	float min_corr = 999999999;

	if( argc >= 3 )
	{
		filesDir = std::string(argv[1]);
		templ = cv::imread( std::string(argv[2]) );

		validExtensions.push_back("jpg");
		validExtensions.push_back("png");
		validExtensions.push_back("ppm");

		FilesHelper::getFilesInDirectory(filesDir, positives, validExtensions);

		std::vector< std::string >::iterator pit = positives.begin();
		for( ; pit != positives.end(); pit++ )
		{
			cv::Mat img = cv::imread( filesDir + "/" + *pit);
			cv::Mat imgResize;
			cv::resize( img, imgResize, cv::Size(templ.cols, templ.rows) );

			cv::Mat result;
			result.create( 1, 1, CV_32FC1 );

			matchTemplate( imgResize, templ, result, CV_TM_SQDIFF );
			max_sqdiff = (result.at<float>(0,0)>max_sqdiff)? result.at<float>(0,0):max_sqdiff;

			matchTemplate( imgResize, templ, result, CV_TM_CCORR );
			min_coeff = (result.at<float>(0,0)<min_coeff)? result.at<float>(0,0):min_coeff;

			matchTemplate( imgResize, templ, result, CV_TM_SQDIFF );
			min_corr = (result.at<float>(0,0)<min_corr)? result.at<float>(0,0):min_corr;
		}
		std::cout << "this dataset has values:" << std::endl;
		std::cout << "\tMax Square diff: " << max_sqdiff << "." << std::endl;
		std::cout << "\tMin ccoef: " << min_coeff << "." << std::endl;
		std::cout << "\tMin corr: " << min_corr << "." << std::endl;
	}
	else
	{
		std::cout <<
				"Usage:\n" << argv[0] <<
				" <path to positive images> <path to template image>\n";
		status = EXIT_FAILURE;
	}

	return status;
}
