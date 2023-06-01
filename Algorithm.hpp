#include <juce_audio_processors/juce_audio_processors.h>

#include "HashRandom.hpp"

class AlgorithmBase {

protected:
    int last_index = -1;

public:

    AlgorithmBase() = default;
    AlgorithmBase(AlgorithmBase *o){
        if (o != nullptr)
            last_index = o->last_index;
    }
    virtual int getNextNote(double timeline_slot, const juce::SortedSet<int> &notes, bool notes_changed)  = 0;

    virtual void reset() {
        last_index = -1;
    }

    virtual ~AlgorithmBase() {}

};

class UpAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    int getNextNote(double, const juce::SortedSet<int> &notes, bool) override {
        if (notes.size() > 0) {
            last_index = (++last_index) % notes.size();
            return notes[last_index];
        } else {
            last_index = -1;
            return -1;
        }
    }

};

class DownAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    int getNextNote(double, const juce::SortedSet<int> &notes, bool) override {
        if (notes.size() > 0) {

            if (last_index <= 0) {
                last_index = notes.size();
            }
            last_index = --last_index;
            return notes[last_index];
        } else {
            last_index = -1;
            return -1;
        }
    }

};

class RandomAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    juce::int64 key_;

    juce::SortedSet<int> available_notes;

    int last_note = -1;

    juce::FileLogger *dbgout_ = nullptr;


public:
    void setKey(juce::int64 key) {
        key_ = key;
    }

    void setDebug(juce::FileLogger *logger) {
        dbgout_ = logger;
    }

    void reset() {
        AlgorithmBase::reset();
        last_note = -1;
        available_notes.clearQuick();
    }

    int getNextNote(double timeline_slot, const juce::SortedSet<int> &notes, bool notes_changed) override {

        if (available_notes.isEmpty()) {
            available_notes.addSet(notes);
        } else if (notes_changed) {
            available_notes.clearQuick();
            // Add all the stuff from the new set.
            available_notes.addSet(notes);
        }

        int num_notes = available_notes.size();
        if (dbgout_) {
            std::stringstream x;
            x << "available note count = " << num_notes;
            dbgout_->logMessage(x.str());
        }

        if (num_notes > 0) {
            if (num_notes == 1) {
                last_note = available_notes[0];
                available_notes.clear();
                return last_note;
            } else {
                last_note = getRandom(timeline_slot, last_note, available_notes);
                available_notes.removeValue(last_note);
                return last_note;
            }
        } else {
            return -1;
        }
    }

    virtual ~RandomAlgorithm() = default;

private :
    int getRandom(double slot, int note_to_avoid, const juce::SortedSet<int> & notes) {


        HashRandom rng{"Note", key_, slot};

        for (int j=0; j < 20; ++j) {
            // need to do better, but this is an okay proof of concept
           int maybe =  notes[rng.nextInt(0, notes.size())];
           if (maybe != note_to_avoid)
            return maybe;
        }

        return -1;
    }

};
