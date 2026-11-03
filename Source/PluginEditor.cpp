#include "PluginEditor.h"
#include "BinaryData.h"

using namespace vintage;

//==============================================================================
VintageLookAndFeel::VintageLookAndFeel()
{
    script        = juce::Typeface::createSystemTypefaceFor (BinaryData::YellowtailRegular_ttf, BinaryData::YellowtailRegular_ttfSize);
    oswaldMedium  = juce::Typeface::createSystemTypefaceFor (BinaryData::OswaldMedium_ttf,     BinaryData::OswaldMedium_ttfSize);
    oswaldRegular = juce::Typeface::createSystemTypefaceFor (BinaryData::OswaldRegular_ttf,    BinaryData::OswaldRegular_ttfSize);
    setDefaultSansSerifTypeface (oswaldRegular);

    setColour (juce::ComboBox::textColourId, cream);
    setColour (juce::PopupMenu::backgroundColourId, cream);
    setColour (juce::PopupMenu::textColourId, ink);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, ink);
    setColour (juce::PopupMenu::highlightedTextColourId, cream);
}

juce::Font VintageLookAndFeel::labelFont (float h, float kerning) const
{
    return juce::Font (juce::FontOptions (oswaldMedium).withHeight (h)).withExtraKerningFactor (kerning);
}
juce::Font VintageLookAndFeel::labelFontRegular (float h) const { return juce::Font (juce::FontOptions (oswaldRegular).withHeight (h)); }
juce::Font VintageLookAndFeel::scriptFont (float h) const       { return juce::Font (juce::FontOptions (script).withHeight (h)); }

juce::Label* VintageLookAndFeel::createSliderTextBox (juce::Slider& s) { return LookAndFeel_V4::createSliderTextBox (s); }

void VintageLookAndFeel::drawJewel (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour c, bool lit)
{
    if (lit)
    {
        g.setGradientFill (juce::ColourGradient (c.withAlpha (0.45f), r.getCentreX(), r.getCentreY(),
                                                 c.withAlpha (0.0f), r.getCentreX() + r.getWidth(), r.getCentreY(), true));
        g.fillEllipse (r.expanded (r.getWidth() * 0.55f));
    }
    // Chromring
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffeeeeee), r.getX(), r.getY(),
                                             juce::Colour (0xff4d4d4d), r.getRight(), r.getBottom(), false));
    g.fillEllipse (r);
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.drawEllipse (r, 0.8f);

    const auto inner = r.reduced (r.getWidth() * 0.17f);
    const auto top = inner.getCentre().translated (-inner.getWidth() * 0.15f, -inner.getHeight() * 0.18f);
    if (lit)
        g.setGradientFill (juce::ColourGradient (c.brighter (0.9f), top.x, top.y, c.darker (0.5f), inner.getRight(), inner.getBottom(), true));
    else
        g.setGradientFill (juce::ColourGradient (c.darker (1.6f), top.x, top.y, c.darker (3.5f), inner.getRight(), inner.getBottom(), true));
    g.fillEllipse (inner);

    // Facetten
    g.setColour (juce::Colours::black.withAlpha (lit ? 0.18f : 0.3f));
    const auto cc = inner.getCentre();
    for (int k = 0; k < 6; ++k)
    {
        const float a = (float) k * juce::MathConstants<float>::pi / 3.0f;
        g.drawLine ({ cc.getPointOnCircumference (inner.getWidth() * 0.18f, a), cc.getPointOnCircumference (inner.getWidth() * 0.48f, a) }, 0.6f);
    }
    g.setColour (juce::Colours::white.withAlpha (lit ? 0.75f : 0.3f));
    g.fillEllipse (juce::Rectangle<float> (inner.getWidth() * 0.28f, inner.getHeight() * 0.2f).withCentre (top));
}

void VintageLookAndFeel::drawScrew (juce::Graphics& g, juce::Point<float> c, float rad, float angle)
{
    const auto r = juce::Rectangle<float> (rad * 2, rad * 2).withCentre (c);
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillEllipse (r.translated (0.6f, 1.0f).expanded (0.6f));
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff2f2f2), r.getX(), r.getY(), juce::Colour (0xff6a6a6a), r.getRight(), r.getBottom(), false));
    g.fillEllipse (r);
    g.setColour (juce::Colour (0xff3a3a3a));
    g.drawLine ({ c.getPointOnCircumference (rad * 0.75f, angle), c.getPointOnCircumference (rad * 0.75f, angle + juce::MathConstants<float>::pi) }, 1.3f);
}

void VintageLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                           float startAngle, float endAngle, juce::Slider& slider)
{
    auto b = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h);
    const auto tagArea = b.removeFromBottom (16.0f);
    const float r = juce::jmin (b.getWidth(), b.getHeight()) * 0.5f - 20.0f;
    const auto c = b.getCentre();
    const float angle = startAngle + pos * (endAngle - startAngle);

    // Skala 0–10, auf die Frontplatte gedruckt
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (float) i / 10.0f * (endAngle - startAngle);
        g.setColour (ink.withAlpha (0.85f));
        g.drawLine ({ c.getPointOnCircumference (r + 3.0f, a), c.getPointOnCircumference (r + (i % 5 == 0 ? 9.0f : 6.5f), a) }, i % 5 == 0 ? 1.4f : 1.0f);
        g.setFont (labelFontRegular (10.0f));
        const auto np = c.getPointOnCircumference (r + 15.5f, a);
        g.drawText (juce::String (i), juce::Rectangle<float> (16.0f, 12.0f).withCentre (np), juce::Justification::centred);
    }

    const auto knob = juce::Rectangle<float> (r * 2, r * 2).withCentre (c);
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillEllipse (knob.translated (2.0f, 3.5f).expanded (1.0f));

    // Rock mit Riffelung
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff4b423b), c.x - r * 0.4f, c.y - r * 0.6f,
                                             juce::Colour (0xff110e0c), c.x + r * 0.7f, c.y + r * 0.9f, true));
    g.fillEllipse (knob);
    for (int k = 0; k < 48; ++k)
    {
        const float a = angle + (float) k * juce::MathConstants<float>::twoPi / 48.0f;
        g.setColour ((k & 1) ? juce::Colours::white.withAlpha (0.07f) : juce::Colours::black.withAlpha (0.35f));
        g.drawLine ({ c.getPointOnCircumference (r * 0.84f, a), c.getPointOnCircumference (r * 0.99f, a) }, 1.0f);
    }

    // Kappe (Bakelit)
    const float capR = r * 0.68f;
    const auto cap = juce::Rectangle<float> (capR * 2, capR * 2).withCentre (c);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff5d534b), c.x - capR * 0.5f, c.y - capR * 0.6f,
                                             juce::Colour (0xff17130f), c.x + capR, c.y + capR, true));
    g.fillEllipse (cap);
    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.drawEllipse (cap, 1.0f);
    g.setColour (juce::Colours::white.withAlpha (0.10f));
    g.fillEllipse (juce::Rectangle<float> (capR * 0.9f, capR * 0.55f).withCentre (c.translated (-capR * 0.25f, -capR * 0.45f)));

    // Zeiger
    g.setColour (juce::Colours::black.withAlpha (0.4f));
    g.drawLine ({ c.getPointOnCircumference (r * 0.22f, angle).translated (0.8f, 1.0f), c.getPointOnCircumference (r * 0.96f, angle).translated (0.8f, 1.0f) }, 2.6f);
    g.setColour (cream);
    g.drawLine ({ c.getPointOnCircumference (r * 0.22f, angle), c.getPointOnCircumference (r * 0.96f, angle) }, 2.4f);

    // Wert anzeigen, solange die Maus darüber ist
    if (slider.isMouseOverOrDragging() && slider.isEnabled())
    {
        const auto text = slider.getTextFromValue (slider.getValue());
        const auto f = labelFontRegular (12.0f);
        const float tw = (float) juce::GlyphArrangement::getStringWidthInt (f, text) + 12.0f;
        const auto tag = juce::Rectangle<float> (tw, 15.0f).withCentre (tagArea.getCentre());
        g.setColour (ink.withAlpha (0.9f));
        g.fillRoundedRectangle (tag, 3.0f);
        g.setColour (cream);
        g.setFont (f);
        g.drawText (text, tag, juce::Justification::centred);
    }
}

void VintageLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool over, bool)
{
    auto r = b.getLocalBounds().toFloat();
    const float h = r.getHeight();
    const bool on = b.getToggleState();
    const auto lamp = juce::Colour ((juce::uint32) (int) b.getProperties().getWithDefault ("lamp", (int) lampRed.getARGB()));

    drawJewel (g, r.removeFromLeft (h).reduced (h * 0.2f), lamp, on);
    r.removeFromLeft (4.0f);

    // Kippschalter
    const auto sw = r.removeFromLeft (h * 0.8f);
    const auto c = sw.getCentre();
    const float nutR = h * 0.2f;
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.fillEllipse (juce::Rectangle<float> (nutR * 2.6f, nutR * 2.6f).withCentre (c.translated (0.8f, 1.2f)));
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff0f0f0), c.x - nutR, c.y - nutR, juce::Colour (0xff595959), c.x + nutR, c.y + nutR, false));
    juce::Path hex;
    for (int k = 0; k < 6; ++k)
    {
        const auto p = c.getPointOnCircumference (nutR * 1.3f, (float) k * juce::MathConstants<float>::pi / 3.0f + 0.52f);
        if (k == 0) hex.startNewSubPath (p); else hex.lineTo (p);
    }
    hex.closeSubPath();
    g.fillPath (hex);
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.strokePath (hex, juce::PathStrokeType (0.7f));

    const float len = h * 0.44f * (on ? -1.0f : 1.0f);
    const juce::Point<float> tip { c.x, c.y + len };
    juce::Path lever;
    lever.startNewSubPath (c.x - 3.0f, c.y);
    lever.lineTo (tip.x - 2.0f, tip.y);
    lever.lineTo (tip.x + 2.0f, tip.y);
    lever.lineTo (c.x + 3.0f, c.y);
    lever.closeSubPath();
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillPath (lever, juce::AffineTransform::translation (1.5f, 1.5f));
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffffffff), c.x - 3.0f, c.y, juce::Colour (0xff7a7a7a), c.x + 3.0f, c.y, false));
    g.fillPath (lever);
    const auto ball = juce::Rectangle<float> (7.0f, 7.0f).withCentre (tip);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffffffff), ball.getX(), ball.getY(), juce::Colour (0xff666666), ball.getRight(), ball.getBottom(), false));
    g.fillEllipse (ball);
    g.setColour (juce::Colours::black.withAlpha (over ? 0.6f : 0.4f));
    g.drawEllipse (ball, 0.7f);

    // AN / AUS
    if (! b.getButtonText().isEmpty())
    {
        g.setColour (ink);
        g.setFont (labelFont (10.0f, 0.1f));
        g.drawText ("AN",  r.withTrimmedLeft (4.0f).removeFromTop (h * 0.5f), juce::Justification::centredLeft);
        g.drawText ("AUS", r.withTrimmedLeft (4.0f).removeFromBottom (h * 0.5f), juce::Justification::centredLeft);
    }
}

void VintageLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox& box)
{
    const auto r = juce::Rectangle<float> (0, 0, (float) w, (float) h);
    const bool plate = box.getProperties()["plate"];

    if (plate)
    {
        g.setGradientFill (juce::ColourGradient (cream.brighter (0.1f), 0, 0, creamDark, 0, (float) h, false));
        g.fillRoundedRectangle (r, 3.0f);
        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.drawRoundedRectangle (r.reduced (0.5f), 3.0f, 1.0f);
        drawScrew (g, { 9.0f, r.getCentreY() }, 4.0f, 0.6f);
        drawScrew (g, { r.getRight() - 9.0f, r.getCentreY() }, 4.0f, 2.1f);
    }
    else
    {
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3a322c), 0, 0, juce::Colour (0xff15110e), 0, (float) h, false));
        g.fillRoundedRectangle (r, 4.0f);
        g.setColour (juce::Colours::black);
        g.drawRoundedRectangle (r.reduced (0.5f), 4.0f, 1.0f);
        g.setColour (juce::Colours::white.withAlpha (0.08f));
        g.drawHorizontalLine (1, 3.0f, (float) w - 3.0f);
    }

    juce::Path arrow;
    const float ax = plate ? r.getRight() - 26.0f : r.getRight() - 14.0f, ay = r.getCentreY();
    arrow.addTriangle (ax - 4.5f, ay - 2.5f, ax + 4.5f, ay - 2.5f, ax, ay + 3.5f);
    g.setColour (plate ? ink : cream.withAlpha (0.8f));
    g.fillPath (arrow);
}

juce::Font VintageLookAndFeel::getComboBoxFont (juce::ComboBox& box)
{
    return box.getProperties()["plate"] ? labelFont (16.0f, 0.06f) : labelFontRegular (14.0f);
}

void VintageLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    const bool plate = box.getProperties()["plate"];
    label.setBounds (plate ? 18 : 6, 1, box.getWidth() - (plate ? 50 : 24), box.getHeight() - 2);
    label.setFont (getComboBoxFont (box));
    label.setColour (juce::Label::textColourId, plate ? ink : cream);
    label.setJustificationType (plate ? juce::Justification::centred : juce::Justification::centredLeft);
}

juce::Font VintageLookAndFeel::getPopupMenuFont() { return labelFontRegular (15.0f); }

//==============================================================================
void VuMeter::update (float avg, float peak, float dt)
{
    const float vu = juce::Decibels::gainToDecibels (avg * 1.1107f, -80.0f) + 18.0f;   // 0 VU = -18 dBFS (Sinus)
    const float target = juce::jlimit (0.0f, 1.12f, avg > 1.0e-6f ? vuToFraction (vu) : 0.0f);

    // Zeiger mit Masse und Feder: ~300 ms Anstiegszeit, leichtes Überschwingen wie ein echtes VU-Meter
    constexpr float omega = 13.0f, zeta = 0.78f;
    const int steps = 4;
    const float hstep = dt / (float) steps;
    for (int i = 0; i < steps; ++i)
    {
        const float acc = omega * omega * (target - position) - 2.0f * zeta * omega * velocity;
        velocity += acc * hstep;
        position += velocity * hstep;
        if (position < 0.0f)  { position = 0.0f;  velocity = 0.0f; }
        if (position > 1.12f) { position = 1.12f; velocity = 0.0f; }
    }

    // PEAK-Lampe in zwei Stufen: bernstein ab -6 dBFS (12 dB über 0 VU), rot ab -1 dBFS; leuchtet 0,8 s nach
    const int level = peak >= 0.891f ? 2 : (peak >= 0.501f ? 1 : 0);
    if (level > 0 && level >= peakLevel)
    {
        peakLevel = level;
        peakHold = 0.8f;
    }
    peakHold -= dt;
    if (peakHold <= 0.0f)
    {
        peakHold = 0.0f;
        peakLevel = 0;
    }
    repaint();
}

void VuMeter::paint (juce::Graphics& g)
{
    const auto b = getLocalBounds().toFloat();

    // Chromrahmen
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.fillRoundedRectangle (b.translated (2.0f, 3.0f), 9.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffeaeaea), 0, b.getY(), juce::Colour (0xff5f5f5f), 0, b.getBottom(), false));
    g.fillRoundedRectangle (b, 9.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff5f5f5f), 0, b.getY(), juce::Colour (0xffdddddd), 0, b.getBottom(), false));
    g.fillRoundedRectangle (b.reduced (3.0f), 7.0f);

    const auto face = b.reduced (8.0f);
    g.setColour (juce::Colours::black);
    g.fillRoundedRectangle (face.expanded (1.5f), 5.0f);

    // hinterleuchtetes Ziffernblatt
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff9edc4), face.getCentreX(), face.getBottom() - face.getHeight() * 0.25f,
                                             juce::Colour (0xffcfa95e), face.getX(), face.getY(), true));
    g.fillRoundedRectangle (face, 4.0f);

    juce::Graphics::ScopedSaveState save (g);
    g.reduceClipRegion (face.toNearestInt());

    const juce::Point<float> pivot { face.getCentreX(), face.getBottom() + face.getHeight() * 0.38f };
    const float radius = face.getHeight() * 0.92f;
    constexpr float maxAngle = 0.70f;
    auto angleFor = [] (float f) { return -maxAngle + 2.0f * maxAngle * f; };

    // Skala
    const float a0 = angleFor (vuToFraction (-20.0f)), aZero = angleFor (vuToFraction (0.0f)), aMax = angleFor (1.0f);
    juce::Path blackArc, redArc;
    blackArc.addCentredArc (pivot.x, pivot.y, radius, radius, 0.0f, a0, aZero, true);
    redArc.addCentredArc (pivot.x, pivot.y, radius + 2.5f, radius + 2.5f, 0.0f, aZero, aMax, true);
    g.setColour (ink);
    g.strokePath (blackArc, juce::PathStrokeType (1.4f));
    g.setColour (juce::Colour (0xffc0281b));
    g.strokePath (redArc, juce::PathStrokeType (5.0f));

    const float marks[] = { -20, -10, -7, -5, -3, -2, -1, 0, 1, 2, 3 };
    for (float v : marks)
    {
        const float a = angleFor (vuToFraction (v));
        const bool red = v > 0.0f;
        g.setColour (red ? juce::Colour (0xffc0281b) : ink);
        g.drawLine ({ pivot.getPointOnCircumference (radius, a), pivot.getPointOnCircumference (radius + 9.0f, a) }, 1.3f);
        g.setFont (lnf.labelFontRegular (11.0f));
        const auto lp = pivot.getPointOnCircumference (radius + 18.0f, a);
        g.drawText (juce::String ((int) std::abs (v)), juce::Rectangle<float> (20.0f, 12.0f).withCentre (lp), juce::Justification::centred);
    }
    // feine Zwischenstriche
    g.setColour (ink.withAlpha (0.6f));
    for (int step = -40; step < 6; ++step)
    {
        if (step < -20 && step % 4 != 0) continue;
        const float a = angleFor (vuToFraction ((float) step * 0.5f));
        g.drawLine ({ pivot.getPointOnCircumference (radius, a), pivot.getPointOnCircumference (radius + 4.0f, a) }, 0.7f);
    }

    g.setColour (ink);
    g.setFont (lnf.labelFont (20.0f, 0.1f));
    g.drawText ("VU", juce::Rectangle<float> (60.0f, 24.0f).withCentre ({ face.getCentreX(), face.getY() + face.getHeight() * 0.68f }), juce::Justification::centred);
    g.setFont (lnf.labelFont (11.0f, 0.1f));
    g.setColour (ink.withAlpha (0.7f));
    g.drawText (name, juce::Rectangle<float> (face.getX() + 8.0f, face.getBottom() - 20.0f, 20.0f, 14.0f), juce::Justification::centredLeft);

    // Peak-Lampe
    const auto led = juce::Rectangle<float> (14.0f, 14.0f).withPosition (face.getRight() - 21.0f, face.getBottom() - 21.0f);
    const auto peakColour = peakLevel == 2 ? juce::Colour (0xffff2a10) : juce::Colour (0xffff8c00);
    if (peakLevel > 0)
    {
        // kräftiger Schein, damit die Lampe auf dem hellen Ziffernblatt gut sichtbar ist
        g.setGradientFill (juce::ColourGradient (peakColour.withAlpha (0.75f), led.getCentreX(), led.getCentreY(),
                                                 peakColour.withAlpha (0.0f), led.getCentreX() + led.getWidth() * 1.4f, led.getCentreY(), true));
        g.fillEllipse (led.expanded (led.getWidth() * 0.9f));
    }
    VintageLookAndFeel::drawJewel (g, led, peakColour, peakLevel > 0);
    g.setFont (lnf.labelFont (9.0f, 0.1f));
    g.setColour (ink.withAlpha (0.7f));
    g.drawText ("PEAK", led.withX (led.getX() - 34.0f).withWidth (30.0f), juce::Justification::centredRight);

    // Zeiger
    const float na = angleFor (position);
    const auto tip = pivot.getPointOnCircumference (radius + 14.0f, na);
    g.setColour (juce::Colours::black.withAlpha (0.18f));
    g.drawLine ({ pivot.translated (3.0f, 2.0f), tip.translated (3.0f, 2.0f) }, 1.6f);
    g.setColour (juce::Colour (0xff1a1410));
    g.drawLine ({ pivot, tip }, 1.5f);

    // Abdeckung über dem Drehpunkt
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3a3029), face.getCentreX(), face.getBottom() - 14.0f,
                                             juce::Colour (0xff120e0b), face.getCentreX(), face.getBottom(), false));
    g.fillEllipse (juce::Rectangle<float> (face.getWidth() * 0.42f, face.getHeight() * 0.34f).withCentre ({ face.getCentreX(), face.getBottom() + 2.0f }));

    // Glas
    g.setGradientFill (juce::ColourGradient (juce::Colours::white.withAlpha (0.28f), face.getX(), face.getY(),
                                             juce::Colours::white.withAlpha (0.0f), face.getX() + face.getWidth() * 0.4f, face.getY() + face.getHeight() * 0.6f, false));
    g.fillRoundedRectangle (face, 4.0f);
}

//==============================================================================
RaumAudioProcessorEditor::RaumAudioProcessorEditor (RaumAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lnf);

    setupSwitch (dOn, "d_on", lampRed);
    setupSwitch (eOn, "e_on", lampRed);
    setupSwitch (sOn, "s_on", lampRed);
    setupSwitch (pingPong, "e_pp", lampAmber);
    for (auto* b : { &dOn, &eOn, &sOn })
        b->setButtonText ("on");   // zeigt "AN/AUS" neben dem Schalter

    dMix   = &addKnob ("d_mix",   "MIX",    0);
    dWidth = &addKnob ("d_width", "BREITE", 0);
    dDepth = &addKnob ("d_depth", "TIEFE",  0);
    eTime  = &addKnob ("e_time",  "ZEIT",   1);
    eFb    = &addKnob ("e_fb",    "FEEDBACK", 1);
    eMix   = &addKnob ("e_mix",   "MIX",    1);
    eTone  = &addKnob ("e_tone",  "TON",    1);
    eWow   = &addKnob ("e_wow",   "WOW",    1);
    sMix   = &addKnob ("s_mix",   "MIX",    2);
    sDecay = &addKnob ("s_decay", juce::String (juce::CharPointer_UTF8 ("L\xc3\x84NGE")), 2);
    sTone  = &addKnob ("s_tone",  "TON",    2);
    sDwell = &addKnob ("s_dwell", "DWELL",  2);
    outKnob = &addKnob ("out",    "PEGEL",  3);

    syncBox.addItemList (RaumAudioProcessor::syncNames(), 1);
    addAndMakeVisible (syncBox);
    syncAttachment = std::make_unique<ComboAttachment> (proc.apvts, "e_sync", syncBox);

    presetBox.getProperties().set ("plate", true);
    for (int i = 0; i < proc.getNumPrograms(); ++i)
        presetBox.addItem (proc.getProgramName (i), i + 1);
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this] { proc.setCurrentProgram (presetBox.getSelectedId() - 1); };
    addAndMakeVisible (presetBox);

    addAndMakeVisible (vuLeft);
    addAndMakeVisible (vuRight);

    sections[0] = { "DOUBLER",   {}, &dOn };
    sections[1] = { "BANDECHO",  {}, &eOn };
    sections[2] = { "FEDERHALL", {}, &sOn };
    sections[3] = { "AUSGANG",   {}, nullptr };

    setSize (1000, 540);
    startTimerHz (60);
}

RaumAudioProcessorEditor::~RaumAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

RaumAudioProcessorEditor::Knob& RaumAudioProcessorEditor::addKnob (const juce::String& paramID, const juce::String& name, int section)
{
    auto k = std::make_unique<Knob>();
    k->name = name;
    k->section = section;
    k->slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k->slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    k->slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f, juce::MathConstants<float>::pi * 2.8f, true);
    k->slider.setMouseDragSensitivity (220);
    addAndMakeVisible (k->slider);
    k->attachment = std::make_unique<SliderAttachment> (proc.apvts, paramID, k->slider);
    if (auto* rp = proc.apvts.getParameter (paramID))
        k->slider.setDoubleClickReturnValue (true, rp->convertFrom0to1 (rp->getDefaultValue()));
    knobs.push_back (std::move (k));
    return *knobs.back();
}

void RaumAudioProcessorEditor::setupSwitch (juce::ToggleButton& b, const juce::String& paramID, juce::Colour lamp)
{
    b.getProperties().set ("lamp", (int) lamp.getARGB());
    addAndMakeVisible (b);
    buttonAttachments.push_back (std::make_unique<ButtonAttachment> (proc.apvts, paramID, b));
}

void RaumAudioProcessorEditor::timerCallback()
{
    constexpr float dt = 1.0f / 60.0f;
    vuLeft.update (proc.vuL.load(), proc.meterL.exchange (0.0f), dt);
    vuRight.update (proc.vuR.load(), proc.meterR.exchange (0.0f), dt);

    const bool on[] = { dOn.getToggleState(), eOn.getToggleState(), sOn.getToggleState(), true };
    for (auto& k : knobs)
        k->slider.setAlpha (on[k->section] ? 1.0f : 0.55f);
    syncBox.setAlpha (on[1] ? 1.0f : 0.55f);
    pingPong.setAlpha (on[1] ? 1.0f : 0.55f);
    eTime->slider.setEnabled (syncBox.getSelectedId() <= 1);

    if (presetBox.getSelectedId() != proc.getCurrentProgram() + 1)
        presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
}

//==============================================================================
void RaumAudioProcessorEditor::renderTextures()
{
    // Tolex-Bezug
    tolexImage = juce::Image (juce::Image::RGB, juce::jmax (1, getWidth()), juce::jmax (1, getHeight()), false);
    {
        juce::Image::BitmapData data (tolexImage, juce::Image::BitmapData::writeOnly);
        juce::Random rnd (7);
        for (int y = 0; y < tolexImage.getHeight(); ++y)
            for (int x = 0; x < tolexImage.getWidth(); ++x)
            {
                const int weave = ((x + y) % 4 == 0 || (x - y + 4096) % 4 == 0) ? -5 : 0;
                const int n = rnd.nextInt (13) - 6 + weave;
                data.setPixelColour (x, y, juce::Colour ((juce::uint8) juce::jlimit (0, 255, 34 + n),
                                                         (juce::uint8) juce::jlimit (0, 255, 28 + n),
                                                         (juce::uint8) juce::jlimit (0, 255, 24 + n)));
            }
    }

    // gebürstete Frontplatte
    if (faceplate.isEmpty())
        return;
    faceplateImage = juce::Image (juce::Image::ARGB, faceplate.getWidth(), faceplate.getHeight(), true);
    juce::Graphics g (faceplateImage);
    const auto r = faceplateImage.getBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (cream.brighter (0.06f), 0, 0, creamDark, 0, r.getHeight(), false));
    g.fillRoundedRectangle (r, 6.0f);
    juce::Random rnd (3);
    for (int y = 0; y < faceplate.getHeight(); ++y)
    {
        const bool light = rnd.nextBool();
        g.setColour ((light ? juce::Colours::white : juce::Colour (0xff6b5534)).withAlpha (0.015f + rnd.nextFloat() * 0.035f));
        g.drawHorizontalLine (y, 0.0f, r.getWidth());
    }
    // leichte Vergilbung an den Rändern
    g.setGradientFill (juce::ColourGradient (juce::Colours::transparentBlack, r.getCentreX(), r.getCentreY(),
                                             juce::Colour (0x30694a1c), 0, 0, true));
    g.fillRoundedRectangle (r, 6.0f);
}

void RaumAudioProcessorEditor::drawEngraved (juce::Graphics& g, const juce::String& text, juce::Rectangle<float> r,
                                             juce::Font f, juce::Justification j)
{
    g.setFont (f);
    g.setColour (juce::Colours::white.withAlpha (0.55f));
    g.drawText (text, r.translated (0.0f, 1.0f), j);
    g.setColour (ink);
    g.drawText (text, r, j);
}

void RaumAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.drawImageAt (tolexImage, 0, 0);

    // Logo
    const auto scriptF = lnf.scriptFont (64.0f);
    const auto logoArea = juce::Rectangle<float> (40.0f, 8.0f, 400.0f, 84.0f);
    g.setFont (scriptF);
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.drawText ("Klangbild", logoArea.translated (2.0f, 3.0f), juce::Justification::centredLeft);
    g.setColour (logo);
    g.drawText ("Klangbild", logoArea, juce::Justification::centredLeft);
    const float sw = (float) juce::GlyphArrangement::getStringWidthInt (scriptF, "Klangbild");
    g.setColour (gold);
    g.setFont (lnf.labelFont (26.0f, 0.35f));
    g.drawText ("RAUM", juce::Rectangle<float> (logoArea.getX() + sw + 14.0f, logoArea.getY() + 30.0f, 120.0f, 40.0f), juce::Justification::centredLeft);
    g.setColour (logo.withAlpha (0.55f));
    g.setFont (lnf.labelFont (12.0f, 0.3f));
    g.drawText (juce::CharPointer_UTF8 ("DOUBLER \xc2\xb7 BANDECHO \xc2\xb7 FEDERHALL"), juce::Rectangle<float> (44.0f, 90.0f, 400.0f, 16.0f),
                juce::Justification::centredLeft);
    g.setColour (logo.withAlpha (0.6f));
    g.setFont (lnf.labelFont (10.0f, 0.3f));
    g.drawText ("PROGRAMM", presetArea.toFloat().withY ((float) presetArea.getY() - 15.0f).withHeight (13.0f), juce::Justification::centredLeft);

    // Keder um die Frontplatte
    const auto fp = faceplate.toFloat();
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.fillRoundedRectangle (fp.expanded (4.0f).translated (0, 2.0f), 9.0f);
    g.setGradientFill (juce::ColourGradient (gold.brighter (0.3f), 0, fp.getY(), gold.darker (0.6f), 0, fp.getBottom(), false));
    g.fillRoundedRectangle (fp.expanded (3.0f), 8.0f);
    g.drawImageAt (faceplateImage, faceplate.getX(), faceplate.getY());
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.drawRoundedRectangle (fp, 6.0f, 1.0f);

    for (auto [x, y, a] : { std::tuple { fp.getX() + 12.0f, fp.getY() + 12.0f, 0.4f }, std::tuple { fp.getRight() - 12.0f, fp.getY() + 12.0f, 1.3f },
                            std::tuple { fp.getX() + 12.0f, fp.getBottom() - 12.0f, 2.2f }, std::tuple { fp.getRight() - 12.0f, fp.getBottom() - 12.0f, 0.9f } })
        VintageLookAndFeel::drawScrew (g, { x, y }, 5.0f, a);

    // Sektionen
    for (size_t i = 0; i < sections.size(); ++i)
    {
        const auto& s = sections[i];
        const auto sb = s.bounds.toFloat();
        drawEngraved (g, s.title, sb.withHeight (36.0f).withTrimmedTop (4.0f), lnf.labelFont (18.0f, 0.22f), juce::Justification::centredLeft);
        g.setColour (ink.withAlpha (0.5f));
        g.drawHorizontalLine ((int) sb.getY() + 40, sb.getX(), sb.getRight());
        g.setColour (juce::Colours::white.withAlpha (0.5f));
        g.drawHorizontalLine ((int) sb.getY() + 41, sb.getX(), sb.getRight());

        if (i + 1 < sections.size())
        {
            const float x = (sb.getRight() + sections[i + 1].bounds.toFloat().getX()) * 0.5f;
            for (float dx : { -2.0f, 2.0f })
            {
                g.setColour (ink.withAlpha (0.35f));
                g.drawVerticalLine ((int) (x + dx), sb.getY(), sb.getBottom());
                g.setColour (juce::Colours::white.withAlpha (0.45f));
                g.drawVerticalLine ((int) (x + dx) + 1, sb.getY(), sb.getBottom());
            }
        }
    }

    for (auto& k : knobs)
    {
        const auto sb = k->slider.getBounds().toFloat();
        drawEngraved (g, k->name, juce::Rectangle<float> (sb.getX(), sb.getBottom() - 2.0f, sb.getWidth(), 18.0f),
                      lnf.labelFont (13.0f, 0.14f), juce::Justification::centred);
    }

    drawEngraved (g, "SYNC", syncLabelArea.toFloat(), lnf.labelFont (12.0f, 0.14f), juce::Justification::centredLeft);
    drawEngraved (g, "PING-PONG", pingPongLabelArea.toFloat(), lnf.labelFont (12.0f, 0.14f), juce::Justification::centredLeft);

    // Betriebslampe und Typenschild
    const auto pa = pilotArea.toFloat();
    VintageLookAndFeel::drawJewel (g, juce::Rectangle<float> (34.0f, 34.0f).withCentre ({ pa.getCentreX(), pa.getY() + 30.0f }), lampRed, true);
    drawEngraved (g, "BETRIEB", juce::Rectangle<float> (pa.getX(), pa.getY() + 52.0f, pa.getWidth(), 16.0f), lnf.labelFont (12.0f, 0.14f), juce::Justification::centred);
    const auto tag = juce::Rectangle<float> (pa.getWidth() - 20.0f, 26.0f).withCentre ({ pa.getCentreX(), pa.getBottom() - 22.0f });
    g.setColour (ink.withAlpha (0.85f));
    g.fillRoundedRectangle (tag, 3.0f);
    g.setColour (gold);
    g.setFont (lnf.labelFont (11.0f, 0.2f));
    g.drawText ("MODELL KR-1", tag, juce::Justification::centred);
}

void RaumAudioProcessorEditor::resized()
{
    const int W = getWidth();

    vuRight.setBounds (W - 36 - 200, 20, 200, 130);
    vuLeft.setBounds (W - 36 - 200 - 16 - 200, 20, 200, 130);

    presetArea = { 44, 128, 270, 32 };
    presetBox.setBounds (presetArea);

    faceplate = { 24, 176, W - 48, 344 };
    auto inner = faceplate.reduced (24, 14);
    const int widths[] = { 205, 318, 212, 140 };
    const int gap = (inner.getWidth() - (widths[0] + widths[1] + widths[2] + widths[3])) / 3;
    for (size_t i = 0; i < sections.size(); ++i)
    {
        sections[i].bounds = inner.removeFromLeft (widths[i]).withHeight (inner.getHeight());
        inner.removeFromLeft (gap);
        if (sections[i].power != nullptr)
            sections[i].power->setBounds (sections[i].bounds.getRight() - 84, sections[i].bounds.getY() + 2, 84, 34);
    }

    constexpr int rowH = 138, sliderH = 116;
    auto cell = [&] (int section, int row, int col, int cols)
    {
        const auto sb = sections[(size_t) section].bounds;
        const int cw = sb.getWidth() / cols;
        return juce::Rectangle<int> (sb.getX() + col * cw, sb.getY() + 46 + row * rowH, cw, sliderH);
    };

    dMix->slider.setBounds   (cell (0, 0, 0, 2));
    dWidth->slider.setBounds (cell (0, 0, 1, 2));
    dDepth->slider.setBounds (cell (0, 1, 0, 2));

    eTime->slider.setBounds (cell (1, 0, 0, 3));
    eFb->slider.setBounds   (cell (1, 0, 1, 3));
    eMix->slider.setBounds  (cell (1, 0, 2, 3));
    eTone->slider.setBounds (cell (1, 1, 0, 3));
    eWow->slider.setBounds  (cell (1, 1, 1, 3));
    {
        auto c = cell (1, 1, 2, 3).reduced (6, 4);
        syncLabelArea = c.removeFromTop (16);
        syncBox.setBounds (c.removeFromTop (26));
        c.removeFromTop (12);
        pingPongLabelArea = c.removeFromTop (16);
        pingPong.setBounds (c.removeFromTop (36).withWidth (70));
    }

    sMix->slider.setBounds   (cell (2, 0, 0, 2));
    sDecay->slider.setBounds (cell (2, 0, 1, 2));
    sTone->slider.setBounds  (cell (2, 1, 0, 2));
    sDwell->slider.setBounds (cell (2, 1, 1, 2));

    outKnob->slider.setBounds (cell (3, 0, 0, 1).withSizeKeepingCentre (110, sliderH));
    pilotArea = cell (3, 1, 0, 1).withHeight (rowH);

    renderTextures();
}
