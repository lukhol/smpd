#ifndef CLASSIFIERNM_H
#define CLASSIFIERNM_H

#include <vector>
#include <iostream>
#include <algorithm>
#include "object.h"
#include <iostream>
#include <QString>
#include "database.h"

class ClassifierNM
{
public:
    ClassifierNM(std::vector<Object> vectorOfObjects, Database& database);
    void prepareForNM();
    QString whichClass(Object verifyingObject);

private:
    std::vector<Object> trainBase;
    int sizeOfTrainBase;
    Database database;
    std::vector<std::pair<std::string, std::vector<Object> > > divisionIntoClasses;
    std::map<std::string, std::vector<float> > mapOfClassNameAndMean;
};

#endif // CLASSIFIERNM_H
