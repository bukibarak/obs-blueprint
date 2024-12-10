#include "obs-blueprint-graph.h"

#include "obs-blueprint-node.h"
#include "obs-blueprint-connector.h"
#include "obs-blueprint-variable.h"
#include "GUI/gui-graph.h"
#include "Nodes/Video/node-color-source.h"
#include "Nodes/Numbers/Float/node-float-to-int.h"
#include "Nodes/Waves/node-sinus-time.h"
#include "Nodes/node-temp-test.h"
#include "Nodes/Numbers/Float/node-float-abs.h"
#include "Nodes/Video/node-image-souce.h"
#include "Structs/obs-frame.h"

extern "C" {
	OBSBlueprintGraph::OBSBlueprintGraph()
	{
		blog(LOG_DEBUG, "\n");
		GDebug("======== Begin graph creation ========");
		mainVideoInput = OBSBlueprintInputPin::CreateAndInitialize(VIDEO_PIN, this, OBSFrame(), "Main Video");

		NodeSinusTime* sinusTimeNode = new NodeSinusTime(2.0f, 800.0f);
		addNode(sinusTimeNode);

		NodeFloatAbs* absNode = new NodeFloatAbs();
		addNode(absNode);

		NodeFloatToInt* floatToIntNode = new NodeFloatToInt();
		addNode(floatToIntNode);

		NodeColorSource* colorSourceNode = new NodeColorSource(1920, 1080,0x80FF0000);
		addNode(colorSourceNode);

		NodeImageSource* imageSourceNode = new NodeImageSource("C:/temp/p.gif");
		addNode(imageSourceNode);

		createConnector(colorSourceNode->getOutputPins()[0], mainVideoInput);

		OBSBlueprintVariable* v1 = OBSBlueprintVariable::CreateVariable<float>(FLOAT_PIN, "My float variable");
		v1->setValue(100.0f);
		addVariable(v1);

		OBSBlueprintVariable* v2 = OBSBlueprintVariable::CreateVariable<bool>(BOOLEAN_PIN, "My bool variable");
		addVariable(v2);

		OBSBlueprintVariable* v3 = OBSBlueprintVariable::CreateVariable<OBSFrame>(VIDEO_PIN, "My video variable");
		addVariable(v3);

		GDebug("=========== Graph created! ===========\n\n");
	}

	OBSBlueprintGraph::~OBSBlueprintGraph()
	{
		blog(LOG_DEBUG, "\n");
		GDebug("============== Begin graph destroy ==============\n");
		delete guiGraph;

		GInfo("Deleting [%zu] connectors...", graphConnectors.size());
		for(auto connector : graphConnectors) {
			delete connector;
		}
		GInfo(" ----> Connectors destroyed!!\n");

		GInfo("Deleting [%zu] nodes...", graphNodes.size());
		for(auto node : graphNodes) {
			delete node;
		}
		GInfo(" ----> Nodes destroyed!!\n");

		GInfo("Deleting [%zu] variables...", graphVariables.size());
		for(auto var : graphVariables) {
			delete var;
		}

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
		graphTime += deltaSeconds;

		mutex().lock();
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

				// Update frame cpu matrix
				frameCpuMatData = mainVideoInput->getValuePtr<OBSFrame>()->toMat();
			}
		}
		else {
			frameCpuMatData = OBSFrame::EmptyFrame;
		}

		onGraphEndTick.execute();


		mutex().unlock();
		// GInfo("Graph main video is %ux%u", mainVideoInput->getValuePtr<video_frame>()->width, mainVideoInput->getValuePtr<video_frame>()->height);
	}

	pixel * OBSBlueprintGraph::getRenderPixels() const
	{
		pixel* pixels = reinterpret_cast<pixel*>(frameCpuMatData.data);

		if(pixels == nullptr && (frameCpuMatData.cols > 0 || frameCpuMatData.rows > 0)) GError("No frame found in graph main video pin! OBS will probably crash...");
		return pixels;
	}

	uint32_t OBSBlueprintGraph::getWidth() const
	{
		return frameCpuMatData.cols;
	}

	uint32_t OBSBlueprintGraph::getHeight() const
	{
		return frameCpuMatData.rows;
	}
}

void OBSBlueprintGraph::addVariable(OBSBlueprintVariable *variable)
{
	mutex().lock();
	variable->setupGraph(this);
	graphVariables.push_back(variable);
	mutex().unlock();
}

void OBSBlueprintGraph::deleteVariable(OBSBlueprintVariable *variable)
{
	mutex().lock();
	graphVariables.remove(variable);
	// TODO DELETE NODES LINKED TO VARIABLES!
	delete variable;
	mutex().unlock();
}

bool OBSBlueprintGraph::isVariableUsed(OBSBlueprintVariable *variable)
{
	return false;
	// TODO CHECK WITH NODES IF USED
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

#if DEBUG
	const char* fromParentName = from->getParentNode() ? from->getParentNode()->getDisplayName() : "NO PARENT NODE";
	const char* toParentName = to->getParentNode() ? to->getParentNode()->getDisplayName() : to->getParentGraph() ? "Main Graph" : "NO PARENT NODE";
#endif

	if(from->getPinType() != to->getPinType()) {
#if DEBUG
		GError("Output pin '%s [%s]' of type %s does not match input pin '%s [%s]' type %s... Abort!",
			from->getDisplayName(), fromParentName, PinName[from->getPinType()],
			to->getDisplayName(), toParentName, PinName[to->getPinType()]
		);
#endif
		return nullptr;
	}

	if(to->getConnector() != nullptr) {
		GWarn("Input pin '%s' is already connected! Deleting previous connector first... (will not be updated by GUI)", to->getDisplayName());
		deleteConnector(to->getConnector());
	}

	mutex().lock();
	OBSBlueprintConnector* newConnector = new OBSBlueprintConnector(from, to);
	graphConnectors.push_back(newConnector);
	mutex().unlock();
#if DEBUG
	GInfo("CONNECT [%s] '%s' =======> '%s' [%s]", fromParentName, from->getDisplayName(), to->getDisplayName(), toParentName);
#endif

	return newConnector;
}

void OBSBlueprintGraph::deleteConnector(OBSBlueprintConnector *connector)
{
	if(connector == nullptr) {
		GError("Cannot delete nullptr connector... Abort!");
		return;
	}

	if(connector->getToPin() == mainVideoInput) mainVideoInput->setValue(OBSFrame());

	mutex().lock();
	graphConnectors.remove(connector);
	delete connector;
	mutex().unlock();
}

void OBSBlueprintGraph::addNode(OBSBlueprintNode *node)
{
	mutex().lock();
	node->setupGraph(this);
	graphNodes.push_back(node);
	mutex().unlock();
}

void OBSBlueprintGraph::deleteNode(OBSBlueprintNode *node)
{
	mutex().lock();
	graphNodes.remove(node);
	for(OBSBlueprintConnector* connector : node->getAllConnectors()) {
		deleteConnector(connector);
	}
	delete node;
	mutex().unlock();
}
