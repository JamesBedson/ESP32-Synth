#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "AudioParameter.h"
#include "../IParameterListener.h"

class AudioParameterTree {

public:

    template<typename T>
    void add(AudioParameter<T>* param) {
        
        param->attachTree(this);
        const std::string& id = param->getID();
        
        // Assign new index
        int index = nextIndex++;
        idToIndex[id] = index;

        if (index >= indexToParam.size())
            indexToParam.resize(index + 1);
        
        indexToParam[index] = param;

        paramToIndex[param] = index;
    }

        void addParameterListener(IParameterListener* l) {
        listener = l;
    }

    // ======================================================
    // DSP --> UI
    // ======================================================
    void notifyParameterChanged(AudioParameterBase* param, float value) {
        auto it = paramToIndex.find(param);
        if (it == paramToIndex.end())
            return;

        int index = it->second; 
        if (listener)
            listener->onParameterChanged(index, value);
    }

    // ======================================================
    // UI --> DSP
    // ======================================================

    bool setParameter(const std::string& id, float value) {
        auto it = idToIndex.find(id);
        if (it == idToIndex.end())
            return false;
        
        int index = it->second;
        AudioParameterBase* param = indexToParam[index];
        param->setValueFromFloat(value);
        return true;
    }

    float getAsFloat(const std::string& id) {
        auto it = idToIndex.find(id);
        if (it == idToIndex.end()) 
            return -1.0f;
        
        int index = it->second;
        return indexToParam[index]->getValueAsFloat();
    }

    // ======================================================
    // Mapping
    // ======================================================

    const std::string& getIDFromIndex(int idx) const {
        static const std::string empty = "";
        if (!isValidIndex(idx))
            return empty;

        return indexToParam[idx]->getID();
    }

    AudioParameterBase* getParameterByIndex(int index) const {
        if (!isValidIndex(index))
            return nullptr;

        return indexToParam[index];
    }

private:

    bool isValidIndex(int index) const {
        int numIndexes = static_cast<int>(indexToParam.size());
        if (index >= 0 && index < numIndexes) return true;
        return false;
    }

    std::unordered_map<std::string, int> idToIndex;
    std::unordered_map<AudioParameterBase*, int> paramToIndex;
    std::vector<AudioParameterBase*> indexToParam;

    IParameterListener* listener = nullptr;
    int nextIndex = 0;
};