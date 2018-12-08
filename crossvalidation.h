#ifndef CROSSVALIDATION_H
#define CROSSVALIDATION_H

#include "database.h"
#include "classifiernn.h"
#include "enums.h"

class CrossValidation
{
public:
    CrossValidation(Database& database, int setNo);
    float run(ClassificationMethod method, int k);
private:
    Database database;
    int setNo;

    ClassifierNN* classifierNN;
};

#endif // CROSSVALIDATION_H
