/*
  ==============================================================================
    MainComponent.h
    Created: 2019-03-05 13:22:15
    Author:  Oliver Brodwolf
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#define AVERAGEVALMax 50

struct AverageVal {
	AverageVal(int minVal, int maxVal) : minVal(minVal), maxVal(maxVal) {
		std::fill_n(valueList, AVERAGEVALMax, 0);
	}

	int64 minVal = 0;
	int64 maxVal = 1000000;
	int index = 0;
	int64 sum = 0;
	int64 valueList[AVERAGEVALMax];
	float fCurTicks = 0;


	double setgetVal(int64 val) {
		if (val >= minVal && val <= maxVal) {
			sum -= valueList[index];  //den letzen Wert wieder abziehen
			sum += val;
			valueList[index] = val;
			if (++index == AVERAGEVALMax) {
				index = 0; // zurück setzten und von vorne füllen
			}

			if (fCurTicks < AVERAGEVALMax) {
				fCurTicks++;
			}

		}

		return (double) sum / fCurTicks;

	}
};


String timeString(Time time) {
	return String::formatted("%02d:%02d:%02d %03d", time.getHours(), time.getMinutes(), time.getSeconds(), time.getMilliseconds());
}

class InfoComponent : public Component {
public:
	InfoComponent() : averageVal_msFrame(1, 100000) {
	}

	~InfoComponent() {}


	void paint(Graphics &g) override {
		auto width = getLocalBounds().getWidth();
		auto height = getLocalBounds().getHeight();

		Time timeNow = Time::getCurrentTime();

		g.setFont(Font(14.0f));
		auto d = width;
		auto angle = (counter % 100) / 100.0f * MathConstants<float>::twoPi;
		g.setColour(pieColor);
		Path filledArc;
		filledArc.addPieSegment(0, 0, d, d, 0, angle, 0.0);
		g.fillPath(filledArc);
		g.setColour(Colours::white);
		g.drawText(timeString(timeNow), 0, height - 40, width, 20, Justification::centred, true);

		if (paintStatus == 0) {
			paintStatus = 1; // first Time

		} else {
			int msFrameRound = roundToInt(averageVal_msFrame.setgetVal((timeNow - timePrevious).inMilliseconds()));

			g.drawText("MS:" + String(msFrameRound) + " FPS:" + String(round(1000.0f / msFrameRound)), 0, height - 20, width, 20, Justification::centred, true);
		}

		counter++;
		timePrevious = timeNow;
	}

	int counter = 0;

	Colour pieColor;

private:
	int paintStatus = 0;
	Time timePrevious;
	AverageVal averageVal_msFrame;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoComponent)
};


class MainComponent : public Component, private Timer {
public:
	//==============================================================================
	MainComponent() {
		setSize(640, 200);
		for (int n = 0; n < 6; n++) {
			InfoComponent &info = infos[n];
			info.pieColor = colors[n];
			addAndMakeVisible(info);
			info.setBounds(20 + n * 100, 40, 90, 140);
		}
		MainComponent::startTimer(2);
	}

	~MainComponent() {}

	//==============================================================================
	void paint(Graphics &g) override {
		auto width = getLocalBounds().getWidth();
		auto height = getLocalBounds().getHeight();
		auto curTime = Time::getCurrentTime();

		g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

		g.setFont(Font(14.0f));
		g.setColour(Colours::white);

		g.drawText("MainComponent updated at: " + timeString(curTime), 0, height - 20, width, 20, Justification::centred, true);

	}

	void resized() override {


		for (int n = 0; n < 6; n++) {
				InfoComponent &info = infos[n];
				info.counter=0;
			}

	}


private:
	//==============================================================================

	void timerCallback() override {

		for (int n = 0; n < 6; n++) {
			bool repaintFlag = n == 0 ? true : counter % (n * 10) == 0;
			if (repaintFlag) {
				InfoComponent &info = infos[n];
				info.repaint();
			}
		}

		counter++;
	}

	int counter = 0;
	InfoComponent infos[6];
	Colour colors[6] = {Colour(0xffff3900), Colour(0xffff7800), Colour(0xffffd100), Colour(0xffe1ff00), Colour(0xff89ff00), Colour(0xff00c0ff)};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};