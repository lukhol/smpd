#include "classifiernn.h"

using namespace std;

ClassifierNN::ClassifierNN(Database& database)
{
    this->database = database;
}

float ClassifierNN::classify(vector<Object> training, vector<Object> test, int k){
    int howManyEquals = 0, testSize = test.size();

    for(int i = 0 ; i<test.size() ; i++){
        Object tempObject = test[i];
        std::string originalClassName = tempObject.getClassName();
        std::string fromNNClassName = kNNWhichClass(test[i], k, training).toStdString();
        if(originalClassName == fromNNClassName) howManyEquals++;
    }

    float percentOfEquals = float(howManyEquals)/(float)testSize;

    return percentOfEquals;
}

QString ClassifierNN::kNNWhichClass(Object verifyingObject, int k, vector<Object> trainBase){
    std::vector<std::pair<float, std::string> >pairs; //Tworzymy wektor par float(odległość), string(nazwa). Na jego podstawie znajdziemy k najmniejszych odległości.
    std::vector<float> verifyingObjectFeatures = verifyingObject.getFeatures(); //Pobranie features z obiektu sprawdzanego
    std::string className = "beznazwy";

    //Przechodzimy przez wszystkie obiekty bazy treningowej:
    for(int i = 0 ; i<trainBase.size() ; i++){
        Object objectFromTrainBase = trainBase[i]; //pobieramy obiekt o numerze i z bazy treningowej
        std::vector<float> objectFromTrainBaseFeatures = objectFromTrainBase.getFeatures(); //z powyżej stworzonego obiektu pobieramy jego features
        float distance = calculateDistance(verifyingObjectFeatures, objectFromTrainBaseFeatures); //liczmy dystans między features obiektu sprawdzanego a i-tego obiektu treningowej bazy danych
        className = objectFromTrainBase.getClassName();

        //Stworzenie pary, a następnie wrzucenie jej do wektora:
        std::pair<float, std::string> element(distance, className);
        pairs.push_back(element);
    }

    //Posortowanie par <odległość, nazwa klas> względem odległości rosnąco.
    sort(pairs.begin(), pairs.end());

    //Tworzymy wektor par <nazwa, ilość> który wypełniony jest na podsatwie informacji z bazy danych:
    std::vector<std::pair<std::string, int> > classesNameResult;
    prepareClassPair(classesNameResult, database);

    //Przypisanie k najmniejszych odległości do
    for(int i = 0 ; i < k ; i++){
        std::string tempClassName = pairs[i].second;
        for(int j = 0 ; j < classesNameResult.size() ; j++){
            if(classesNameResult[j].first == tempClassName){
                classesNameResult[j].second += 1;
                break;
            }
        }
    }

    //Zobaczyć, której klasy jest najwięcej i zwrócić wynik w postaci stringa
    int tempAmmount = 0;
    for(int i = 0 ; i < classesNameResult.size() ; i++){
        if(classesNameResult[i].second > tempAmmount) {
            tempAmmount = classesNameResult[i].second;
            className = classesNameResult[i].first;
        }
    }

    return QString::fromStdString(className);
}

void ClassifierNN::prepareClassPair(std::vector<std::pair<std::string, int> > &classesNameResult, const Database &database){
    std::vector<std::string> classNameFormDB = database.getClassNames();
    for(unsigned int i = 0 ; i < database.getNoClass() ; i++){
        classesNameResult.push_back(std::pair<std::string, int>(classNameFormDB[i], 0));
    }
}

float ClassifierNN::calculateDistance(std::vector<float> first, std::vector<float> second){
    float sum = 0;
    for(int i = 0 ; i<first.size() ; i++){
        float difference = first[i]-second[i];
        sum += (difference*difference);
    }
    return sum;
}
