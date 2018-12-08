#include "classifiernm.h"
#include "boost/boost/numeric/ublas/matrix.hpp"

#define TEST 0

using namespace std;

ClassifierNM::ClassifierNM(std::vector<Object> vectorOfObjects, Database& database)
{
    this->trainBase = vectorOfObjects;
    this->sizeOfTrainBase = vectorOfObjects.size();
    this->database = database;
}

void ClassifierNM::prepareForNM()
{
    unsigned int classNo = database.getNoClass();
    vector<string> vectorOfClassName = database.getClassNames();
    vector<vector<Object> > vectorOfVectorWithObject;

    //Zgrupowanie bazy treningowe względem klas
    for(int i = 0 ; i < classNo ; i++){
        vector<Object> tempVec;
        for(int j = 0 ; j < trainBase.size() ; j++){
            Object tempObject = trainBase.at(j);
            if(tempObject.getClassName() == vectorOfClassName.at(i)){
                tempVec.push_back(trainBase.at(j));
            }
        }
        vectorOfVectorWithObject.push_back(tempVec);
    }

  #if TEST == 1
    //---------------------------------------TESTY---------------------------------
    cout<<"---------------------------------------TESTY---------------------------------"<<endl;
    cout<<"Wielkosc bazy treningowej: "<<trainBase.size()<<endl;
    cout<<"Ilosc klas: "<<vectorOfVectorWithObject.size()<<endl;;
    for(int i = 0 ; i < vectorOfVectorWithObject.size() ; i++){
        cout<<"Wielkosc wektora "<<i<<": "<<vectorOfVectorWithObject.at(i).size()<<endl;
    }
  #endif

    //Policzenie średnich dla każdej klasy:
    for(int i = 0 ; i < vectorOfVectorWithObject.size() ; i++){
        vector<Object> tempVectorOfObject = vectorOfVectorWithObject.at(i); //Pobranie wektora obiektów danej klasy
        unsigned int ammountOfObjectInClass = tempVectorOfObject.size(); //Ile jest obiektów danej klasy
        string tempClassName = "bez nazwy"; //Jak dana klasa się nazywa
        vector<float> vectorOfMeanForActuallClass;

        //Przechodzimy przez wszystkie obiekty klasy tyle razy ile jest cech:
        for(int noFeatures = 0 ; noFeatures < database.getNoFeatures() ; noFeatures++){
            float sumOfFeatures = 0;
            //Przejście przez wszystkie obiekty dla konkretnej cechy.
            for(int j = 0 ; j < ammountOfObjectInClass ; j++){
                Object tempObject = tempVectorOfObject.at(j);
                //Jednorazowe pobranie nazwy klasy:
                if(noFeatures == 0){
                    tempClassName = tempObject.getClassName();
                }
                sumOfFeatures += tempObject.getFeatures().at(noFeatures);
            }
            //cout<<"Suma: "<<sumOfFeatures<<", ilosc: "<<database.getNoFeatures()<<endl;
            float tempMean = sumOfFeatures / ammountOfObjectInClass;
            vectorOfMeanForActuallClass.push_back(tempMean);

        }
        mapOfClassNameAndMean.insert(pair<string, vector<float> >(tempClassName, vectorOfMeanForActuallClass));
        vector<float> test = mapOfClassNameAndMean[tempClassName];

      #if TEST == 1
        cout<<"Obiektow klasy "<<tempClassName<<" jest: "<<ammountOfObjectInClass<<endl;
        for(int i = 0 ; i < test.size() ; i++){
            cout<<"Srednia dla cechy "<<i+1<<": "<<test.at(i)<<endl;
        }
      #endif

    }
}

QString ClassifierNM::whichClass(Object verifyingObject){
    vector<float> verifyingObjectFeatures = verifyingObject.getFeatures();
    vector<string> vectorOfClassName = database.getClassNames();
    string classNameToReturn = "";
    float differentToReturn = 10000;
    for(int i = 0 ; i < vectorOfClassName.size() ; i++){
        std::vector<float> insideTestingFeatures = mapOfClassNameAndMean.at(vectorOfClassName.at(i));
        string tempClassName = vectorOfClassName.at(i);
        float mean = 0;

        for(int j = 0 ; j < insideTestingFeatures.size() ; j++){
            mean += ((insideTestingFeatures.at(j) - verifyingObjectFeatures.at(j)) * (insideTestingFeatures.at(j) - verifyingObjectFeatures.at(j)));
            //cout<<"insideTestingFeatures.at(j):"<<insideTestingFeatures.at(j)<<endl;
            //cout<<"verifyingObjectFeatures.at(j):"<<verifyingObjectFeatures.at(j)<<endl;
        }
      #if TEST == 1
        cout<<tempClassName<<"-mean: "<<mean<<endl;
      #endif
        if(mean < differentToReturn) {
            differentToReturn = mean;
            classNameToReturn = tempClassName;
        }
    }
  #if TEST == 1
    cout<<"Wyszla klasa: "<<classNameToReturn<<", sprawdzana jest: "<<verifyingObject.getClassName()<<endl;
  #endif

    return QString::fromStdString(classNameToReturn);
}
