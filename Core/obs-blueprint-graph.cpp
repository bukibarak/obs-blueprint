#include "obs-blueprint-graph.h"

#include "Nodes/node-color-source.h"
#include "Nodes/FloatHelpers/node-float-to-int.h"
#include "Nodes/node-sinus-time.h"
#include "Nodes/FloatHelpers/node-float-abs.h"
#include "Structs/video-frame.h"

extern "C" {
	OBSBlueprintGraph::OBSBlueprintGraph()
	{
		blog(LOG_DEBUG, "[OBS BLUEPRINT] Graph created!");
		mainVideoInput = OBSBlueprintInputPin::CreateAndInitialize(VIDEO_PIN, this, video_frame());

		NodeColorSource* colorSourceNode = new NodeColorSource(300, 100,0xFFAA3FBB);
		addNode(colorSourceNode);
		createConnector(colorSourceNode->getVideoPin(), mainVideoInput);

		// NodeFloatToInt* floatToIntNode = new NodeFloatToInt();
		// addNode(floatToIntNode);
		// createConnector(floatToIntNode->getIntOutputPin(), colorSourceNode->getWidthPin());
		//
		// NodeFloatAbs* absNode = new NodeFloatAbs();
		// addNode(absNode);
		// createConnector(absNode->getOutputPin(), floatToIntNode->getFloatInputPin());
		//
		// NodeSinusTime* sinusTimeNode = new NodeSinusTime(5.0f, 300.0f);
		// addNode(sinusTimeNode);
		// createConnector(sinusTimeNode->getResultPin(), absNode->getInputPin());

	}

	OBSBlueprintGraph::~OBSBlueprintGraph()
	{
		for(auto connector : graphConnectors) {
			delete connector;
		}
		blog(LOG_DEBUG, "[OBS BLUEPRINT] Graph connectors deleted!");

		for(auto node : graphNodes) {
			delete node;
		}
		blog(LOG_DEBUG, "[OBS BLUEPRINT] Graph nodes deleted!");

		delete mainVideoInput;
		blog(LOG_DEBUG, "[OBS BLUEPRINT] Graph video input pin deleted!");

		blog(LOG_DEBUG, "[OBS BLUEPRINT] Graph deleted!");
	}

	void OBSBlueprintGraph::tick(float deltaSeconds)
	{
		onGraphBeginTick.execute(deltaSeconds);

		// If the main video input pin is connected, use it to call tick() on parent node
		if(mainVideoInput->isConnected()) {
			if(mainVideoInput->getConnector()->getFromPin() == nullptr || mainVideoInput->getConnector()->getFromPin()->getParentNode() == nullptr) {
				blog(LOG_ERROR, "Graph video input pin is connected, but no corresponding pin and/or node was found!");
			} else {
				// Use connector to call tick on parent node
				mainVideoInput->getConnector()->getFromPin()->getParentNode()->tick(deltaSeconds);

				// Propagate data to main video input pin
				mainVideoInput->getConnector()->propagateData();

				// blog(LOG_DEBUG, "[OBS BLUEPRINT] after tick, main video graph input node is %ux%u", mainVideoInput->getValuePtr<video_frame>()->width, mainVideoInput->getValuePtr<video_frame>()->height);
			}
		}

		onGraphEndTick.execute();

	}

	pixel * OBSBlueprintGraph::getRenderPixels()
	{
		if(!mainVideoInput->isConnected()) {
			blog(LOG_WARNING, "Graph video output pin is not connected!");
			return nullptr;
		}

		pixel* pixels = mainVideoInput->getValuePtr<video_frame>()->pixels;
		if(pixels == nullptr) {
			blog(LOG_WARNING, "Graph video output pin is connected but no video frame found!");
		}
		return pixels;
	}

	uint32_t OBSBlueprintGraph::getWidth()
	{
		return mainVideoInput->getValuePtr<video_frame>()->width;
	}

	uint32_t OBSBlueprintGraph::getHeight()
	{
		return mainVideoInput->getValuePtr<video_frame>()->height;
	}
}

void OBSBlueprintGraph::createConnector(OBSBlueprintOutputPin *from,
	OBSBlueprintInputPin *to)
{
	if(from == nullptr || to == nullptr) {
		blog(LOG_ERROR, "Graph connector pins cannot be null");
		return;
	}

	if(from->getConnector() != nullptr) {
		blog(LOG_WARNING, "Graph connector given output pin is already connected! Deleting previous connector first...");
		deleteConnector(from->getConnector());
	}

	if(to->getConnector() != nullptr) {
		blog(LOG_WARNING, "Graph connector given input pin is already connected! Deleting previous connector first...");
		deleteConnector(to->getConnector());
	}


	OBSBlueprintConnector* newConnector = new OBSBlueprintConnector(from, to);
	graphConnectors.push_back(newConnector);
}

void OBSBlueprintGraph::deleteConnector(OBSBlueprintConnector *connector)
{
	if(connector == nullptr) return;

	graphConnectors.remove(connector);
	delete connector;
}

void OBSBlueprintGraph::addNode(OBSBlueprintNode *node)
{
	node->setupGraphDelegates(onGraphBeginTick, onGraphEndTick);
	graphNodes.push_back(node);
}

void OBSBlueprintGraph::deleteNode(OBSBlueprintNode *node)
{
	graphNodes.remove(node);
	for(OBSBlueprintConnector* connector : node->getAllConnectors()) {
		deleteConnector(connector);
	}
	delete node;
}
