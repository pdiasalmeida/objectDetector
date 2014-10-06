#include "../../auxiliar/FilesHelper.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

std::string processDir;
std::string outDir;
std::string gtPath;

std::vector< std::string > fileNames;
std::vector< std::string > validExtensions;
std::map< std::string, std::vector< std::string > > kValues;

void readInfoPositives( )
{
	validExtensions.push_back("jpg");
	validExtensions.push_back("png");

	FilesHelper::getFilesInDirectory( processDir, fileNames, validExtensions );

	FilesHelper::parseKeyValue( gtPath, kValues, ' ' );
}

void makeThumbs()
{
	std::map< std::string, std::vector< std::string > >::iterator itKV = kValues.begin();
	for( int i = 0; itKV != kValues.end(); itKV++, i++ )
	{
		cv::Mat image = cv::imread( processDir+"/"+itKV->first );
		std::vector< std::string >::iterator itSV = itKV->second.begin();
		int n = atoi( (*itSV).c_str() ); itSV++;
		for( ; itSV != itKV->second.end(); n-- )
		{
			int x = atoi( (*itSV).c_str() ); itSV++;
			int y = atoi( (*itSV).c_str() ); itSV++;

			int w = atoi( (*itSV).c_str() ); itSV++;
			if( (x+w) > image.cols ) w = image.cols - x;

			int h = atoi( (*itSV).c_str() ); itSV++;
			if( (y+h) > image.rows ) h = image.rows - y;

			cv::Rect roi = cv::Rect( x, y, w, h );
			cv::Mat roiImage = cv::Mat( image, roi );

			std::stringstream path;
			path << outDir << "/" << i << "-" << FilesHelper::getFileName(itKV->first) << "_" << n << ".jpg";
			cv::imwrite( path.str(), roiImage );
		}
	}
}

int main( int argc, char** argv )
{
	FilesHelper::debug = false;
	int status = EXIT_SUCCESS;

	if( argc >= 4 )
	{
		processDir = std::string(argv[1]);
		outDir = std::string(argv[2]);
		gtPath = std::string(argv[3]);

		mkdir(outDir.c_str(), 0777);

		readInfoPositives();
		makeThumbs();

	}else{
		std::cout <<
				"Usage:\n" << argv[0] <<
				" <path to working dir> <path to save output> <path to groundtruth file> \n";
		status = EXIT_FAILURE;
	}

	return status;
}
