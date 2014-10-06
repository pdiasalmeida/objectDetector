#include "../auxiliar/FilesHelper.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <sys/stat.h>

std::string processDir;
std::string outDir;

std::vector< std::string > validExtensions;
std::vector< std::string > pfileNames;

enum colorspaces{HSV,BGR};
cv::MatND bgrHist[3];
cv::MatND hsvHist[2];

cv::MatND getHistHSVH( cv::Mat imgHSV )
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

cv::MatND getHistHSVS( cv::Mat imgHSV )
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

cv::MatND getHistHSVC( cv::Mat img, int channel )
{
	cv::MatND hist;

	hist = (channel==0) ? getHistHSVH(img) : getHistHSVS(img);

	return hist;
}

cv::MatND getDataSetHistHSVH( std::vector< std::string >& pfileNames )
{
	int h_bins = 50;
	int histSize = h_bins;

	float h_ranges[] = { 0, 180 };

	const float* ranges = { h_ranges };

	cv::MatND hist_base;
	std::vector< std::string >::iterator pit = pfileNames.begin();
	for( ; pit != pfileNames.end(); pit++ )
	{
		cv::Mat img = cv::imread( processDir+"/"+(*pit) );
		cv::Mat imgHSV;
		cv::cvtColor( img, imgHSV, CV_BGR2HSV );

		std::vector<cv::Mat> hsv_planes;
		split( imgHSV, hsv_planes );

		cv::calcHist( &hsv_planes[0], 1, 0, cv::Mat(), hist_base, 1, &histSize, &ranges, true, true );
	}

	cv::normalize( hist_base, hist_base, 0, hist_base.rows, cv::NORM_MINMAX, -1, cv::Mat() );

	return hist_base;
}

cv::MatND getDataSetHistHSVS( std::vector< std::string >& pfileNames )
{
	int s_bins = 60;
	int histSize = s_bins;

	float s_ranges[] = { 0, 256 };

	const float* ranges = { s_ranges };

	cv::MatND hist_base;
	std::vector< std::string >::iterator pit = pfileNames.begin();
	for( ; pit != pfileNames.end(); pit++ )
	{
		cv::Mat img = cv::imread( processDir+"/"+(*pit) );
		cv::Mat imgHSV;
		cv::cvtColor( img, imgHSV, CV_BGR2HSV );

		std::vector<cv::Mat> hsv_planes;
		split( imgHSV, hsv_planes );

		cv::calcHist( &hsv_planes[1], 1, 0, cv::Mat(), hist_base, 1, &histSize, &ranges, true, true );
	}

	cv::normalize( hist_base, hist_base, 0, hist_base.rows, cv::NORM_MINMAX, -1, cv::Mat() );

	return hist_base;
}

cv::MatND getDataSetHistHSVC( std::vector< std::string >&pfileNames, int channel )
{
	cv::MatND result;

	result = (channel==0)? getDataSetHistHSVH(pfileNames) :
			getDataSetHistHSVS(pfileNames);

	hsvHist[channel] = result;

	return result;
}

void saveDataSetHistHSV( std::vector< std::string >& pfileNames )
{
	cv::FileStorage fs(outDir+"/hsvHist.xml", cv::FileStorage::WRITE);
	fs << "Hhist" << hsvHist[0];
	fs << "Shist" << hsvHist[1];
	fs.release();
}

cv::MatND getHistBGRC( cv::Mat imgBGR, int channel )
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

cv::MatND getDataSetHistBGRC( std::vector< std::string >& pfileNames, int channel )
{
	int histSize = 256;

	float range[] = { 0, 256 };

	const float* ranges = { range };

	cv::MatND hist_base;
	std::vector< std::string >::iterator pit = pfileNames.begin();
	for( ; pit != pfileNames.end(); pit++ )
	{
		cv::Mat img = cv::imread( processDir+"/"+(*pit) );

		std::vector<cv::Mat> bgr_planes;
		split( img, bgr_planes );

		cv::calcHist( &bgr_planes[channel], 1, 0, cv::Mat(), hist_base, 1, &histSize, &ranges, true, true );
	}

	cv::normalize( hist_base, hist_base, 0, hist_base.rows, cv::NORM_MINMAX, -1, cv::Mat() );

	bgrHist[channel] = hist_base;

	return hist_base;
}

void saveDataSetHistBGR( std::vector< std::string >& pfileNames )
{
	cv::FileStorage fs(outDir+"/bgrHist.xml", cv::FileStorage::WRITE);
	fs << "Bhist" << bgrHist[0];
	fs << "Ghist" << bgrHist[1];
	fs << "Rhist" << bgrHist[2];
	fs.release();
}

void printThresholds(int colorspace, int channel)
{
	double min_correlation = 9999999.0;
	double max_chisqare = 0;
	double min_inters = 9999999.0;
	double max_batta = 0;

	std::string min_correlationImage;
	std::string max_chisqareImage;
	std::string min_intersImage;
	std::string max_battaImage;

	cv::MatND hist1;

	hist1 = (colorspace==0)? getDataSetHistHSVC(pfileNames, channel) :
			getDataSetHistBGRC( pfileNames, channel );

	std::vector< std::string >::iterator pit = pfileNames.begin();
	for( ; pit != pfileNames.end(); pit++ )
	{
		cv::Mat img = cv::imread( processDir+"/"+(*pit) );
		cv::Mat imgHSV;
		cv::cvtColor( img, imgHSV, CV_BGR2HSV );

		cv::MatND hist2;
		hist2 = (colorspace==0)? getHistHSVC( img, channel ) :
				getHistBGRC( img, channel );

		for( int i = 0; i < 4; i++ )
		{
			int compare_method = i;
			double base_other = compareHist( hist1, hist2, compare_method );

			switch(compare_method)
			{
			case CV_COMP_CORREL:
				if( base_other < min_correlation )
				{
					min_correlation = base_other;
					min_correlationImage = *pit;
				}
				break;
			case CV_COMP_CHISQR:
				if( base_other > max_chisqare )
				{
					max_chisqare = base_other;
					max_chisqareImage = *pit;
				}
				break;
			case CV_COMP_INTERSECT:
				if( base_other < min_inters )
				{
					min_inters = base_other;
					min_intersImage = *pit;
				}
				break;
			case CV_COMP_BHATTACHARYYA:
				if( base_other > max_batta )
				{
					max_batta = base_other;
					max_battaImage = *pit;
				}
				break;
			}
		}
	}

	std::cout << "****** Comparison results ******" << std::endl;
	std::cout << "Minimum correlation value of " << min_correlation << " for image " << min_correlationImage << "." << std::endl;
	std::cout << "Maximum chi_square value of " << max_chisqare << " for image " << max_chisqareImage << "." << std::endl;
	std::cout << "Minimum intersection value of " << min_inters << " for image " << min_intersImage << "." << std::endl;
	std::cout << "Maximum batta value of " << max_batta << " for image " << max_battaImage << "." << std::endl;

	std::cout << std::endl;
}

int main( int argc, char** argv )
{
	int status = EXIT_SUCCESS;

	if( argc >= 3 )
	{
		processDir = std::string(argv[1]);
		outDir = std::string(argv[2]);

		mkdir(outDir.c_str(), 0777);

		validExtensions.push_back("jpg");
		validExtensions.push_back("png");
		FilesHelper::getFilesInDirectory( processDir, pfileNames, validExtensions );

		std::cout << "Results for channel H in HSV histogram" << std::endl;
		printThresholds(HSV,0);
		std::cout << "Results for channel S in HSV histogram" << std::endl;
		printThresholds(HSV,1);

		std::cout << "Results for channel B in BGR histogram" << std::endl;
		printThresholds(BGR,0);
		std::cout << "Results for channel G in BGR histogram" << std::endl;
		printThresholds(BGR,1);
		std::cout << "Results for channel R in BGR histogram" << std::endl;
		printThresholds(BGR,2);

		std::cout << "Saving dataset histograms to " << outDir << "." << std::endl;
		saveDataSetHistHSV(pfileNames);
		saveDataSetHistBGR(pfileNames);
	}
	else
	{
		std::cout <<
				"Usage:\n" << argv[0] <<
				" <path to image dir> <path to save output> \n";
		status = EXIT_FAILURE;
	}

	return status;
}
