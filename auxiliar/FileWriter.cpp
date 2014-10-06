#include "FileWriter.hpp"

#include <iostream>
#include <sys/stat.h>

std::map< std::string, std::ofstream* > FileWriter::_files;

void FileWriter::openFile( std::string dirPath, std::string name )
{
	std::ofstream* file = new std::ofstream;
	std::string fullPath = dirPath+"/"+name;
	mkdir(dirPath.c_str(), 0777);

	file->open(fullPath.c_str());

	FileWriter::_files.insert( std::pair< std::string, std::ofstream* >(name, file) );
}

void FileWriter::writeToFile( std::string file, std::string content )
{
	std::map< std::string, std::ofstream* >::iterator it = FileWriter::_files.find(file);
	if(it != FileWriter::_files.end() )
	{
		(*it->second) << content;
	}
}

void FileWriter::close()
{
	std::map< std::string, std::ofstream* >::iterator it = FileWriter::_files.begin();
	for( ; it != FileWriter::_files.end(); it++ )
	{
		it->second->close();
	}
}

FileWriter::~FileWriter()
{
	FileWriter::close();
}
