#ifndef DETECTOR_HPP_
#define DETECTOR_HPP_

#include "../validators/Validator.hpp"
#include "../classifier/Classifier.hpp"

#include <opencv2/core/core.hpp>

class Detector
{
public:
	virtual std::vector< cv::Rect > detect( cv::Mat image, std::string name ) = 0;

	std::string getName(){return _name;};

	void addValidator(Validator* v){_validators.push_back(v);};
	void setClassifier(Classifier* c){_classifier = c;};
	int getOutputMethod(){return _output;};
	int isAggregate(){return _aggregate;};
	int doAnnotate(){return _annotate;};

	virtual ~Detector(){}

	Classifier* _classifier;
protected:
	std::string _name;
	std::string _processDir;
	std::string _outDir;
	std::vector< Validator* > _validators;

	int _output;
	bool _aggregate;
	bool _annotate;

	bool debug;
	bool saveThumbs;

	Detector(std::string strName, std::string procD, std::string outD, int output, bool aggregate, bool annotate,
			bool db, bool saveThb): _name(strName), _processDir(procD), _outDir(outD)
	{_output = output;_aggregate = aggregate;_annotate = annotate;debug = db; saveThumbs = saveThb;_classifier=NULL;}
};

#endif
