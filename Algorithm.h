#include <juce_audio_processors/juce_audio_processors.h>


class AlgorithmBase {

protected:
    int last_index = -1;

public:

    AlgorithmBase() = default;
    AlgorithmBase(AlgorithmBase *o){
        last_index = o->last_index;
    }
    virtual int getNextValue(const juce::SortedSet<int> &notes)  = 0;

    virtual ~AlgorithmBase() {}

};

class UpAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    int getNextValue(const juce::SortedSet<int> &notes) {
        if (notes.size() > 0) {
            last_index = (++last_index) % notes.size();
        } else {
            last_index = -1;
        }
        return last_index;
    }

};

class DownAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    int getNextValue(const juce::SortedSet<int> &notes) {
        if (notes.size() > 0) {

            if (last_index <= 0) {
                last_index = notes.size();
            }
            last_index = --last_index;
        } else {
            last_index = -1;
        }
        return last_index;
    }

};