#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <vector>
#include <iostream>
#include <algorithm>
#include "object.h"
#include <iostream>
#include <QString>
#include "database.h"

class Classifier
{
private:
    std::vector<Object> trainBase;
    int sizeOfTrainBase;
    std::vector<std::pair<std::vector<float>, std::string> > meanForEachClass; //Wektor par, który paruje wektor floatów i string

public:
    /*
     * Konstruktor klasyfikator przyjmuje bazę treningową.
     */
    Classifier(std::vector<Object> vectorOfObjects);

    /*
     * Metoda zwracająca nazwę klasy po wykonaniu klasyfikacji. Jako argument
     * przyjmuje objekt, którey następnie porównywany jest do wszystkich obiektów
     * w bazie treningowej. Zwracana jest nazwa obiektu, do którego odległość jest
     * najmniejsza.
     */
    QString NNWhichClass(Object verifyingObject);
    QString kNNWhichClass(Object verifyingObject, int k, const Database &database);
    QString NMWhichClass(Object verifyingObject, const Database &database);

private:
    /*
     * Metoda umożliwiająca obliczenie odległości między dwoma punktami w przestrzeni
     * wielowymiarowej. Oba wektory muszą mieć taką samą długość. Podczas liczenia
     * odległości pominięty został pierwiastek ponieważ nie jest on potrzebny do ustalenia
     * która odległość jest najmniejsza. (coś w rodzaju optymalizacji)
     */
    float calculateDistance(std::vector<float> first, std::vector<float> second);

    /*
     * Metoda ta edytuje wektor par <string, int> classesNameResult czyli <nazwa klasy, ilość wystąpień klasy>
     * na podsatwie bazy danych, która zawiera informacje i lości klas oraz ich nazwach.
     */

    /*
     * Używane w kNN. Wektor przyjmowany jako pierwszy parametr metody jest wypełniany parami w postaci
     * <nazwa klasy, ilość wystąpień>. W momencie gdy liczę k-NN muszę podzielić k najmniejszych odległosci
     * między klasy. W ten sposób obliczam, która klasa występuje więcej razy.
     */
    void prepareClassPair(std::vector<std::pair<std::string, int> > &classesNameResult, const Database &database);

    /*
     * Funkcja dzieląca bazę testową na x części. X zależne jest od ilości klas.
     * Następnie liczone są średnie dla wszystkich cech wszystkich obiektów, a to
     * umieszaczne w zmiennej o wektorze meanForEachClass.
     */
    void calculateMeanForEachClass(const Database &database);
};

#endif // CLASSIFIER_H
