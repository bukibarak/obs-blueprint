#include "node-sinus-time.h"

NodeSinusTime::NodeSinusTime(const float &defaultSpeed,
	const float &defaultAmplitude)
{
	speedPin = createInputPin(FLOAT_PIN, defaultSpeed);
	amplitudePin = createInputPin(FLOAT_PIN, defaultAmplitude);
	resultPin = createOutputPin(FLOAT_PIN, 0.0f);
}

void NodeSinusTime::execute(float deltaSeconds)
{
	resultPin->setValue(static_cast<float>(sin(totalTime * *speedPin->getValuePtr<float>())) * *amplitudePin->getValuePtr<float>());
	haveExecutedThisCycle = true;
}

void NodeSinusTime::onGraphBeginTick(float deltaSeconds)
{
	totalTime += deltaSeconds;
}
