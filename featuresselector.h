#ifndef FEATURESSELECTOR_H
#define FEATURESSELECTOR_H

#include <vector>
#include <iostream>
#include <algorithm>
#include "object.h"
#include <iostream>
#include <QString>
#include "database.h"
#include <set>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>

class FeaturesSelector
{
private:
    std::vector<Object> objectsFromDB;
    Database database;
    std::map<std::string, std::vector<float> > mapOfClassNameAndMean;
    std::map<std::string, std::vector<float> > mapOfClassNameAndStandardDeviation;
    std::vector<float> vectorOfFisher;
    //Wartość najlepszej cechy, numer cechy:
    std::vector<std::pair<float, int> >pairsOfFisher;
    double CalcDeterminant(boost::numeric::ublas::matrix<double> m);

    float globalFisher;
    std::string globalFisherLog;
    std::vector<int> bestIndexesFisher;

    void combination(std::vector<int>& valuesToPrint,std::set<int>& valuesInSet, std::set<int>::iterator currentValue, int numbersLeft);

    //Wrzucamy wektor z numerami cech, dostajemy parę - wartość fishera oraz numery cech
    std::pair<float, std::vector<int> > calculateFisherForFeaturesNo(std::vector<int> vectorOfIndex);
    void wypiszMacierz(boost::numeric::ublas::matrix<float> matrix);

public:
    FeaturesSelector(std::vector<Object> objectsFromDB, Database& database);
    void calculateVariance();
    void calculateFisher();
    std::vector<std::pair<float, int> > calculateFisherAndGetQualities(int n);

    std::pair<float, std::vector<int> > findBestFeaturesCombinationByFisher(int dimension);
    std::pair<float, std::vector<int> > SFS(unsigned int featuresToUseNo);
};

#endif // FEATURESSELECTOR_H
