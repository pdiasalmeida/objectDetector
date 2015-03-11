#include "FileDataStore.hpp"

#include "../auxiliar/FilesHelper.hpp"
#include "../auxiliar/FileWriter.hpp"

FileDataStore::FileDataStore( std::string name, std::string processDir, std::string outDir ):
	DataStore(name)
{
	_processDir = processDir;
	_outDir = outDir;
}

std::map< std::string, std::vector<std::pair<cv::Rect, std::string> > > FileDataStore::getAnnotations( std::string processDir )
{
	std::map< std::string, std::vector<std::pair<cv::Rect, std::string> > > result;

	return result;
}


void FileDataStore::writeAnnotations( std::string name, std::string classe, std::vector< cv::Rect > objs )
{
	std::vector< cv::Rect >::iterator wf = objs.begin();
	std::stringstream ss;

	if(objs.empty())
	{
		ss << name;
		ss << std::endl;
	}
	for( ; wf != objs.end(); wf++ )
	{
		ss << name << "," << classe << "," << wf->x << "," << wf->y << ","
				<< wf->width << "," << wf->height;

		ss << std::endl;
	}
	FileWriter::writeToFile( FilesHelper::getLeafDirName(_processDir)+".dat", ss.str() );
	FileWriter::setFileContent( _outDir+"/"+FilesHelper::getFileNameNoExt(name)+".dat", ss.str() );
}

void FileDataStore::writeAnnotations( std::string name, std::vector< std::pair < cv::Rect, std::string > > objs )
{
	std::vector< std::pair < cv::Rect, std::string > >::iterator wf = objs.begin();
	std::stringstream ss;

	if(objs.empty()) ss << name;
	for( ; wf != objs.end(); wf++ )
	{
		ss << name << "," << wf->first.x << "," << wf->first.y << ","
				<< wf->first.width << "," << wf->first.height << "," << wf->second;

		ss << std::endl;
	}

	FileWriter::writeToFile( FilesHelper::getLeafDirName(_processDir)+".dat", ss.str() );
	FileWriter::setFileContent( _outDir+"/"+FilesHelper::getFileNameNoExt(name)+".dat", ss.str() );
}

FileDataStore::~FileDataStore(){}
