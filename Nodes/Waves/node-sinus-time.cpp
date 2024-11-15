#include "node-sinus-time.h"

#include "Core/obs-blueprint-graph.h"

NodeSinusTime::NodeSinusTime(const float &defaultSpeed,
                             const float &defaultAmplitude, const float &defaultShift) : OBSBlueprintNode(obs_module_text("NodeSinusTime"))
{
	speedPin = createInputPin(FLOAT_PIN, defaultSpeed, "Speed");
	amplitudePin = createInputPin(FLOAT_PIN, defaultAmplitude, "Amplitude");
	shiftPin = createInputPin(FLOAT_PIN, defaultShift, "Shift");

	resultPin = createOutputPin(FLOAT_PIN, 0.0f, "Wave value");
}

void NodeSinusTime::execute(float deltaSeconds)
{
	resultPin->setValue(sin(totalTime * speedPin->getValue<float>() + shiftPin->getValue<float>()) * amplitudePin->getValue<float>());
	haveExecutedThisCycle = true;
}

void NodeSinusTime::onGraphLinked()
{
	totalTime = graph->getGraphTotalTime();
}

void NodeSinusTime::onGraphBeginTick(float deltaSeconds)
{
	totalTime += deltaSeconds;
}
