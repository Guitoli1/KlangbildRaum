#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

namespace theme
{
const juce::Colour background { 0xff14161b };
const juce::Colour panel      { 0xff1c1f26 };
const juce::Colour inset      { 0xff101216 };
const juce::Colour line       { 0xff343945 };
const juce::Colour text       { 0xffc9ced8 };
const juce::Colour textDim    { 0xff7d8493 };
const juce::Colour bright     { 0xfff2f4f8 };
const juce::Colour doubler    { 0xff4cc3b0 };
const juce::Colour echo       { 0xffe8914a };
const juce::Colour spring     { 0xff9a7be8 };
}

class RaumLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RaumLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool over, bool down) override;
};

class RaumAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit RaumAudioProcessorEditor (RaumAudioProcessor&);
    ~RaumAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<SliderAttachment> attachment;
        void setBounds (juce::Rectangle<int> r) { label.setBounds (r.removeFromTop (16)); slider.setBounds (r); }
        void setActive (bool a) { slider.setAlpha (a ? 1.0f : 0.4f); label.setAlpha (a ? 1.0f : 0.4f); }
    };

    Knob& addKnob (const juce::String& paramID, const juce::String& text, juce::Colour accent);
    void setupPower (juce::ToggleButton& b, const juce::String& paramID, juce::Colour accent);
    void timerCallback() override;

    void drawReels (juce::Graphics&, juce::Rectangle<float>, bool active);
    void drawSpring (juce::Graphics&, juce::Rectangle<float>, bool active);
    void drawDoublerIcon (juce::Graphics&, juce::Rectangle<float>, bool active);
    void drawPanel (juce::Graphics&, juce::Rectangle<int>, juce::Colour accent, bool active);

    RaumAudioProcessor& proc;
    RaumLookAndFeel lnf;

    std::vector<std::unique_ptr<Knob>> knobs;
    Knob *dMix = nullptr, *dWidth = nullptr, *dDepth = nullptr;
    Knob *eTime = nullptr, *eFb = nullptr, *eTone = nullptr, *eWow = nullptr, *eMix = nullptr;
    Knob *sMix = nullptr, *sDecay = nullptr, *sTone = nullptr, *sDwell = nullptr, *outKnob = nullptr;

    juce::ToggleButton dOn { "DOUBLER" }, eOn { "BANDECHO" }, sOn { "FEDERHALL" }, pingPong { "" };
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments;
    juce::ComboBox syncBox, presetBox;
    juce::Label syncLabel, presetLabel, pingPongLabel;
    std::unique_ptr<ComboAttachment> syncAttachment;

    juce::Rectangle<int> dPanel, ePanel, sPanel, oPanel;
    float reelAngle = 0.0f, springPhase = 0.0f, meterL = 0.0f, meterR = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RaumAudioProcessorEditor)
};
