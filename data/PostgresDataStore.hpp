#ifndef POSTGRESDATASTORE_HPP_
#define POSTGRESDATASTORE_HPP_

#include "DataStore.hpp"

#include "../libs/rapidxml/rapidxml.hpp"

#include <pqxx/pqxx>

class PostgresDataStore : DataStore
{
public:
	PostgresDataStore( rapidxml::xml_node<> *node, std::string name, std::string processDir);

	virtual void writeAnnotations( std::string name, std::string classe, std::vector< cv::Rect > objs );
	virtual void writeAnnotations( std::string name, std::vector< std::pair < cv::Rect, std::string > > objs );

	virtual std::map< std::string, std::vector<std::pair<cv::Rect, std::string> > > getAnnotations( std::string processDir );

	~PostgresDataStore();

protected:
	std::string _processDir;

	std::string _dbName;
	std::string _dbUser;
	std::string _dbUserPass;
	std::string _dbHostAddr;
	std::string _dbPort;

	std::string _connString;
	pqxx::connection* _conn;

	int _trackID;
	int _videoID;

private:
	void init();
	int getInt( std::string command );
};

#endif
