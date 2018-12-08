#include "classifier.h"

Classifier::Classifier(std::vector<Object> vectorOfObjects)
{
    this->trainBase = vectorOfObjects;
    this->sizeOfTrainBase = vectorOfObjects.size();
    //std::cout<<"Rozmiar bazy treningowej w Classifier: "<<this->trainBase.size()<<std::endl;
}

QString Classifier::NNWhichClass(Object verifyingObject){
    std::vector<float> verifyingObjectFeatures = verifyingObject.getFeatures(); //Pobranie features z obiektu sprawdzanego
    std::string className = "beznazwy";
    float minDistance = 100; //Tutaj można by wymyślić coś lepszego

    //Przechodzimy przez wszystkie obiekty bazy treningowej:
    for(int i = 0 ; i<sizeOfTrainBase ; i++){
        Object objectFromTrainBase = trainBase[i]; //pobieramy obiekt o numerze i z bazy treningowej
        std::vector<float> objectFromTrainBaseFeatures = objectFromTrainBase.getFeatures(); //z powyżej stworzonego obiektu pobieramy jego features
        float distance = calculateDistance(verifyingObjectFeatures, objectFromTrainBaseFeatures); //liczmy dystans między features obiektu sprawdzanego a i-tego obiektu treningowej bazy danych
        if(minDistance>distance){
            /*
             * Jezeli otrzymany powżej dystans jest mniejszy od aktualnie najmniejszego to wpisujemy
             * do zmiennej minDistance tę odległość oraz pobieramy nazwę klasy z aktualnie badanego
             * obiektu bazy treningowej. Ta nazwa klasy wpisywana jest do zmiennej className, która
             * ostatecznie zwracana jest jako wynik całej funkcji.
             */

            minDistance = distance;
            className = objectFromTrainBase.getClassName();
        }
    }

    return QString::fromStdString(className);
}

float Classifier::calculateDistance(std::vector<float> first, std::vector<float> second){
    float sum = 0;
    for(int i = 0 ; i<first.size() ; i++){
        float difference = first[i]-second[i];
        sum += (difference*difference);
    }
    return sum;
}

QString Classifier::kNNWhichClass(Object verifyingObject, int k, const Database &database){
    std::vector<std::pair<float, std::string> >pairs; //Tworzymy wektor par float(odległość), string(nazwa). Na jego podstawie znajdziemy k najmniejszych odległości.
    std::vector<float> verifyingObjectFeatures = verifyingObject.getFeatures(); //Pobranie features z obiektu sprawdzanego
    std::string className = "beznazwy";

    //Przechodzimy przez wszystkie obiekty bazy treningowej:
    for(int i = 0 ; i<sizeOfTrainBase ; i++){
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
    //[](std::pair<int, std::string> p1, std::pair<int,std::string> p2) {return p1.first > p2.first; } wyrażenie lambda jak bym chciał sortować malejąco

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

void Classifier::prepareClassPair(std::vector<std::pair<std::string, int> > &classesNameResult, const Database &database){
    std::vector<std::string> classNameFormDB = database.getClassNames();
    for(unsigned int i = 0 ; i < database.getNoClass() ; i++){
        classesNameResult.push_back(std::pair<std::string, int>(classNameFormDB[i], 0));
    }

    /*
    //Test wypełnienia wektora par:
    for(int i = 0 ; i < 2 ; i++){
        std::cout<<classesNameResult[i].first<<" - "<<classesNameResult[i].second<<std::endl;
    }
    */

}

QString Classifier::NMWhichClass(Object verifyingObject, const Database &database){
     calculateMeanForEachClass(database);
     float globalDifference = 10000;
     std::string className;
     //std::vector<std::pair<std::vector<float>, std::string> > meanForEachClass;
     for(int i = 0 ; i < meanForEachClass.size() ; i++){
         std::vector<float> vectorOfMeanForSingleClass = meanForEachClass[i].first;
         std::vector<float> vectorOfFeaturesVerifying = verifyingObject.getFeatures();
         float difference = 0;
         for(int j = 0 ; j <vectorOfMeanForSingleClass.size() ; j++){
             float tempDif = vectorOfFeaturesVerifying[j] - vectorOfMeanForSingleClass[j];
             tempDif *= tempDif;
             difference += tempDif;
         }
         if(difference < globalDifference){
             globalDifference = difference;
             className = meanForEachClass[i].second;
         }
     }

    return QString::fromStdString(className);
}

void Classifier::calculateMeanForEachClass(const Database &database){
    std::vector<std::vector<Object> > vectorOfClass;

    unsigned int classNo = database.getNoClass();
    std::vector<std::string> classesName = database.getClassNames();
    for(int i = 0 ; i < classesName.size() ; i++){
        std::vector<Object> tempObjectVector;
        for(int j = 0 ; j < trainBase.size() ; j++){
            Object tempObject = trainBase[j];
            if(classesName[i] == tempObject.getClassName()){
                tempObjectVector.push_back(tempObject);
            }
        }
        vectorOfClass.push_back(tempObjectVector);
    }

    //For w zależności od ilości klas w bazie danych
    for(int i = 0 ; i < classNo ; i++){
        std::vector<float> meanOfSingleClassFeatures;
        std::vector<Object> singleClassObjectVector= vectorOfClass[i];
        //For zależy od ilości cech (w tym przypadku 64)
        for(int noOfFeatures = 0 ; noOfFeatures < database.getNoFeatures() ; noOfFeatures++){
            int count;
            float sum;
            //For przez wszystkie obiekty w celu policzenia wektora 64 wymiarowego (średnich) dla każdej klasy
            for(int j = 0 ; j < singleClassObjectVector.size() ; j++){
                Object tempObject = singleClassObjectVector.at(j);
                count++;
                sum += tempObject.getFeatures().at(noOfFeatures);
            }
            float meanOfFeatures = sum/count;
            meanOfSingleClassFeatures.push_back(meanOfFeatures);
        }
        Object tempObject = singleClassObjectVector.at(0);
        std::string nameOfClass = tempObject.getClassName();
        meanForEachClass.push_back(std::make_pair(meanOfSingleClassFeatures, nameOfClass));
    }

    /*
    std::cout<<"Ilosc klas:";
    std::cout<<meanForEachClass.size()<<std::endl;

    std::pair<std::vector<float>, std::string> test = meanForEachClass.at(0);
    std::pair<std::vector<float>, std::string> test2 = meanForEachClass.at(1);

    std::cout<<"Nazwa klasy:";
    std::cout<<test.second<<std::endl;
    std::cout<<"Nazwa klasy:";
    std::cout<<test2.second<<std::endl;

    std::cout<<"Ilosc srednich od cech:";
    std::cout<<test.first.size()<<std::endl;
    std::cout<<"Ilosc srednich od cech:";
    std::cout<<test2.first.size()<<std::endl;
    */

}
