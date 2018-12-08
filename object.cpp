#include "object.h"

    std::string Object::getClassName() const
    {
        return className;
    }

    size_t Object::getFeaturesNumber() const
    {
        return features.size();
    }

    std::vector<float> &Object::getFeatures()
    {
        return features;
    }

    int Object::getClassID(){
        return classID;
    }

    void Object::selectIndexes(std::vector<unsigned int> indexes){
        //features
        std::vector<float> newFeatures;
        for(int j = 0 ; j < indexes.size() ; j++){
            for(int i =  0 ; i <features.size() ; i++){
               if(indexes.at(j) == i) newFeatures.push_back(features.at(i));
            }
        }
        features.clear();
        for(int i = 0 ; i < newFeatures.size() ; i++){
            features.push_back(newFeatures.at(i));
        }
    }



