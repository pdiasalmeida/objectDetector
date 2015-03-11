#include "PostgresDataStore.hpp"

#include "../auxiliar/FilesHelper.hpp"

#include <iostream>

PostgresDataStore::PostgresDataStore( rapidxml::xml_node<> *node, std::string name, std::string processDir ):
	DataStore(name)
{
	_processDir = processDir;

	_dbName = std::string(node->first_node("dbName")->value());
	_dbUser = std::string(node->first_node("dbUser")->value());
	_dbUserPass = std::string(node->first_node("dbUserPass")->value());
	_dbHostAddr = std::string(node->first_node("dbHostAddr")->value());
	_dbPort = std::string(node->first_node("dbPort")->value());

	std::stringstream connString;
	connString << "dbname=" << _dbName << " user=" << _dbUser << " password=" << _dbUserPass
			<< " hostaddr=" << _dbHostAddr << " port=" << _dbPort;
	_connString = connString.str();

	init();
}

void PostgresDataStore::init()
{
	try{
		_conn = new pqxx::connection(_connString);
		if( _conn->is_open() )
		{
			std::cout << "Opened database successfully: " << _conn->dbname() << std::endl;

			std::stringstream commIDT;
			commIDT << "SELECT id_track FROM public.trackvideo "
					<< "WHERE nome LIKE '" << FilesHelper::getLeafDirName(_processDir) << "%'";
			_trackID = getInt(commIDT.str());

			std::stringstream commIDV;
			commIDV << "SELECT id FROM public.video "
					<< "WHERE nome LIKE '" << FilesHelper::getLeafDirName(_processDir) << "%'";
			_videoID = getInt(commIDV.str());

			std::cout << "Initialized postgres store with track id: " << _trackID <<
					" and video id: " << _videoID << std::endl;
		}
		else
		{
			std::cout << "Can't open database" << std::endl;
		}
	}
	catch( const std::exception &e )
	{
		std::cerr << e.what() << std::endl;
	}
}

int PostgresDataStore::getInt( std::string command )
{
	int result = -1;

	pqxx::nontransaction nt(*_conn);
	pqxx::result rs( nt.exec( command ));

	if(!rs.empty())
	{
		pqxx::result::const_iterator c = rs.begin();
		result = c[0].as<int>();
	}

	return result;
}

std::map< std::string, std::vector<std::pair<cv::Rect, std::string> > > PostgresDataStore::getAnnotations( std::string processDir )
{
	std::map< std::string, std::vector<std::pair<cv::Rect, std::string> > > result;

	std::stringstream commMeta;
	commMeta << "SELECT frame,x,y,width,height,classe FROM public.trackframes "
						<< "WHERE id_track=" << _trackID;

	pqxx::nontransaction nt(*_conn);
	pqxx::result rs( nt.exec( commMeta ));

	if(!rs.empty())
	{
		std::cout << "Got " << rs.size() << " results." << std::endl;
		pqxx::result::const_iterator c = rs.begin();
		for( ; c != rs.end(); c++ )
		{
			std::map< std::string, std::vector<std::pair<cv::Rect, std::string> > >::iterator it =
					result.find(c[0].as<std::string>());

			cv::Rect object = cv::Rect(c[1].as<int>(), c[2].as<int>(), c[3].as<int>(), c[4].as<int>());
			if( it == result.end() )
			{
				std::vector<std::pair<cv::Rect, std::string> > objs;
				objs.push_back( std::make_pair(object, c[5].as<std::string>()) );

				result.insert( std::make_pair(c[0].as<std::string>(), objs) );
			}
			else
			{
				it->second.push_back( std::make_pair(object, c[5].as<std::string>()) );
			}
		}
	}

	return result;
}

void PostgresDataStore::writeAnnotations( std::string name, std::string classe, std::vector< cv::Rect > objs )
{
	std::vector< cv::Rect >::iterator wf = objs.begin();
	for( ; wf != objs.end(); wf++ )
	{
		std::stringstream ss;
		ss << name << "," << classe << "," << wf->x << "," << wf->y << ","
				<< wf->width << "," << wf->height;

		ss << std::endl;

	}
}

void PostgresDataStore::writeAnnotations( std::string name, std::vector< std::pair < cv::Rect, std::string > > objs )
{
	std::vector< std::pair < cv::Rect, std::string > >::iterator wf = objs.begin();
	for( ; wf != objs.end(); wf++ )
	{
		std::stringstream ss;

		ss << "INSERT INTO public.trackframes(id_track,id_video,frame,x,y,width,height,classe) ";
		ss << "VALUES (" << _trackID << "," << _videoID << ",'" << name << "',";
		ss << wf->first.x << "," << wf->first.y << ","
				<< wf->first.width << "," << wf->first.height << ",'" << wf->second << "');";
		try
		{
			pqxx::work trans(*_conn);

			trans.exec( ss );
			trans.commit();
		}
		catch( const std::exception &e )
		{
			std::cerr << e.what() << std::endl;
		}
	}
}

PostgresDataStore::~PostgresDataStore()
{
	_conn->disconnect();
}
