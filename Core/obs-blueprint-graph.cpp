#include "obs-blueprint-graph.h"

#include "obs-blueprint-node.h"
#include "obs-blueprint-connector.h"
#include "GUI/gui-graph.h"
#include "Nodes/node-color-source.h"
#include "Nodes/FloatHelpers/node-float-to-int.h"
#include "Nodes/node-sinus-time.h"
#include "Nodes/FloatHelpers/node-float-abs.h"
#include "Structs/video-frame.h"

extern "C" {
	OBSBlueprintGraph::OBSBlueprintGraph()
	{
		blog(LOG_DEBUG, "\n");
		GDebug("======== Begin graph creation ========");
		mainVideoInput = OBSBlueprintInputPin::CreateAndInitialize(VIDEO_PIN, this, video_frame(), "Main Video");

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
		// NodeSinusTime* sinusTimeNode = new NodeSinusTime(2.0f, 300.0f);
		// addNode(sinusTimeNode);
		// createConnector(sinusTimeNode->getResultPin(), absNode->getInputPin());
		GDebug("=========== Graph created! ===========\n\n");
	}

	OBSBlueprintGraph::~OBSBlueprintGraph()
	{
		blog(LOG_INFO, "\n");
		GDebug("============== Begin graph destroy ==============\n");
		delete guiGraph;

		GInfo("Deleting [%zu] connectors...", graphConnectors.size());
		for(auto connector : graphConnectors) {
			delete connector;
		}
		GInfo("Connectors destroyed!\n");

		GInfo("Deleting [%zu] nodes...", graphNodes.size());
		for(auto node : graphNodes) {
			delete node;
		}
		GInfo("Nodes destroyed!\n");

		delete mainVideoInput;
		blog(LOG_DEBUG, "\n");
		GDebug("================ Graph destroyed ================\n\n");
	}


	void OBSBlueprintGraph::sourcePropertiesClick()
	{
		if(guiGraph == nullptr) {
			guiGraph = new GUIGraph(this);
		}
		guiGraph->show();
	}

	void OBSBlueprintGraph::tick(float deltaSeconds)
	{
		onGraphBeginTick.execute(deltaSeconds);

		// If the main video input pin is connected, use it to call tick() on parent node
		if(mainVideoInput->isConnected()) {
			if(mainVideoInput->getConnector()->getFromPin() == nullptr || mainVideoInput->getConnector()->getFromPin()->getParentNode() == nullptr) {
				GError("Graph main video pin is connected, but no corresponding pin/node was found! OBS scene will not be updated!");
			} else {
				// Use connector to call tick on parent node
				mainVideoInput->getConnector()->getFromPin()->getParentNode()->tick(deltaSeconds);

				// Propagate data to main video input pin
				mainVideoInput->getConnector()->propagateData();
			}
		}

		onGraphEndTick.execute();
		// GInfo("Graph main video is %ux%u", mainVideoInput->getValuePtr<video_frame>()->width, mainVideoInput->getValuePtr<video_frame>()->height);
	}

	pixel * OBSBlueprintGraph::getRenderPixels() const
	{
		video_frame* frame = mainVideoInput->getValuePtr<video_frame>();
		pixel* pixels = frame->pixels;
		if(pixels == nullptr && (frame->width > 0 || frame->height > 0)) GError("No frame found in graph main video pin! OBS will probably crash...");
		return pixels;
	}

	uint32_t OBSBlueprintGraph::getWidth() const
	{
		return mainVideoInput->getValuePtr<video_frame>()->width;
	}

	uint32_t OBSBlueprintGraph::getHeight() const
	{
		return mainVideoInput->getValuePtr<video_frame>()->height;
	}
}

OBSBlueprintConnector* OBSBlueprintGraph::createConnector(OBSBlueprintOutputPin *from,
                                        OBSBlueprintInputPin *to)
{
	if(from == nullptr) {
		GError("Cannot create connector, from pin is nullptr... Abort!");
		return nullptr;
	}
	if(to == nullptr) {
		GError("Cannot create connector, to pin is nullptr... Abort!");
		return nullptr;
	}

	const char* fromParentName = from->getParentNode() ? from->getParentNode()->getDisplayName() : "NO_PARENT";
	const char* toParentName = to->getParentNode() ? to->getParentNode()->getDisplayName() : to->getParentGraph() ? "Graph" : "NO_PARENT";

	if(from->getPinType() != to->getPinType()) {
		GError("Output pin '%s' (%s) of type %s does not match input pin '%s' (%s) type %s... Abort!",
			from->getDisplayName(), fromParentName, PinName[from->getPinType()],
			to->getDisplayName(), toParentName, PinName[to->getPinType()]
		);
	}

	if(from->getConnector() != nullptr) {
		GWarn("Output pin '%s' is already connected! Deleting previous connector first... (will not be updated by GUI)", from->getDisplayName());
		deleteConnector(from->getConnector());
	}

	if(to->getConnector() != nullptr) {
		GWarn("Input pin '%s' is already connected! Deleting previous connector first... (will not be updated by GUI)", to->getDisplayName());
		deleteConnector(to->getConnector());
	}

	OBSBlueprintConnector* newConnector = new OBSBlueprintConnector(from, to);
	graphConnectors.push_back(newConnector);
	GInfo("Added connector from pin '%s' (%s) to pin '%s' (%s)", from->getDisplayName(), fromParentName, to->getDisplayName(), toParentName);
	return newConnector;
}

void OBSBlueprintGraph::deleteConnector(OBSBlueprintConnector *connector)
{
	if(connector == nullptr) {
		GError("Cannot delete nullptr connector... Abort!");
	}

	if(connector->getToPin() == mainVideoInput) mainVideoInput->setValue(video_frame());

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
