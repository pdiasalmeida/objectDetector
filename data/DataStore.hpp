#ifndef DATASTORE_HPP_
#define DATASTORE_HPP_

#include <opencv2/core/core.hpp>

class DataStore
{
public:
	virtual void writeAnnotations( std::string name, std::string classe, std::vector< cv::Rect > objs ) = 0;
	virtual void writeAnnotations( std::string name, std::vector< std::pair < cv::Rect, std::string > > objs ) = 0;

	virtual std::map< std::string, std::vector<std::pair<cv::Rect, std::string> > > getAnnotations( std::string processDir ) = 0;

	virtual ~DataStore(){}

protected:
	std::string _name;

	DataStore(std::string name) {_name = name;}
};

#endif
