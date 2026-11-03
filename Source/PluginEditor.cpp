#include "PluginEditor.h"

//==============================================================================
RaumLookAndFeel::RaumLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, theme::background);
    setColour (juce::Label::textColourId, theme::textDim);
    setColour (juce::Slider::textBoxTextColourId, theme::text);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::ComboBox::backgroundColourId, theme::inset);
    setColour (juce::ComboBox::outlineColourId, theme::line);
    setColour (juce::ComboBox::textColourId, theme::text);
    setColour (juce::ComboBox::arrowColourId, theme::textDim);
    setColour (juce::PopupMenu::backgroundColourId, theme::panel);
    setColour (juce::PopupMenu::textColourId, theme::text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, theme::line);
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::ToggleButton::textColourId, theme::text);
    setColour (juce::TextEditor::backgroundColourId, theme::inset);
    setColour (juce::TextEditor::textColourId, theme::text);
}

void RaumLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                        float startAngle, float endAngle, juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (4.0f);
    const float r = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto c = bounds.getCentre();
    const float lineW = 4.0f, arcR = r - lineW * 0.5f;
    const auto colour = slider.isEnabled() ? slider.findColour (juce::Slider::rotarySliderFillColourId)
                                           : theme::textDim.withAlpha (0.4f);

    juce::Path bg;
    bg.addCentredArc (c.x, c.y, arcR, arcR, 0.0f, startAngle, endAngle, true);
    g.setColour (theme::line);
    g.strokePath (bg, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    float fromAngle = startAngle;
    if (slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0)
        fromAngle = startAngle + (float) slider.valueToProportionOfLength (0.0) * (endAngle - startAngle);
    const float toAngle = startAngle + pos * (endAngle - startAngle);
    if (std::abs (toAngle - fromAngle) > 0.001f)
    {
        juce::Path val;
        val.addCentredArc (c.x, c.y, arcR, arcR, 0.0f, juce::jmin (fromAngle, toAngle), juce::jmax (fromAngle, toAngle), true);
        g.setColour (colour);
        g.strokePath (val, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    const float knobR = r - lineW - 4.0f;
    const auto knob = juce::Rectangle<float> (knobR * 2, knobR * 2).withCentre (c);
    g.setGradientFill (juce::ColourGradient (theme::panel.brighter (0.35f), c.x, knob.getY(),
                                             theme::panel.brighter (0.1f), c.x, knob.getBottom(), false));
    g.fillEllipse (knob);
    g.setColour (theme::line.brighter (0.2f));
    g.drawEllipse (knob, 1.0f);
    g.setColour (slider.isEnabled() ? theme::bright : theme::textDim);
    g.drawLine ({ c.getPointOnCircumference (knobR * 0.35f, toAngle), c.getPointOnCircumference (knobR - 3.0f, toAngle) }, 2.0f);
}

void RaumLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool over, bool)
{
    const auto r = b.getLocalBounds().toFloat();
    const auto sw = juce::Rectangle<float> (34.0f, 18.0f).withY (r.getCentreY() - 9.0f).withX (r.getX() + 2.0f);
    const bool on = b.getToggleState();
    const auto accent = b.findColour (juce::ToggleButton::tickColourId);

    g.setColour (on ? accent.withAlpha (0.9f) : theme::line.brighter (over ? 0.15f : 0.0f));
    g.fillRoundedRectangle (sw, 9.0f);
    g.setColour (juce::Colours::white);
    g.fillEllipse (juce::Rectangle<float> (14.0f, 14.0f).withCentre ({ on ? sw.getRight() - 9.0f : sw.getX() + 9.0f, sw.getCentreY() }));

    const bool header = b.getProperties()["header"];
    g.setColour (header ? (on ? theme::bright : theme::textDim) : theme::text);
    g.setFont (juce::FontOptions (header ? 14.0f : 13.0f, header ? juce::Font::bold : juce::Font::plain));
    g.drawText (b.getButtonText(), r.withTrimmedLeft (sw.getRight() + 9.0f), juce::Justification::centredLeft);
}

//==============================================================================
RaumAudioProcessorEditor::RaumAudioProcessorEditor (RaumAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lnf);

    setupPower (dOn, "d_on", theme::doubler);
    setupPower (eOn, "e_on", theme::echo);
    setupPower (sOn, "s_on", theme::spring);
    for (auto* b : { &dOn, &eOn, &sOn })
        b->getProperties().set ("header", true);
    setupPower (pingPong, "e_pp", theme::echo);

    dMix   = &addKnob ("d_mix",   "MIX",    theme::doubler);
    dWidth = &addKnob ("d_width", "BREITE", theme::doubler);
    dDepth = &addKnob ("d_depth", "TIEFE",  theme::doubler);

    eTime = &addKnob ("e_time", "ZEIT",     theme::echo);
    eFb   = &addKnob ("e_fb",   "FEEDBACK", theme::echo);
    eMix  = &addKnob ("e_mix",  "MIX",      theme::echo);
    eTone = &addKnob ("e_tone", "TON",      theme::echo);
    eWow  = &addKnob ("e_wow",  "WOW",      theme::echo);

    sMix   = &addKnob ("s_mix",   "MIX",   theme::spring);
    sDecay = &addKnob ("s_decay", juce::CharPointer_UTF8 ("L\xc3\x84NGE"), theme::spring);
    sTone  = &addKnob ("s_tone",  "TON",   theme::spring);
    sDwell = &addKnob ("s_dwell", "DWELL", theme::spring);

    outKnob = &addKnob ("out", "OUTPUT", theme::bright);

    syncBox.addItemList (RaumAudioProcessor::syncNames(), 1);
    addAndMakeVisible (syncBox);
    syncAttachment = std::make_unique<ComboAttachment> (proc.apvts, "e_sync", syncBox);
    pingPongLabel.setText ("PING-PONG", juce::dontSendNotification);
    pingPongLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    addAndMakeVisible (pingPongLabel);
    syncLabel.setText ("SYNC", juce::dontSendNotification);
    syncLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    addAndMakeVisible (syncLabel);

    for (int i = 0; i < proc.getNumPrograms(); ++i)
        presetBox.addItem (proc.getProgramName (i), i + 1);
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this] { proc.setCurrentProgram (presetBox.getSelectedId() - 1); };
    addAndMakeVisible (presetBox);
    presetLabel.setText ("PRESET", juce::dontSendNotification);
    presetLabel.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    presetLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (presetLabel);

    setSize (990, 380);
    startTimerHz (30);
}

RaumAudioProcessorEditor::~RaumAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

RaumAudioProcessorEditor::Knob& RaumAudioProcessorEditor::addKnob (const juce::String& paramID, const juce::String& text, juce::Colour accent)
{
    auto k = std::make_unique<Knob>();
    k->slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 18);
    k->slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    k->slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    k->slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    k->slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    k->slider.setColour (juce::Slider::textBoxTextColourId, theme::text);
    k->label.setText (text, juce::dontSendNotification);
    k->label.setJustificationType (juce::Justification::centred);
    k->label.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    addAndMakeVisible (k->slider);
    addAndMakeVisible (k->label);
    k->attachment = std::make_unique<SliderAttachment> (proc.apvts, paramID, k->slider);
    if (auto* rp = proc.apvts.getParameter (paramID))
        k->slider.setDoubleClickReturnValue (true, rp->convertFrom0to1 (rp->getDefaultValue()));
    knobs.push_back (std::move (k));
    return *knobs.back();
}

void RaumAudioProcessorEditor::setupPower (juce::ToggleButton& b, const juce::String& paramID, juce::Colour accent)
{
    b.setColour (juce::ToggleButton::tickColourId, accent);
    addAndMakeVisible (b);
    buttonAttachments.push_back (std::make_unique<ButtonAttachment> (proc.apvts, paramID, b));
}

void RaumAudioProcessorEditor::timerCallback()
{
    const bool d = dOn.getToggleState(), e = eOn.getToggleState(), s = sOn.getToggleState();
    for (auto* k : { dMix, dWidth, dDepth }) k->setActive (d);
    for (auto* k : { eTime, eFb, eMix, eTone, eWow }) k->setActive (e);
    for (auto* k : { sMix, sDecay, sTone, sDwell }) k->setActive (s);
    syncBox.setAlpha (e ? 1.0f : 0.4f);
    pingPong.setAlpha (e ? 1.0f : 0.4f);
    pingPongLabel.setAlpha (e ? 1.0f : 0.4f);

    const bool free = syncBox.getSelectedId() <= 1;
    eTime->slider.setEnabled (free);

    if (presetBox.getSelectedId() != proc.getCurrentProgram() + 1)
        presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);

    if (e) reelAngle += 0.06f;
    springPhase += 0.35f;
    meterL = juce::jmax (proc.meterL.load(), meterL * 0.85f);
    meterR = juce::jmax (proc.meterR.load(), meterR * 0.85f);
    proc.meterL.store (0.0f);
    proc.meterR.store (0.0f);
    repaint();
}

//==============================================================================
void RaumAudioProcessorEditor::drawPanel (juce::Graphics& g, juce::Rectangle<int> r, juce::Colour accent, bool active)
{
    const auto rf = r.toFloat();
    g.setColour (theme::panel);
    g.fillRoundedRectangle (rf, 10.0f);
    g.setColour (active ? accent.withAlpha (0.55f) : theme::line);
    g.drawRoundedRectangle (rf.reduced (0.5f), 10.0f, 1.0f);
    // farbiger Streifen oben
    g.setColour (active ? accent : theme::line);
    g.fillRoundedRectangle (rf.withHeight (3.0f).reduced (14.0f, 0.0f), 1.5f);
}

void RaumAudioProcessorEditor::drawReels (juce::Graphics& g, juce::Rectangle<float> area, bool active)
{
    const auto col = active ? theme::echo : theme::textDim.withAlpha (0.5f);
    const float rad = area.getHeight() * 0.42f;
    const juce::Point<float> c1 { area.getCentreX() - rad * 1.25f, area.getCentreY() };
    const juce::Point<float> c2 { area.getCentreX() + rad * 1.25f, area.getCentreY() };
    for (auto [c, rr] : { std::pair { c1, rad }, std::pair { c2, rad * 0.8f } })
    {
        g.setColour (col.withAlpha (0.25f));
        g.fillEllipse (juce::Rectangle<float> (rr * 2, rr * 2).withCentre (c));
        g.setColour (col);
        g.drawEllipse (juce::Rectangle<float> (rr * 2, rr * 2).withCentre (c), 1.5f);
        for (int k = 0; k < 3; ++k)
        {
            const float a = reelAngle + (float) k * juce::MathConstants<float>::twoPi / 3.0f;
            g.drawLine ({ c.getPointOnCircumference (rr * 0.25f, a), c.getPointOnCircumference (rr * 0.85f, a) }, 1.5f);
        }
    }
    g.setColour (col.withAlpha (0.7f));
    g.drawLine (c1.x, c1.y + rad, c2.x, c2.y + rad * 0.8f, 1.2f);
}

void RaumAudioProcessorEditor::drawSpring (juce::Graphics& g, juce::Rectangle<float> area, bool active)
{
    const auto col = active ? theme::spring : theme::textDim.withAlpha (0.5f);
    const float wobble = active ? juce::jlimit (0.0f, 1.0f, (meterL + meterR) * 1.5f) : 0.0f;
    juce::Path p;
    const int coils = 9;
    for (int i = 0; i <= coils * 8; ++i)
    {
        const float t = (float) i / (float) (coils * 8);
        const float x = area.getX() + t * area.getWidth();
        const float amp = area.getHeight() * 0.4f * (1.0f + 0.25f * wobble * std::sin (springPhase + t * 6.0f));
        const float y = area.getCentreY() + amp * std::sin (t * (float) coils * juce::MathConstants<float>::twoPi);
        if (i == 0) p.startNewSubPath (x, y); else p.lineTo (x, y);
    }
    g.setColour (col);
    g.strokePath (p, juce::PathStrokeType (1.5f));
}

void RaumAudioProcessorEditor::drawDoublerIcon (juce::Graphics& g, juce::Rectangle<float> area, bool active)
{
    const auto col = active ? theme::doubler : theme::textDim.withAlpha (0.5f);
    const float r = area.getHeight() * 0.38f;
    const auto c = area.getCentre();
    g.setColour (col.withAlpha (0.3f));
    g.fillEllipse (juce::Rectangle<float> (r * 2, r * 2).withCentre (c.translated (-r * 0.55f, 0)));
    g.setColour (col);
    g.drawEllipse (juce::Rectangle<float> (r * 2, r * 2).withCentre (c.translated (-r * 0.55f, 0)), 1.5f);
    g.drawEllipse (juce::Rectangle<float> (r * 2, r * 2).withCentre (c.translated (r * 0.55f, 0)), 1.5f);
}

void RaumAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (theme::background);

    auto header = getLocalBounds().removeFromTop (56).reduced (18, 0);
    g.setColour (theme::bright);
    g.setFont (juce::FontOptions (19.0f, juce::Font::bold));
    g.drawText ("KLANGBILD", header, juce::Justification::centredLeft);
    const int w = juce::GlyphArrangement::getStringWidthInt (juce::FontOptions (19.0f, juce::Font::bold), "KLANGBILD");
    g.setColour (theme::echo);
    g.setFont (juce::FontOptions (19.0f));
    g.drawText ("RAUM", header.withTrimmedLeft (w + 8), juce::Justification::centredLeft);
    g.setColour (theme::textDim);
    g.setFont (juce::FontOptions (12.0f));
    const int w2 = juce::GlyphArrangement::getStringWidthInt (juce::FontOptions (19.0f), "RAUM");
    g.drawText (juce::CharPointer_UTF8 ("Doubler \xc2\xb7 Bandecho \xc2\xb7 Federhall"),
                header.withTrimmedLeft (w + w2 + 24), juce::Justification::centredLeft);

    const bool d = dOn.getToggleState(), e = eOn.getToggleState(), s = sOn.getToggleState();
    drawPanel (g, dPanel, theme::doubler, d);
    drawPanel (g, ePanel, theme::echo, e);
    drawPanel (g, sPanel, theme::spring, s);
    drawPanel (g, oPanel, theme::bright, true);

    auto iconArea = [] (juce::Rectangle<int> panel, float width)
    {
        return panel.toFloat().removeFromTop (46.0f).removeFromRight (width + 16.0f).withTrimmedRight (16.0f).reduced (0.0f, 14.0f);
    };
    drawDoublerIcon (g, iconArea (dPanel, 36.0f), d);
    drawReels (g, iconArea (ePanel, 46.0f), e);
    drawSpring (g, iconArea (sPanel, 70.0f), s);

    // Pegelanzeige im Output-Feld
    auto meter = oPanel.reduced (14).withTrimmedTop (150).toFloat();
    meter = meter.withSizeKeepingCentre (26.0f, meter.getHeight());
    auto drawBar = [&] (juce::Rectangle<float> bar, float level)
    {
        g.setColour (theme::inset);
        g.fillRoundedRectangle (bar, 3.0f);
        const float db = juce::Decibels::gainToDecibels (level, -60.0f);
        const float h = bar.getHeight() * juce::jlimit (0.0f, 1.0f, (db + 60.0f) / 60.0f);
        g.setColour (db > -1.0f ? juce::Colour (0xffe05d5d) : theme::doubler);
        g.fillRoundedRectangle (bar.withTop (bar.getBottom() - h), 3.0f);
    };
    drawBar (meter.removeFromLeft (11.0f), meterL);
    meter.removeFromLeft (4.0f);
    drawBar (meter.removeFromLeft (11.0f), meterR);
}

void RaumAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto header = area.removeFromTop (56).reduced (18, 13);
    presetBox.setBounds (header.removeFromRight (200));
    presetLabel.setBounds (header.removeFromRight (80).withTrimmedRight (8));

    area = area.reduced (14, 0).withTrimmedBottom (14);
    dPanel = area.removeFromLeft (230); area.removeFromLeft (12);
    ePanel = area.removeFromLeft (330); area.removeFromLeft (12);
    sPanel = area.removeFromLeft (260); area.removeFromLeft (12);
    oPanel = area;

    constexpr int rowH = 128, cellW = 100;
    auto layoutPanel = [&] (juce::Rectangle<int> panel, juce::ToggleButton& power)
    {
        auto inner = panel.reduced (14, 8);
        power.setBounds (inner.removeFromTop (38).withTrimmedTop (6).withWidth (160));
        return inner;
    };

    {
        auto in = layoutPanel (dPanel, dOn);
        auto row1 = in.removeFromTop (rowH), row2 = in.removeFromTop (rowH);
        dMix->setBounds (row1.removeFromLeft (cellW));
        dWidth->setBounds (row1.removeFromLeft (cellW));
        dDepth->setBounds (row2.removeFromLeft (cellW));
    }
    {
        auto in = layoutPanel (ePanel, eOn);
        auto row1 = in.removeFromTop (rowH), row2 = in.removeFromTop (rowH);
        eTime->setBounds (row1.removeFromLeft (cellW));
        eFb->setBounds (row1.removeFromLeft (cellW));
        eMix->setBounds (row1.removeFromLeft (cellW));
        eTone->setBounds (row2.removeFromLeft (cellW));
        eWow->setBounds (row2.removeFromLeft (cellW));
        auto ctl = row2.reduced (4, 0);
        syncLabel.setBounds (ctl.removeFromTop (16));
        syncBox.setBounds (ctl.removeFromTop (26));
        ctl.removeFromTop (10);
        pingPongLabel.setBounds (ctl.removeFromTop (16));
        pingPong.setBounds (ctl.removeFromTop (26));
    }
    {
        auto in = layoutPanel (sPanel, sOn);
        auto row1 = in.removeFromTop (rowH), row2 = in.removeFromTop (rowH);
        const int cw = in.getWidth() / 2;
        sMix->setBounds (row1.removeFromLeft (cw));
        sDecay->setBounds (row1.removeFromLeft (cw));
        sTone->setBounds (row2.removeFromLeft (cw));
        sDwell->setBounds (row2.removeFromLeft (cw));
    }
    {
        auto in = oPanel.reduced (6, 8).withTrimmedTop (18);
        outKnob->setBounds (in.removeFromTop (rowH));
    }
}
