#include "StarpProcessor.hpp"
#include "StarpEditor.hpp"

#include "Starp.hpp"

struct AlgoListener : public juce::Value::Listener {

    StarpEditor& e_;

    AlgoListener(StarpEditor& e) :e_{e} {

    }
    void valueChanged(juce::Value& v) override {
        e_.processorRef.set_algo_index(v.getValue());
        e_.resized();
    DBGLOG("Algorithm value changed = ", int(v.getValue()));
    }
};

struct SpeedListener : public juce::Value::Listener {

    StarpEditor& e_;

    SpeedListener(StarpEditor& e) :e_{e} {

    }
    void valueChanged(juce::Value& v) override {
        *e_.processorRef.speed = int(v.getValue());
        DBGLOG("Speed GUI value changed = ", int(v.getValue()));
    }
};

struct SpeedParameterListener : public juce::AudioProcessorParameter::Listener {

    StarpEditor& e_;

    SpeedParameterListener(StarpEditor& p) :e_{p} {

    }
    void parameterValueChanged(int, float newValue) override {
        DBGLOG("Speed float value = ", newValue);
        int idx = int(newValue * (SpeedChoices.size()-1));
        // This conditional is to break a possible endless loop.
        // When the GUI changes, it changes the processor's value
        // which will fire this listener.
        if (idx != int(e_.speed_value_.getValue())) {
            e_.speed_value_.setValue(idx);
            DBGLOG("Speed value parameter changed = ", idx);
        }

    }
    void parameterGestureChanged(int, bool) override {
    }
};

//==============================================================================
StarpEditor::StarpEditor (StarpProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p) {


    //==============================================
    addAndMakeVisible(algorithmChoice_);
    
    algo_value_ = AlgorithmIndexes[0];
    algo_value_.addListener(new AlgoListener(*this));

    //==============================================
    addAndMakeVisible(speedChoice_);

    speed_value_.setValue(int(*p.speed));
    speed_value_.addListener(new SpeedListener(*this));
    p.speed->addListener(new SpeedParameterListener(*this));
    
    //==============================================
    addChildComponent(keyLabel_);
    keyLabel_.getTextValue().referTo(key_value_);

    key_value_.setValue(juce::String::toHexString(p.random_key_));

    //==============================================
    addAndMakeVisible(gateSlider_);
    gateSlider_.setRange(10.0, 200.0, 1.0);
    gateSlider_.setTextValueSuffix("%");
    gateSlider_.setDoubleClickReturnValue(true, 100.0, juce::ModifierKeys::ctrlModifier);
    gateSlider_.setValue(*p.gate);
    gateSlider_.onValueChange = [this] { *processorRef.gate = (float)gateSlider_.getValue();};

    //==============================================
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 200);
}

StarpEditor::~StarpEditor() {
    
}

//==============================================================================
void StarpEditor::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void StarpEditor::resized() {
    juce::Grid grid;
 
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using GridItem = juce::GridItem;

    grid.alignItems = juce::Grid::AlignItems::start;
    grid.justifyContent = juce::Grid::JustifyContent::start;
    grid.justifyItems = juce::Grid::JustifyItems::start;
    grid.templateColumns = { Track (Fr (1)) };

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(algorithmChoice_));

    if (algo_value_ == Algorithm::Random) {
        keyLabel_.setVisible(true);
        grid.templateRows.add(Track (Fr (1)));
        grid.items.add(GridItem(keyLabel_));
       
    } else {
        keyLabel_.setVisible(false);
    }

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(speedChoice_));

    grid.templateRows.add(Track (Fr (1)));
    grid.items.add(GridItem(gateSlider_));



    grid.performLayout (getLocalBounds());
    
    }
