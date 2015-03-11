#ifndef FILEDATASTORE_HPP_
#define FILEDATASTORE_HPP_

#include "DataStore.hpp"

class FileDataStore : DataStore
{
public:
	FileDataStore( std::string name, std::string processDir, std::string outDir);

	virtual void writeAnnotations( std::string name, std::string classe, std::vector< cv::Rect > objs );
	virtual void writeAnnotations( std::string name, std::vector< std::pair < cv::Rect, std::string > > objs );

	virtual std::map< std::string, std::vector<std::pair<cv::Rect, std::string> > > getAnnotations( std::string processDir );

	~FileDataStore();

protected:
	std::string _processDir;
	std::string _outDir;
};

#endif
