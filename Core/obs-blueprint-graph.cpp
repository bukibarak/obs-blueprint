#include "obs-blueprint-graph.h"

#include "obs-blueprint-node.h"
#include "obs-blueprint-connector.h"
#include "obs-blueprint-variable.h"
#include "GUI/gui-handler.h"
#include "Helpers/obs-blueprint-factory.h"
#include "Nodes/Video/node-color-source.h"
#include "Nodes/Numbers/Float/node-float-to-int.h"
#include "Nodes/Waves/node-sinus-time.h"
#include "Nodes/node-temp-test.h"
#include "Nodes/Numbers/Float/node-float-abs.h"
#include "Nodes/Video/node-image-souce.h"
#include "Nodes/Video/node-image-source2.h"
#include "Nodes/Video/node-video-source.h"
#include "Structs/obs-frame.h"

// TODO: Is 'extern "C" {}' needed here ? Not sure...
extern "C" {
	OBSBlueprintGraph::OBSBlueprintGraph()
	{
		blog(LOG_DEBUG, "\n");
		GDebug("======== Begin graph creation ========");
		mainVideoInput = OBSBlueprintInputPin::CreateAndInitialize(VIDEO_PIN, this, OBSFrame::EmptyFrame, "Main Video");

		OBSBlueprintNode* sinusTimeNode = OBSBlueprintFactory::CreateNode<NodeSinusTime>();
		addNode(sinusTimeNode);

		OBSBlueprintNode* absNode = OBSBlueprintFactory::CreateNode<NodeFloatAbs>();
		addNode(absNode);

		OBSBlueprintNode* floatToIntNode = OBSBlueprintFactory::CreateNode<NodeFloatToInt>();
		addNode(floatToIntNode);

		NodeColorSource* colorSourceNode = new NodeColorSource(1920, 1080,0x80FF0000);
		addNode(colorSourceNode);

		NodeImageSource2* imageSourceNode = new NodeImageSource2("C:/temp/testgif.gif");
		addNode(imageSourceNode);

		NodeVideoSource* video = new NodeVideoSource("C:/temp/v.mp4");
		addNode(video);
		//createConnector(video->getOutputPins()[0], mainVideoInput);

		OBSBlueprintNode* test = new NodeTempTest();
		addNode(test);

		createConnector(imageSourceNode->getOutputPins()[0], mainVideoInput);

		OBSBlueprintVariable* v1 = OBSBlueprintFactory::CreateFloatVariable("My float variable");
		v1->setValue(100.0f);
		addVariable(v1);

		OBSBlueprintVariable* v2 = OBSBlueprintFactory::CreateBooleanVariable("My bool variable");
		addVariable(v2);

		OBSBlueprintVariable* v3 = OBSBlueprintFactory::CreateVideoVariable("My video variable");
		addVariable(v3);

		GDebug("=========== Graph created! ===========\n\n");
	}

	OBSBlueprintGraph::~OBSBlueprintGraph()
	{
		blog(LOG_DEBUG, "\n");
		GDebug("============== Begin graph destroy ==============\n");
		delete guiHandler;

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
		if (guiHandler == nullptr) {
			guiHandler = new GUIHandler(this);
		}
		guiHandler->show();
	}

	void OBSBlueprintGraph::tick(float deltaSeconds)
	{
		graphTime += deltaSeconds;

		mutex.lock();
		onGraphBeginTick.execute(deltaSeconds);

		// If the main video input pin is connected, use it to call tick() on parent node
		if(mainVideoInput->isConnected()) {
			// Check if connector have been changed
			bool forceRedraw = mainVideoInput->getConnector() == mainVideoConnector;
			mainVideoConnector = mainVideoInput->getConnector();

			if(mainVideoConnector->getFromPin() == nullptr || mainVideoConnector->getFromPin()->getParentNode() == nullptr) {
				GError("Graph main video pin is connected, but no corresponding pin/node was found! OBS scene will not be updated!");
			} else {
				// Use connector to call tick on parent node
				mainVideoConnector->getFromPin()->getParentNode()->tick(deltaSeconds);

				// Propagate data to main video input pin
				mainVideoConnector->propagateData();

				// Update pixels matrix if frame has been updated
				const OBSFrame& frame = mainVideoInput->getValue<OBSFrame>();
				if (forceRedraw || !inputConnected || frame.updated) {
					if (frame.empty() && !pixels.empty())
						pixels = OBSFrame::EmptyFrame.getMat();
					else if (!frame.empty()) {
						// I need to do it this way because if we use copyTo(pixels) directly, then pixels data ptr might
						// be unchanged which will result in obs-blueprint-source.c, line 100 "pixels == context->pixels"
						// to returns true even if pixels data DOES have changed. Other solution would be to change
						// the render behaviour in obs-blueprint-source.c
						cv::Mat newPixels;
						frame.getMat().copyTo(newPixels);
						pixels = newPixels;
					}
				}
				inputConnected = true;
			}
		}
		else {
			if (!pixels.empty())
				pixels = OBSFrame::EmptyFrame.getMat();
			inputConnected = false;
		}

		onGraphEndTick.execute();


		mutex.unlock();
		// GInfo("Graph main video is %ux%u", mainVideoInput->getValuePtr<video_frame>()->width, mainVideoInput->getValuePtr<video_frame>()->height);
	}

	uint8_t * OBSBlueprintGraph::getRenderPixels() const
	{
		return pixels.data;
	}

	uint32_t OBSBlueprintGraph::getWidth() const
	{
		return pixels.cols;
	}

	uint32_t OBSBlueprintGraph::getHeight() const
	{
		return pixels.rows;
	}
}

void OBSBlueprintGraph::addVariable(OBSBlueprintVariable *variable)
{
	mutex.lock();
	variable->setupGraph(this);
	graphVariables.push_back(variable);
	mutex.unlock();
}

void OBSBlueprintGraph::deleteVariable(OBSBlueprintVariable *variable)
{
	mutex.lock();
	graphVariables.remove(variable);
	// TODO: DELETE NODES LINKED TO VARIABLES!
	delete variable;
	mutex.unlock();
}

bool OBSBlueprintGraph::isVariableUsed(OBSBlueprintVariable *variable)
{
	return false;
	// TODO: CHECK WITH NODES IF USED
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
			from->getDisplayName(), fromParentName, EnumStr::PinType[from->getPinType()],
			to->getDisplayName(), toParentName, EnumStr::PinType[to->getPinType()]
		);
#endif
		return nullptr;
	}

	if(to->getConnector() != nullptr) {
		GWarn("Input pin '%s' is already connected! Deleting previous connector first... (will not be updated by GUI)", to->getDisplayName());
		deleteConnector(to->getConnector());
	}

	mutex.lock();
	OBSBlueprintConnector* newConnector = new OBSBlueprintConnector(from, to);
	graphConnectors.push_back(newConnector);
	mutex.unlock();
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

	mutex.lock();
	if(connector->getToPin() == mainVideoInput) mainVideoInput->setValue(OBSFrame::EmptyFrame);
	graphConnectors.remove(connector);
	delete connector;
	mutex.unlock();
}

void OBSBlueprintGraph::addNode(OBSBlueprintNode *node)
{
	mutex.lock();
	node->setupGraph(this);
	graphNodes.push_back(node);
	mutex.unlock();
}

void OBSBlueprintGraph::deleteNode(OBSBlueprintNode *node)
{
	for(OBSBlueprintConnector* connector : node->getAllConnectors()) {
		deleteConnector(connector);
	}
	mutex.lock();
	graphNodes.remove(node);
	delete node;
	mutex.unlock();
}
