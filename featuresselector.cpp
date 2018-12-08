#include "featuresselector.h"
#include <math.h>
#include <algorithm>

#define TEST 0
#define FISHER 1
#define MATRIXFISHER 1
#define INDEXESSFS 1
#define CLASSSFS 1
#define FISHERSFS 1

using namespace std;

FeaturesSelector::FeaturesSelector(std::vector<Object> objectsFromDB, Database& database)
{
    this->objectsFromDB = objectsFromDB;
    this->database = database;

    globalFisher = 0;

    unsigned int classNo = database.getNoClass();
    vector<string> vectorOfClassName = database.getClassNames();
    vector<vector<Object> > vectorOfVectorWithObject;

    //Zgrupowanie bazy treningowe względem klas
    for(int i = 0 ; i < classNo ; i++){
        vector<Object> tempVec;
        for(int j = 0 ; j < objectsFromDB.size() ; j++){
            Object tempObject = objectsFromDB.at(j);
            if(tempObject.getClassName() == vectorOfClassName.at(i)){
                tempVec.push_back(objectsFromDB.at(j));
            }
        }
        vectorOfVectorWithObject.push_back(tempVec);
    }

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
            float tempMean = sumOfFeatures / ammountOfObjectInClass;
            vectorOfMeanForActuallClass.push_back(tempMean);
        }
        //Mapa ta zawiera nazwę klasy i wektor średnich cech dla tej klasy
        mapOfClassNameAndMean.insert(pair<string, vector<float> >(tempClassName, vectorOfMeanForActuallClass));
    }
}

void FeaturesSelector::calculateVariance()
{
    unsigned int classNo = database.getNoClass();
    vector<string> vectorOfClassName = database.getClassNames();
    vector<vector<Object> > vectorOfVectorWithObject;

    //Zgrupowanie bazy treningowe względem klas
    for(int i = 0 ; i < classNo ; i++){
        vector<Object> tempVec;
        for(int j = 0 ; j < objectsFromDB.size() ; j++){
            Object tempObject = objectsFromDB.at(j);
            if(tempObject.getClassName() == vectorOfClassName.at(i)){
                tempVec.push_back(objectsFromDB.at(j));
            }
        }
        vectorOfVectorWithObject.push_back(tempVec);
    }

    //Policzenie średnich dla każdej klasy:
    for(int i = 0 ; i < vectorOfVectorWithObject.size() ; i++){
        vector<Object> tempVectorOfObject = vectorOfVectorWithObject.at(i); //Pobranie wektora obiektów danej klasy
        unsigned int ammountOfObjectInClass = tempVectorOfObject.size(); //Ile jest obiektów danej klasy
        string tempClassName = "bez nazwy"; //Jak dana klasa się nazywa
        vector<float> vectorOfStandardDeviation;
        //Pobranie nazwy klasy:
        Object tempObjectGlob = tempVectorOfObject.at(0);
        tempClassName = tempObjectGlob.getClassName();
        //Pobranie z mapy wektora obiektów tej klasy:
        vector<float> vectorOfMeans = mapOfClassNameAndMean.at(tempClassName);

        //Przechodzimy przez wszystkie obiekty klasy tyle razy ile jest cech:
        for(int noFeatures = 0 ; noFeatures < database.getNoFeatures() ; noFeatures++){
            float variance = 0;
            //Przejście przez wszystkie obiekty dla konkretnej cechy.
            for(int j = 0 ; j < ammountOfObjectInClass ; j++){
                Object tempObject = tempVectorOfObject.at(j);
                float tempVar = tempObject.getFeatures().at(noFeatures) - vectorOfMeans.at(noFeatures);
                variance += tempVar*tempVar;
            }
            variance /= ammountOfObjectInClass;
            float standardDeviation = sqrt(variance);
            vectorOfStandardDeviation.push_back(standardDeviation);
            //cout<<"Odchylenie: "<<standardDeviation<<endl;
        }
        //cout<<"To byla klasa: "<<tempClassName<<endl;
        mapOfClassNameAndStandardDeviation.insert(pair<string, vector<float> >(tempClassName, vectorOfStandardDeviation));
    }
}

void FeaturesSelector::calculateFisher()
{
    vector<string> vectorOfClassName = database.getClassNames();
    if(vectorOfClassName.size() != 2) return;

    string firstName = vectorOfClassName.at(0);
    string secondName = vectorOfClassName.at(1);

    vector<float> vectorOfMeanFirstClass = mapOfClassNameAndMean.at(firstName);
    vector<float> vectorOfMeanSecondClass = mapOfClassNameAndMean.at(secondName);

    vector<float> vectorOfStandardDeviationFirstClass = mapOfClassNameAndStandardDeviation.at(firstName);
    vector<float> vectorOfStandardDeviationSecondClass = mapOfClassNameAndStandardDeviation.at(secondName);

    for(int i = 0 ; i < vectorOfMeanFirstClass.size() ; i++){
        float distance = 0;
        distance = vectorOfMeanFirstClass.at(i) - vectorOfMeanSecondClass.at(i);
        distance = abs(distance);

        float sumOfSD = vectorOfStandardDeviationFirstClass.at(i) + vectorOfStandardDeviationSecondClass.at(i);
        float tempFisher = distance / sumOfSD;
        vectorOfFisher.push_back(tempFisher);
        //cout<<tempFisher<<endl;
    }
}

std::vector<std::pair<float, int> > FeaturesSelector::calculateFisherAndGetQualities(int n)
{
    this->calculateVariance();
    this->calculateFisher();

    for(int i = 0 ; i < vectorOfFisher.size() ; i++){
        pair<float, int> element(vectorOfFisher.at(i), i);
        pairsOfFisher.push_back(element);
    }

    sort(pairsOfFisher.begin(), pairsOfFisher.end(), [](std::pair<float, int> p1, std::pair<float, int> p2) {return p1.first > p2.first; });
    //Posortowane cechy:
    for(int i = 0 ; i < n ; i++){
        //cout<<pairsOfFisher.at(i).first<<endl;
    }

    return pairsOfFisher;
}

void FeaturesSelector::combination(vector<int>& valuesToPrint,set<int>& valuesInSet, set<int>::iterator currentValue, int numbersLeft)
{
    if(numbersLeft > 0){
        for(set<int>::iterator it = currentValue; it!=valuesInSet.end(); it++){
            valuesToPrint.push_back(*it);
            set<int>::iterator next = it;
            next++;
            combination(valuesToPrint,valuesInSet,next,numbersLeft-1);
            valuesToPrint.pop_back();
        }
    }
    else {
        vector<int> vectorOfIndex;

        //cout<<"Indeksy: ";
        for(vector<int>::iterator it=valuesToPrint.begin(); it!=valuesToPrint.end();it++){
            vectorOfIndex.push_back(*it);
            //cout<<*it<<" ";
        }
        //cout<<endl;

        //Mapa używana wewnątrz tej funkcji - nazwa klasy i wyznacznik macierzy.
        map<string, float> mapOfClassNameAndDeterminant;


        for(int i = 0 ; i < mapOfClassNameAndMean.size() ; i++){
            string className = database.getClassNames().at(i);
            vector<float> vectorOfMeans = mapOfClassNameAndMean[className];
            vector<Object> objectsOfThisClass;

            //Pobranie wszystkich obiektów danej klasy
            for(int z = 0 ; z < database.getObjects().size() ; z++){
                if(database.getObjects().at(z).getClassName() == className){
                    objectsOfThisClass.push_back(database.getObjects().at(z));
                }
            }
            //cout<<"Klasa: "<<className<<endl;
            boost::numeric::ublas::matrix<float> firstMatrix(vectorOfIndex.size(), objectsOfThisClass.size());

            //Uzupełnienie macierzy wartościami
            for(int row = 0 ; row < vectorOfIndex.size() ; row++){
                for(int column = 0 ; column < objectsOfThisClass.size(); column++){
                    Object tempObject = objectsOfThisClass.at(column);
                    float variable = tempObject.getFeatures().at(vectorOfIndex.at(row));
                    variable -= vectorOfMeans.at(vectorOfIndex.at(row));
                    firstMatrix(row, column) = variable;
                }
            }

            //Wstawienie nazwy stringa i macierzy do mapy
            boost::numeric::ublas::matrix<float> firstMatrixTranspose = boost::numeric::ublas::trans(firstMatrix);
            boost::numeric::ublas::matrix<float> afterMultiplicaton = boost::numeric::ublas::prod(firstMatrix, firstMatrixTranspose);

            //wypiszMacierz(afterMultiplicaton);

            double determinant = CalcDeterminant(afterMultiplicaton);
            mapOfClassNameAndDeterminant.insert(pair<string, float>(className, determinant));
        }

        float fisher = 0;
        vector<string> vectorOfClassName = database.getClassNames();
        float differential = 0;
        float sumOfDeterminant = 0;

        //Obliczenie sumy wyznaczników
        for(int classNo = 0 ; classNo < vectorOfClassName.size() ; classNo++){
            string tempClassName = vectorOfClassName.at(classNo);
            sumOfDeterminant += mapOfClassNameAndDeterminant[tempClassName];
        }

        //Wykonuje się tyle razy ile cech jest badanych:
        for(int featuresNo = 0 ; featuresNo < vectorOfIndex.size() ; featuresNo++){
            float tempDifferential = 0;
            //Wykona się tyle razy ile jest klas
            for(int classNo = 0 ; classNo < vectorOfClassName.size() ; classNo++){
                //cout<<classNo<<endl;
                string tempClassName = vectorOfClassName.at(classNo);
                if(classNo == 0){
                    tempDifferential = mapOfClassNameAndMean[tempClassName].at(featuresNo);
                }else{
                    tempDifferential -= mapOfClassNameAndMean[tempClassName].at(featuresNo);
                }
            }
            tempDifferential *= tempDifferential;
            differential += tempDifferential;
        }
        differential = sqrt(differential);
        //cout<<sumOfDeterminant<<endl;

        if(differential == 0 || sumOfDeterminant == 0){
            cout<<"equals"<<endl;
        }

        if(sumOfDeterminant != 0){
            fisher = differential / (sumOfDeterminant);
        }else{
            cout<<"Nie mozna policzyc wspolczynnika fishera poniewaz suma wyznacznikow macierzy rozrzutu wynosi 0"<<endl;
            fisher = 0;
        }

      #if TEST == 1 && FISHER == 1
        cout<<"Fisher:"<<fisher<<endl;
      #endif

        string log = "Indeksy cech: ";
        for(int i = 0 ; i < vectorOfIndex.size() ; i++){
            log += std::to_string(vectorOfIndex.at(i)) + " ";
        }
        log += "- Fisher: " + std::to_string(fisher);
        //cout<<log<<endl;

        if(fisher > globalFisher){
            globalFisher = fisher;
            globalFisherLog = log;
            bestIndexesFisher = vectorOfIndex;
        } else if(fisher == globalFisher){
            cout<<"ROWNE"<<endl;
        }

        //===========================================================
    }
}

double FeaturesSelector::CalcDeterminant(boost::numeric::ublas::matrix<double> m)
{
      assert(m.size1() == m.size2() && "Can only calculate the determinant of square matrices");
      boost::numeric::ublas::permutation_matrix<std::size_t> pivots(m.size1() );

      const int is_singular = boost::numeric::ublas::lu_factorize(m, pivots);

      if (is_singular) return 0.0;

      double d = 1.0;
      const std::size_t sz = pivots.size();
      for (std::size_t i=0; i != sz; ++i)
      {
        if (pivots(i) != i)
        {
          d *= -1.0;
        }
        d *= m(i,i);
      }
      return d;
}

std::pair<float, std::vector<int> > FeaturesSelector::findBestFeaturesCombinationByFisher(int dimension){
    set<int> valuesSet;
    for(int i = 0 ; i < database.getNoFeatures() ; i++)
    {
         valuesSet.insert(i);
    }
    vector<int> valuesToPrint;
    combination(valuesToPrint,valuesSet,valuesSet.begin(), dimension);

    return std::make_pair(globalFisher, bestIndexesFisher);
}

pair<float, vector<int> > FeaturesSelector::SFS(unsigned int featuresToUseNo){

    //pairsOfFisher - pole prywatne zawierające wartość fishera dla danej cechy oraz numer cechy vector<pair<float, int> >
    calculateFisherAndGetQualities(featuresToUseNo);

    //Stworzenie wektora wybranych cech i dodanie do niego najlepszej cechy początkowej.
    vector<int> chosenFeatures;
    chosenFeatures.push_back(pairsOfFisher.at(0).second);
    float chosenFisher = 0;

    //
    int featuresToCheck = pairsOfFisher.size();

    //For przez ilość badanych cech
    for(int featuresNumber = 1 ; featuresNumber < featuresToUseNo ; featuresNumber++){
        vector<int> bestIndexes; //Indeksy najlepszego Fishera
        float biggestFisher = 0; //Największy Fisher


        for(int featuresNoAll = 0 ; featuresNoAll < featuresToCheck ; featuresNoAll++){
            vector<int> indexes; //Aktualnie sprawdzane indexy

            indexes.push_back(featuresNoAll);
            for(int j = 0 ; j < chosenFeatures.size() ; j++){
                if(chosenFeatures.at(j) == featuresNoAll) {
                    continue;
                }
                indexes.push_back(chosenFeatures.at(j));
            }
            if(indexes.size() > 1){
                pair<float, vector<int> > returned = calculateFisherForFeaturesNo(indexes);

                if(returned.first > biggestFisher){
                    biggestFisher = returned.first;
                    bestIndexes = indexes;
                }

                //Wypisanie fishea i indeksów cech:
                /*
                cout<<returned.first<<" - ";
                for(int t = 0 ; t < returned.second.size() ; t++){
                    cout<<returned.second.at(t)<<" ";
                }
                cout<<endl;
                */
            }
        }
        chosenFeatures = bestIndexes;
        chosenFisher = biggestFisher;
    }
    pair<float, vector<int> > winPair = make_pair(chosenFisher, chosenFeatures);
    return winPair;
}

pair<float, vector<int> > FeaturesSelector::calculateFisherForFeaturesNo(vector<int> vectorOfIndex){

  #if TEST == 1 && INDEXESSFS == 1
    cout<<"Indeksy: ";
    for(vector<int>::iterator it = vectorOfIndex.begin() ; it != vectorOfIndex.end() ; it++){
        cout<<*it<<" ";
    }
    cout<<endl;
  #endif

    //Mapa używana wewnątrz tej funkcji - nazwa klasy i wyznacznik macierzy.
    map<string, float> mapOfClassNameAndDeterminant;

    for(int i = 0 ; i < mapOfClassNameAndMean.size() ; i++){
        string className = database.getClassNames().at(i);
        vector<float> vectorOfMeans = mapOfClassNameAndMean[className];
        vector<Object> objectsOfThisClass;

        //Pobranie wszystkich obiektów danej klasy
        for(int z = 0 ; z < database.getObjects().size() ; z++){
            if(database.getObjects().at(z).getClassName() == className){
                objectsOfThisClass.push_back(database.getObjects().at(z));
            }
        }
     #if TEST == 1 && CLASSSFS == 1
        cout<<"Klasa: "<<className<<endl;
     #endif
        boost::numeric::ublas::matrix<float> firstMatrix(vectorOfIndex.size(), objectsOfThisClass.size());

        //Uzupełnienie macierzy wartościami
        for(int row = 0 ; row < vectorOfIndex.size() ; row++){
            for(int column = 0 ; column < objectsOfThisClass.size(); column++){
                Object tempObject = objectsOfThisClass.at(column);
                float variable = tempObject.getFeatures().at(vectorOfIndex.at(row));
                variable -= vectorOfMeans.at(vectorOfIndex.at(row));
                firstMatrix(row, column) = variable;
            }
        }

        //Wstawienie nazwy stringa i macierzy do mapy
        boost::numeric::ublas::matrix<float> firstMatrixTranspose = boost::numeric::ublas::trans(firstMatrix);
        boost::numeric::ublas::matrix<float> afterMultiplicaton = boost::numeric::ublas::prod(firstMatrix, firstMatrixTranspose);
     #if TEST == 1 && MATRIXFISHER == 1
        wypiszMacierz(afterMultiplicaton);
     #endif

        double determinant = CalcDeterminant(afterMultiplicaton);
        mapOfClassNameAndDeterminant.insert(pair<string, float>(className, determinant));
    }

    float fisher = 0;
    vector<string> vectorOfClassName = database.getClassNames();
    float differential = 0;
    float sumOfDeterminant = 0;

    //Obliczenie sumy wyznaczników
    for(int classNo = 0 ; classNo < vectorOfClassName.size() ; classNo++){
        string tempClassName = vectorOfClassName.at(classNo);
        sumOfDeterminant += mapOfClassNameAndDeterminant[tempClassName];
    }

    //Wykonuje się tyle razy ile cech jest badanych:
    for(int featuresNo = 0 ; featuresNo < vectorOfIndex.size() ; featuresNo++){
        float tempDifferential = 0;
        //Wykona się tyle razy ile jest klas
        for(int classNo = 0 ; classNo < vectorOfClassName.size() ; classNo++){
            //cout<<classNo<<endl;
            string tempClassName = vectorOfClassName.at(classNo);
            if(classNo == 0){
                tempDifferential = mapOfClassNameAndMean[tempClassName].at(featuresNo);
            }else{
                tempDifferential -= mapOfClassNameAndMean[tempClassName].at(featuresNo);
            }
        }
        tempDifferential *= tempDifferential;
        differential += tempDifferential;
    }
    differential = sqrt(differential);
    //cout<<sumOfDeterminant<<endl;
    if(sumOfDeterminant != 0){
        fisher = differential / (sumOfDeterminant);
    }else{
        //cout<<"Nie mozna policzyc wspolczynnika fishera poniewaz suma wyznacznikow macierzy rozrzutu wynosi 0"<<endl;
    }
  #if TEST == 1 && FISHERSFS == 1
    cout<<"Fisher SFS: "<<fisher<<endl;
  #endif
    return make_pair(fisher, vectorOfIndex);

}

void FeaturesSelector::wypiszMacierz(boost::numeric::ublas::matrix<float> matrix){
    cout<<"---------------------"<<endl;
    for(int row = 0 ; row < matrix.size1() ; row++){
        for(int collumn = 0 ; collumn < matrix.size2() ; collumn++){
            cout<<matrix(row, collumn)<<" ";
        }
        cout<<endl;
    }
}
