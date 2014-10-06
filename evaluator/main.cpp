#include "../auxiliar/FilesHelper.hpp"
#include "../auxiliar/key_iterator.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <set>
#include <utility>
#include <iostream>
#include <algorithm>

std::string benchmarkFile;
std::string fileToEvaluate;
std::string imageDir;

char gtDelimiter = ' ';
char rsDelimiter = ',';

struct eval_result
{
	std::vector< cv::Rect > correctMatches;
	std::vector< cv::Rect > falsePositives;
	std::vector< cv::Rect > falseNegatives;

	std::vector< cv::Rect > correctClass;
	std::vector< cv::Rect > incorrectClass;

	float detectionRate;
	float falsePositiveRate;
	float falseNegativeRate;
};

// string to correct image paths
const std::string relPath = "";

// print results in image
bool printInImage = true;

int buildStructure( std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >& baseStruc,
		std::vector< std::pair< std::string, std::pair< cv::Rect, std::string > > >& signs );
void printStructure( std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >& baseStruc );

void evaluate( std::map< std::string, eval_result >& evaluation,
		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >& baseStruc,
		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >& evalStruc );
bool acceptRect( cv::Rect a, cv::Rect b);
void getRects( std::vector< std::pair< cv::Rect, std::string > >& elements,  std::vector< cv::Rect >& out );
void printEvaluation( std::map< std::string, eval_result >& evaluation, int nSigns );
void drawRects( cv::Mat& image, std::vector< cv::Rect > rects, cv::Scalar color );

int main( int argc, char** argv )
{
	int status = EXIT_SUCCESS;
	if( argc >= 3 )
	{
		for(int i = 1; i < argc; i++ )
		{
			std::string opt = std::string(argv[i]);
			if( opt == "-r" )
			{
				fileToEvaluate = std::string(argv[i+1]);
				i++;
			}
			if( opt == "-gt" )
			{
				benchmarkFile = std::string(argv[i+1]);
				i++;
			}
			if( opt == "-d" )
			{
				imageDir = std::string(argv[i+1]);
				i++;
			}
		}

		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > > gtObjs;
		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > > bcObjs;

		std::vector< std::pair< std::string, std::pair< cv::Rect, std::string > > > objs;
		std::vector< std::pair< std::string, std::pair< cv::Rect, std::string > > > pobjs;

		std::map< std::string, eval_result > evaluation;

		// base benchmark ground truth
		FilesHelper::parseGroundTruth( benchmarkFile, gtDelimiter, objs );

		// ground truth to evaluate
		FilesHelper::parseGroundTruth( fileToEvaluate, rsDelimiter, pobjs );

		int nObjs = buildStructure( gtObjs, objs );
		buildStructure( bcObjs, pobjs );

		std::cout << std::endl << "In the groundTruth file:" << std::endl;
		printStructure( gtObjs );
		std::cout << std::endl << "In the results to evaluate:" << std::endl;
		printStructure( bcObjs );

		evaluate( evaluation, gtObjs, bcObjs );
		printEvaluation( evaluation, nObjs );
	}
	else
	{
		status = EXIT_FAILURE;
	}

	return status;
}

int buildStructure( std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >& baseStruc,
		std::vector< std::pair< std::string, std::pair< cv::Rect, std::string > > >& signs )
{
	int nSigns = 0;
	std::vector< std::pair< std::string, std::pair< cv::Rect, std::string > > >::iterator it = signs.begin();
	for( ; it != signs.end(); it++ )
	{
		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >::iterator element =
				baseStruc.find( it->first );

		if( element == baseStruc.end() )
		{
			std::vector< std::pair< cv::Rect, std::string > > vectorSigns;
			vectorSigns.push_back( it->second ); nSigns++;
			baseStruc.insert(
					std::pair< std::string, std::vector< std::pair< cv::Rect, std::string > > >( it->first, vectorSigns ) );
		}
		else
		{
			element->second.push_back( it->second ); nSigns++;
		}
	}

	return nSigns;
}

// TODO ensure that referenced filenames are equal
void evaluate( std::map< std::string, eval_result >& evaluation,
		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >& baseStruc,
		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >& evalStruc )
{
	std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >::iterator it = evalStruc.begin();
	for( ; it != evalStruc.end(); it++ )
	{
		// necessary to parse file name
		unsigned found = it->first.find_last_of("/\\");
		std::string fileName = it->first.substr(found+1);
		// expected signs for this image
		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >::iterator element =
				baseStruc.find( fileName );

		// signs already identified
		std::vector< cv::Rect > idtSigns;

		// auxiliary set with all expected signs, to calculate false negatives
		std::vector< cv::Rect > fneg;

		// structure to store evaluation results
		eval_result evr = eval_result();

		if( element != baseStruc.end() )
		{
			getRects( element->second, fneg );

			std::vector< std::pair< cv::Rect, std::string > >::iterator itev = it->second.begin();
			for( ; itev != it->second.end(); itev++ )
			{
				bool foundMatch = false;
				std::vector< std::pair< cv::Rect, std::string > >::iterator itgt = element->second.begin();
				for( ; !foundMatch && itgt != element->second.end(); itgt++ )
				{
					if( std::find(idtSigns.begin(), idtSigns.end(), itgt->first ) == idtSigns.end()
							&& acceptRect( itgt->first, itev->first ) )
					{
						// correct
						evr.correctMatches.push_back( itev->first );
						foundMatch = true;
						idtSigns.push_back( itgt->first );

						std::vector< cv::Rect >::iterator sf =
								std::find( fneg.begin(), fneg.end(), itgt->first );
						if( sf != fneg.end() )
						{
							fneg.erase( sf );
						}
						foundMatch = true;
						if(itev->second == itgt->second)
						{
							evr.correctClass.push_back(itev->first);
						}
						else
						{
							evr.incorrectClass.push_back(itev->first);
						}
					}
				}

				if( foundMatch == false  )
				{
					// false positive
					evr.falsePositives.push_back( itev->first );
				}
			}
		}
		else
		{
			//they're all false positives
			std::vector< std::pair< cv::Rect, std::string > >::iterator itev = it->second.begin();
			for( ; itev != it->second.end(); itev++ )
			{
				evr.falsePositives.push_back( itev->first );
			}
		}

		evr.falseNegatives.resize(fneg.size());
		std::copy( fneg.begin(), fneg.end(), evr.falseNegatives.begin() );

		evr.detectionRate = (float) evr.correctMatches.size() / (float) element->second.size();
		evr.falsePositiveRate = (float) evr.falsePositives.size() / (float) it->second.size();
		evr.falseNegativeRate = (float) evr.falseNegatives.size() / (float) element->second.size();

		evaluation.insert( std::pair< std::string, eval_result >( fileName, evr ) );
	}

	it = baseStruc.begin();
	for( ; it != baseStruc.end(); it++ )
	{
		// necessary to parse file name
		std::string fileName = relPath + it->first;
		// expected signs for this image
		std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >::iterator element =
				evalStruc.find( fileName );

		if( element == evalStruc.end() )
		{
			eval_result evr = eval_result();
			std::vector< cv::Rect > fneg;
			getRects(it->second, fneg);

			evr.falseNegatives = fneg;
			evr.detectionRate = 0.0f;
			evr.falsePositiveRate = 0.0f;
			evr.falseNegativeRate = 1.0f;

			evaluation.insert( std::pair< std::string, eval_result >( fileName, evr ) );
		}
	}
}

bool acceptRect( cv::Rect a, cv::Rect b)
{
	cv::Rect intersect = a & b;
	float unionArea = a.area() + b.area() - intersect.area();

	return ( intersect.area() / unionArea ) > 0.5;
}

void printStructure( std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >& baseStruc )
{
	std::cout << "Found " << baseStruc.size() << " images to evaluate." << std::endl;

	std::map< std::string, std::vector< std::pair< cv::Rect, std::string > > >::iterator it = baseStruc.begin();
	for( ; it != baseStruc.end(); it++ )
	{
		std::cout << "In the image file, '" << it->first  << "', there are the following signs:" << std::endl;
		std::vector< std::pair< cv::Rect, std::string > >::iterator itv = it->second.begin();
		for( ; itv != it->second.end(); itv++ )
		{
			std::cout << '\t' << itv->first.x << " " << itv->first.y << " " << itv->first.width << " " << itv->first.height;
			if(!itv->second.empty())
				std::cout << " with class " << itv->second;

			std::cout << std::endl;
		}
	}
}

void printEvaluation( std::map< std::string, eval_result >& evaluation, int nSigns )
{
	int numberDetections = 0;
	int numberMatches = 0;
	int numberCC = 0;
	int numberIC = 0;
	int numberFP = 0;
	int numberFN = 0;

	cv::Mat result;

	std::cout << "Found " << evaluation.size() << " images evaluated." << std::endl;
	for( std::map< std::string, eval_result >::iterator it = evaluation.begin(); it != evaluation.end(); it++ )
	{
		if( printInImage && !imageDir.empty() ) result = cv::imread( imageDir + "/" + (*it).first );

		std::cout << "In the image file, '" << (*it).first  << "', the following results were obtained:" << std::endl;
		std::cout << "Overall results:" << std::endl << "\t Detection Rate = " << (*it).second.detectionRate <<
				"\t False Positive Rate = " << (*it).second.falsePositiveRate <<
				"\t False Negative Rate = " << (*it).second.falseNegativeRate << std::endl;

		//-----------------------------Correct_Matches---------------------------------------------//
		numberDetections += (*it).second.correctMatches.size();
		numberMatches += (*it).second.correctMatches.size();
		std::cout << '\t' << (*it).second.correctMatches.size() << " correct Matches: ";
		for( std::vector< cv::Rect >::iterator itv = (*it).second.correctMatches.begin();
				itv != (*it).second.correctMatches.end(); itv++ )
		{
			std::cout << '\t' << itv->x << " " << itv->y << " " << itv->width << " " << itv->height;
		}
		std::cout << std::endl;

		//-----------------------------False_Positives---------------------------------------------//
		numberDetections += (*it).second.falsePositives.size();
		numberFP += (*it).second.falsePositives.size();
		std::cout << '\t' << (*it).second.falsePositives.size() << " false positives: ";
		for( std::vector< cv::Rect >::iterator itv = (*it).second.falsePositives.begin();
				itv != (*it).second.falsePositives.end(); itv++ )
		{
			std::cout << '\t' << itv->x << " " << itv->y << " " << itv->width << " " << itv->height;
		}
		std::cout << std::endl;

		//-----------------------------False_Negatives---------------------------------------------//
		numberFN += (*it).second.falseNegatives.size();
		std::cout << '\t' << (*it).second.falseNegatives.size() << " false negatives: ";
		for( std::vector< cv::Rect >::iterator itv = (*it).second.falseNegatives.begin();
				itv != (*it).second.falseNegatives.end(); itv++ )
		{
			std::cout << '\t' << itv->x << " " << itv->y << " " << itv->width << " " << itv->height;
		}

		std::cout << std::endl;
		//---------------------------Classification----------------------------------------------//
		numberCC += (*it).second.correctClass.size();
		std::cout << '\t' << (*it).second.correctClass.size() << " correctly classified: ";
		for( std::vector< cv::Rect >::iterator itv = (*it).second.correctClass.begin();
				itv != (*it).second.correctClass.end(); itv++ )
		{
			std::cout << '\t' << itv->x << " " << itv->y << " " << itv->width << " " << itv->height;
		}
		std::cout << std::endl;

		numberIC += (*it).second.incorrectClass.size();
		std::cout << '\t' << (*it).second.incorrectClass.size() << " incorrectly classified: ";
		for( std::vector< cv::Rect >::iterator itv = (*it).second.incorrectClass.begin();
				itv != (*it).second.incorrectClass.end(); itv++ )
		{
			std::cout << '\t' << itv->x << " " << itv->y << " " << itv->width << " " << itv->height;
		}
		std::cout << std::endl;

		if( printInImage && !imageDir.empty() )
		{
			drawRects( result, (*it).second.correctMatches, cv::Scalar(0,255,0) );
			drawRects( result, (*it).second.falsePositives, cv::Scalar(255,0,0) );
			drawRects( result, (*it).second.falseNegatives, cv::Scalar(0,0,255) );
			cv::namedWindow( "result", CV_WINDOW_NORMAL );
			cv::imshow( "result", result );

			cv::waitKey(0);
		}
	}

	std::cout << std::endl << "Overall results for complete image set, containing " << nSigns  << " traffic signs."<< std::endl
			<< "\t Detection Rate " << "(found " << numberMatches << " traffic signs out of " << nSigns
			<< ") = " << (float) numberMatches / (float) nSigns << std::endl <<
				"\t False Positive Rate " << "(there were " << numberFP << " false positives out of " << numberDetections <<
				" potencial signs) = " << (float) numberFP / (float) numberDetections << std::endl <<
				"\t False Negative Rate " << "(missed " << numberFN << " traffic signs out of " << nSigns << ") = " <<
				(float) numberFN / (float) nSigns << std::endl;
	std::cout << "Correctly classified " << ((float)numberCC / numberMatches)*100 << "% of detected signs." << std::endl;
}

void drawRects( cv::Mat& image, std::vector< cv::Rect > rects, cv::Scalar color )
{

	for( std::vector< cv::Rect >::iterator sit = rects.begin(); sit != rects.end(); sit ++ )
	{
		cv::Point2f rect_points[4];
		rect_points[0] = cv::Point2f( sit->x, sit->y );
		rect_points[1] = cv::Point2f( sit->x, sit->y + sit->width );
		rect_points[2] = cv::Point2f( sit->x + sit->height, sit->y + sit->width );
		rect_points[3] = cv::Point2f( sit->x + sit->height, sit->y );
		for( int j = 0; j < 4; j++ )
			line( image, rect_points[j], rect_points[(j+1)%4], color, 2, 8 );
	}
}

void getRects( std::vector< std::pair< cv::Rect, std::string > >& elements,  std::vector< cv::Rect >& out )
{
	std::vector< std::pair< cv::Rect, std::string > >::iterator it = elements.begin();
	for( ; it != elements.end(); it++ )
	{
		out.push_back(it->first);
	}
}
