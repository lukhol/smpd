#ifndef CLASSIFIERNN_H
#define CLASSIFIERNN_H

#include "database.h"
#include "object.h"
#include "vector"
#include "QString"
#include <iostream>

class ClassifierNN
{
public:
    ClassifierNN(Database& database);
    float classify(std::vector<Object> training, std::vector<Object> test, int k);

private:
    Database database;

    QString kNNWhichClass(Object verifyingObject, int k, std::vector<Object> trainBase);
    void prepareClassPair(std::vector<std::pair<std::string, int> > &classesNameResult, const Database &database);
    float calculateDistance(std::vector<float> first, std::vector<float> second);
};

#endif // CLASSIFIERNN_H
