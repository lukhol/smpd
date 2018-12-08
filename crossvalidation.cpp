#include "crossvalidation.h"
#include <iostream>
#include <algorithm>
using namespace std;

#define TEST 1

CrossValidation::CrossValidation(Database& database, int setNo){
    this->setNo = setNo;
    this->database = database;
    classifierNN = new ClassifierNN(database);
}

//TO DO - narazie na sztywno jest wpisana metoda, należy to zmienić
float CrossValidation::run(ClassificationMethod method, int k){
    vector<Object> vectorOfAllObjects = database.getObjects();
    std::random_shuffle(vectorOfAllObjects.begin(), vectorOfAllObjects.end());

    int objectsNo = vectorOfAllObjects.size();
    if(objectsNo % setNo != 0){
      #if TEST == 1
        cout<<"Ilosc obiektow w zbiorze nie jest podzielna przez podane k!"<<endl;
        cout<<"objectsNo:"<<objectsNo<<", setNo:"<<setNo<<endl;
      #endif
        return 0;
    }

    vector<float> allPercentResult;

    int sizeOfEachSet = objectsNo / setNo;
    int withOneIsChecking = 1;

    for(int i = 0 ; i < setNo ; i++){
        vector<Object> tempTraining;
        vector<Object> tempTest;

        int stopSection = sizeOfEachSet * withOneIsChecking - 1;
        int startSection = stopSection - sizeOfEachSet;

        for(int objNo = 0 ; objNo < objectsNo ; objNo++){
            if(objNo >= startSection && objNo <= stopSection){
                tempTest.push_back(vectorOfAllObjects.at(objNo));
            } else {
                tempTraining.push_back(vectorOfAllObjects.at(objNo));
            }
        }

        float tempResult = classifierNN->classify(tempTraining, tempTest, k);
        allPercentResult.push_back(tempResult);

      #if TEST == 1
        cout<<withOneIsChecking<<". "<<tempResult<<endl;
      #endif

        withOneIsChecking++;
    }

    float meanToReturn = 0;
    for(vector<float>::iterator it = allPercentResult.begin() ; it != allPercentResult.end() ; it++){
        meanToReturn += *it;
    }
    meanToReturn /= allPercentResult.size();

    return meanToReturn;
}

