#include "node-sinus-time.h"

NodeSinusTime::NodeSinusTime(const float &defaultSpeed,
	const float &defaultAmplitude) : OBSBlueprintNode(obs_module_text("NodeSinusTime"))
{
	speedPin = createInputPin(FLOAT_PIN, defaultSpeed, "Speed");
	amplitudePin = createInputPin(FLOAT_PIN, defaultAmplitude, "Amplitude");
	resultPin = createOutputPin(FLOAT_PIN, 0.0f, "Wave value");
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
