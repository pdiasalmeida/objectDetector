#ifndef FILEWRITER_HPP_
#define FILEWRITER_HPP_

#include <map>
#include <string>
#include <fstream>

class FileWriter
{
public:
	static void openFile( std::string dirPath, std::string name );
	static void writeToFile( std::string file, std::string content );

	static void close();

	~FileWriter();

protected:
	static std::map< std::string, std::ofstream* > _files;

private:
	FileWriter();
};

#endif /* FILEWRITER_HPP_ */