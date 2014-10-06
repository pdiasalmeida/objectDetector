#include "libs/rapidxml/rapidxml.hpp"

#include "auxiliar/FilesHelper.hpp"
#include "auxiliar/FileWriter.hpp"

#include "validators/SVMValidator.hpp"
#include "validators/ColorValidator.hpp"
#include "validators/HistValidator.hpp"

#include "classifier/TemplateClassifier.hpp"
#include "classifier/SVMClassifier.hpp"

#include "detectors/CascadeClassifierDetector.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <tesseract/baseapi.h>

#include <fstream>
#include <sys/stat.h>

std::string processDir;
std::string outDir;
std::vector< std::string > fileNames;
std::vector< std::string > processed;
std::vector< std::string > toProcess;
std::vector< std::string > validExtensions;

bool exclusive = false;
bool debug = false;
bool silent = false;
bool saveThumbs = false;

double aggreScale = 0.0;

std::vector< Detector* > _detectors;

bool validateLetters( cv::Mat pImage )
{
	bool result = false;

	tesseract::TessBaseAPI tess;
	tess.Init( NULL, "por", tesseract::OEM_DEFAULT);
	tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	tess.SetPageSegMode( tesseract::PSM_SINGLE_BLOCK);

	cv::Mat gray;
	cv::cvtColor( pImage, gray, CV_BGR2GRAY );
	blur( gray, gray, cv::Size(3,3) );

	cv::Mat _thresh;
	threshold( gray, _thresh, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY_INV );


	tess.SetImage( (uchar*) _thresh.data, _thresh.cols, _thresh.rows, 1, _thresh.cols );
	char* outc = tess.GetUTF8Text();

	std::string out(outc);

	if( (out.length() != 0) &&
			abs((FilesHelper::countAlpha(out) - FilesHelper::countDigit(out)) >= 5) )
	{
		if(debug) std::cout << "Plate rejected by letter filter." << std::endl;
	}
	else
		result = true;

	return result;
}

int getPreviousProgress()
{
	FilesHelper::getFilesInDirectory( outDir, processed, validExtensions );

	std::sort(fileNames.begin(), fileNames.end());
	std::sort(processed.begin(), processed.end());

	std::set_difference( fileNames.begin(), fileNames.end(), processed.begin(),
			processed.end(), std::back_inserter(toProcess) );


	return processed.size();
}

void saveBlurredResults( cv::Mat image, std::string name, std::vector< cv::Rect > foundplates )
{
	if(debug) std::cout << "Saving detector results ..." << std::endl;

	std::vector< cv::Rect >::iterator it = foundplates.begin();
	for( ; it != foundplates.end(); it++ )
	{
		cv::Mat pplate = image(*it);
		cv::GaussianBlur(pplate,pplate,cv::Size(15,15), 100.0);
	}
	cv::imwrite( outDir+"/"+FilesHelper::getFileName(name), image );

	if(debug) std::cout << "Finished saving detector results." << std::endl;
}

void saveDrawnResults( cv::Mat image, std::string name, std::vector< cv::Rect > foundplates )
{
	if(debug) std::cout << "Saving detector results ..." << std::endl;

	std::vector< cv::Rect >::iterator it = foundplates.begin();
	for( ; it != foundplates.end(); it++ )
	{
		cv::rectangle(image, *it, cv::Scalar(255 ,0 ,0), 2);
	}
	cv::imwrite( outDir+"/"+FilesHelper::getFileName(name), image );

	if(debug) std::cout << "Finished saving detector results." << std::endl;
}

void writeAnnotations( std::string name, std::string classe, std::vector< cv::Rect > objs )
{
	std::vector< cv::Rect >::iterator wf = objs.begin();
	for( ; wf != objs.end(); wf++ )
	{
		std::stringstream ss;
		ss << name << "," << classe << "," << wf->x << "," << wf->y << ","
				<< wf->width << "," << wf->height;

		ss << std::endl;
		FileWriter::writeToFile( FilesHelper::getLeafDirName(processDir)+".dat", ss.str() );
	}
}

void writeAnnotations( std::string name, std::vector< std::pair < cv::Rect, std::string > > objs )
{
	std::vector< std::pair < cv::Rect, std::string > >::iterator wf = objs.begin();
	for( ; wf != objs.end(); wf++ )
	{
		std::stringstream ss;
		ss << name << "," << wf->first.x << "," << wf->first.y << ","
				<< wf->first.width << "," << wf->first.height << "," << wf->second;

		ss << std::endl;
		FileWriter::writeToFile( FilesHelper::getLeafDirName(processDir)+".dat", ss.str() );
	}
}

int parseConfig()
{
	int status = EXIT_SUCCESS;
	std::ifstream configFile;
	configFile.open ("config.xml");

	if( configFile.is_open() )
	{
		configFile.seekg( 0, configFile.end );
		int length = configFile.tellg();
		configFile.seekg( 0, configFile.beg );

		char* buffer = new char[length];
		configFile.read( buffer, length );

		rapidxml::xml_document<> doc;
		doc.parse<rapidxml::parse_full>(buffer);

		rapidxml::xml_node<> *config = doc.first_node("config");
		if( config )
		{
			exclusive = (std::string(config->first_node("exclusive")->value()) == "1") ? true : false;
			debug = (std::string(config->first_node("debug")->value()) == "1") ? true : false;
			silent = (std::string(config->first_node("silent")->value()) == "1") ? true : false;
			saveThumbs = (std::string(config->first_node("saveThumbs")->value()) == "1") ? true : false;

			FilesHelper::debug = debug;

			rapidxml::xml_node<> *detector = doc.first_node("detector");
			for( ; detector; detector = detector->next_sibling("detector") )
			{
				Detector* d;
				std::string type = std::string(std::string(detector->first_node("type")->value()));
				std::string name = std::string(std::string(detector->first_node("name")->value()));

				bool aggregate = false;
				rapidxml::xml_node<> *aggre = detector->first_node("aggregate");
				if(aggre)
				{
					aggregate = true;
					aggreScale = atof(aggre->first_node("scale")->value());
				}

				bool annotate = false;
				rapidxml::xml_node<> *anno = detector->first_node("annotate");
				if(anno)
				{
					annotate = (std::string(anno->value()) == "1") ? true : false;
				}

				int output = 2;
				rapidxml::xml_node<> *outpt = detector->first_node("output");
				if(outpt)
				{
					std::string outp = std::string(outpt->value());
					if( outp=="Blurr" ) output = 0;
					else if( outp=="Draw" ) output =1;
				}

				if( type == "cascade" )
				{
					d = (Detector*) new CascadeClassifierDetector(
							detector, name, processDir, outDir, output, aggregate, annotate, debug, saveThumbs);

					_detectors.push_back(d);
				}

				rapidxml::xml_node<> *validator = detector->first_node("validator");
				for( ; validator; validator = validator->next_sibling("validator") )
				{
					Validator* v;
					std::string type = std::string(validator->first_node("type")->value());
					std::string name = std::string(validator->first_node("name")->value());
					if( type == "svm" )
					{
						v = (Validator*) new SVMValidator(validator, name, debug);
					}
					else if( type == "color" )
					{
						v = (Validator*) new ColorValidator(validator, name, debug);
					}
					else if( type == "histogram" )
					{
						v = (Validator*) new HistValidator(validator, name, debug);
					}

					d->addValidator(v);
				}

				rapidxml::xml_node<> *classifier = detector->first_node("classifier");
				for( ; classifier; classifier = classifier->next_sibling("classifier") )
				{
					Classifier* c;
					std::string type = std::string(std::string(classifier->first_node("type")->value()));
					std::string name = std::string(std::string(classifier->first_node("name")->value()));

					if( type == "template" )
					{
						TemplateClassifier* tc = new TemplateClassifier(classifier, name, debug);

						rapidxml::xml_node<> *classe = classifier->first_node("class");
						for( ; classe; classe = classe->next_sibling() )
						{
							std::string className = std::string(classe->first_node("name")->value());
							std::string classPath = std::string(classe->first_node("path")->value());

							tc->addClass(className, classPath);
						}
						c = (Classifier*) tc;
					}
					if( type == "svm" )
					{
						SVMClassifier* sc = new SVMClassifier(classifier, name, debug);

						rapidxml::xml_node<> *classe = classifier->first_node("class");
						for( ; classe; classe = classe->next_sibling("class") )
						{
							std::string className = std::string(classe->first_node("name")->value());
							int classId = atoi(classe->first_node("id")->value());

							sc->addClass(className, classId);
						}
						c = (Classifier*) sc;
					}

					d->setClassifier(c);
				}
			}

			delete(buffer);
		}
	}
	else
	{
		std::cout << "Missing or invalid configuration file. Exiting" << std::endl;
		status = EXIT_FAILURE;
	}

	configFile.close();

	return status;
}

cv::Rect expand(cv::Rect rect)
{
	int dx = rect.width*aggreScale;
	int dy = rect.height*aggreScale;
	int nx = ((rect.x - dx)>0)?(rect.x - dx):0;
	int ny = ((rect.y - dy)>0)?(rect.y - dy):0;

	int nw = rect.width+(rect.width*aggreScale)*2;
	int nh = rect.height+(rect.height*aggreScale)*2;

	cv::Rect result = cv::Rect( nx, ny, nw, nh );

	return result;
}

void getExternalContours( std::vector<std::vector<cv::Point> > const& contours,
		std::vector<cv::Vec4i> const& hierarchy, int const idx, std::vector< cv::Rect >& out )
{
	if( contours.size()>0 )
	{
		for( int i = idx; i >= 0; i = hierarchy[i][0] )
		{
			out.push_back(cv::boundingRect(contours[i]));

			for( int j = hierarchy[i][2]; j >= 0; j = hierarchy[j][0] )
			{
				getExternalContours( contours, hierarchy, hierarchy[j][2], out);
			}
		}
	}
}

std::vector< cv::Rect > aggregate( cv::Mat image, std::vector< cv::Rect > objects )
		{
	std::vector< cv::Rect > result;
	cv::Mat mask = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);

	std::vector< cv::Rect >::iterator it = objects.begin();
	for( ; it != objects.end(); it++ )
	{
		cv::rectangle(mask, expand(*it), cv::Scalar(255 ,255 ,255), 2);
	}

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	getExternalContours(contours, hierarchy, 0, result);

	return result;
		}

std::vector< cv::Rect > exclude( std::vector< cv::Rect > objs, std::vector< cv::Rect > base )
		{
	std::vector< cv::Rect > result;

	if(exclusive)
	{
		std::vector< cv::Rect >::iterator oit = objs.begin();
		for( ; oit != objs.end(); oit++)
		{
			bool valid = true;
			std::vector< cv::Rect >::iterator bit = base.begin();
			for( ; valid && bit != base.end(); bit++)
			{
				valid = valid && ( ((*oit)&(*bit)).size().area() <= 0 );
			}
			if(valid) result.push_back(*oit);
		}
	}
	else
	{
		result = objs;
	}

	return result;
}

std::vector< std::pair< cv::Rect, std::string > > classify( Detector* det, cv::Mat frame, std::vector< cv::Rect> out )
{
	std::vector< std::pair< cv::Rect, std::string > > result;

	std::vector< cv::Rect>::iterator it = out.begin();
	for( ; it != out.end(); it++ )
	{
		cv::Mat roi = frame(*it);
		std::string classe = det->_classifier->getClass(roi);

		cv::putText(frame, classe, cv::Point(it->x, it->y-5), CV_FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 0, 255));

		result.push_back( std::make_pair(*it, classe) );
	}

	return result;
}

int processDirectory()
{
	int status = EXIT_SUCCESS;
	if( processDir.empty() )
	{
		if(!silent) std::cout << "No processing directory given. Proceeding with benchmark." << std::endl;
		processDir = "./benchmark";
	}
	if( outDir.empty() )
	{
		outDir = processDir + "_out";
		if(!silent) std::cout << "No output directory given. Saving results to " << outDir << "." << std::endl;

		mkdir(outDir.c_str(), 0777);
	}
	FileWriter::openFile(outDir,FilesHelper::getLeafDirName(processDir)+".dat");

	status = parseConfig();

	FilesHelper::getFilesInDirectory( processDir, fileNames, validExtensions );

	int n = getPreviousProgress();
	std::vector< std::string >::iterator itFN = toProcess.begin();
	for( int total = fileNames.size() ; itFN != toProcess.end(); itFN++ )
	{
		if(!silent) std::cout << "\nProcessing file " << *itFN << " ..." << std::endl;

		cv::Mat frame = cv::imread( processDir + "/" + *itFN );
		std::vector< cv::Rect > inImage;

		std::vector<Detector*>::iterator dit = _detectors.begin();
		for( ; dit != _detectors.end(); dit ++ )
		{
			std::vector< cv::Rect > objects = (*dit)->detect(frame, *itFN);
			std::vector< cv::Rect > out = (*dit)->isAggregate() ? exclude(aggregate(frame,objects), inImage) :
					exclude(objects, inImage);

			inImage.insert(inImage.end(), out.begin(), out.end());

			if((*dit)->doAnnotate() && (*dit)->_classifier)
			{
				std::vector< std::pair< cv::Rect, std::string > > final = classify( *dit, frame, out );
				writeAnnotations( *itFN, final );
			}
			else if((*dit)->doAnnotate())
			{
				writeAnnotations( *itFN, (*dit)->getName(), out );
			}

			switch((*dit)->getOutputMethod())
			{
			case 0:
				saveBlurredResults( frame, *itFN, out );
				break;
			case 1:
				saveDrawnResults( frame, *itFN, out );
				break;
			}
		}
		n++;

		std::cout << "\tFinished processing file " << *itFN << ". " << std::endl
				<< (float) n/total * 100 << "% complete." << std::endl;
	}

	return status;
}

int processImage( std::string imagePath )
{
	int status = EXIT_SUCCESS;
	if( outDir.empty() )
	{
		outDir = "out";
		if(!silent) std::cout << "No output directory given. Saving results to " << outDir << "." << std::endl;
		mkdir(outDir.c_str(), 0777);
	}

	if( FilesHelper::fileAvailable(FilesHelper::getDirName(imagePath), FilesHelper::getFileName(imagePath), validExtensions) )
	{
		status = parseConfig();

		cv::Mat frame = cv::imread( imagePath );
		std::vector< cv::Rect > inImage;

		std::vector<Detector*>::iterator dit = _detectors.begin();
		for( ; dit != _detectors.end(); dit ++ )
		{
			std::vector< cv::Rect > objects = (*dit)->detect(frame, imagePath);
			std::vector< cv::Rect > out = (*dit)->isAggregate()?exclude(aggregate(frame,objects), inImage) :
					exclude(objects, inImage);

			inImage.insert(inImage.end(), out.begin(), out.end());

			if((*dit)->doAnnotate() && (*dit)->_classifier)
			{
				std::vector< std::pair< cv::Rect, std::string > > final = classify( *dit, frame, out );
				writeAnnotations( imagePath, final );
			}
			else if((*dit)->doAnnotate())
			{
				writeAnnotations( imagePath, (*dit)->getName(), out );
			}

			switch((*dit)->getOutputMethod())
			{
			case 0:
				saveBlurredResults( frame, imagePath, out );
				break;
			case 1:
				saveDrawnResults( frame, imagePath, out );
				break;
			}
		}
	}
	else
	{
		std::cout << "Unable to access requested file '" << processDir + "/" + imagePath
				<< "' or incompatible extension. Terminating process." << std::endl;
		status = EXIT_FAILURE;
	}

	return status;
}

void processFromFile( std::string path )
{
	std::map< std::string, std::vector< std::string > > kValues;
	FilesHelper::parseKeyValue( path, ',', kValues );
	FilesHelper::printKeyValue(kValues);
}

int main( int argc, char** argv )
{
	int status = EXIT_SUCCESS;
	std::string imagePath;
	std::string infoFile;

	validExtensions.push_back("jpg");
	validExtensions.push_back("png");
	validExtensions.push_back("ppm");

	if( argc >= 2 )
	{
		for(int i = 1; i < argc; i++ )
		{
			std::string opt = std::string(argv[i]);
			if( opt == "-d" )
			{
				processDir = std::string(argv[i+1]);
				i++;
			}
			if( opt == "-o" )
			{
				outDir = std::string(argv[i+1]);
				mkdir(outDir.c_str(), 0777);
				i++;
			}
			if( opt == "-i" )
			{
				imagePath = std::string(argv[i+1]);
				i++;
			}
			if( opt == "-v" )
			{
				debug = true;
			}
			if( opt == "-s" )
			{
				silent = true;
			}
			if( opt == "-st" )
			{
				saveThumbs = true;
			}
			if( opt == "-f" )
			{
				infoFile = std::string(argv[i+1]);
			}
		}
	}

	if( imagePath.empty() && infoFile.empty() )
	{
		status = processDirectory();
	}
	else if(!imagePath.empty())
	{
		status = processImage(imagePath);
	}
	else if(!infoFile.empty())
	{
		processFromFile(infoFile);
	}

	FileWriter::close();

	return status;
}
