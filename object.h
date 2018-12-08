#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>

class Object
{
private:
	int classID;
    std::string className;
    std::vector<float> features;


public:

    Object(const std::string &className, const std::vector<float> &features) :classID(-1), className(className), features(features)
    {
    }

    std::string getClassName() const;
    size_t getFeaturesNumber() const;
    std::vector<float> &getFeatures();
    int getClassID();
    void selectIndexes(std::vector<unsigned int> indexes);
};



#endif // OBJECT_H
