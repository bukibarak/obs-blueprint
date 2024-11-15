#include "obs-blueprint-node.h"

#include "obs-blueprint-connector.h"
#include "obs-blueprint-graph.h"
#include "Helpers/global-logger.h"

OBSBlueprintNode::OBSBlueprintNode(std::string name): displayName(std::move(name))
{
	GInfo("-------- Creating Node [%s] --------", displayName.c_str());
}

OBSBlueprintNode::~OBSBlueprintNode()
{
	GInfo("-------- Destroy Node [%s] --------", getDisplayName());
	for(auto inputPin : inputPins) {
		delete inputPin;
	}
	for(auto outputPin : outputPins) {
		delete outputPin;
	}

	if(graph != nullptr) {
		graph->onGraphBeginTick -= graphBeginTick;
		graph->onGraphEndTick -= graphEndTick;
	}
	else {
		GWarn("Node [%s] was not linked to graph, should have been done by graph on node creation!", getDisplayName());
	}
}

void OBSBlueprintNode::setupGraph(OBSBlueprintGraph* graph)
{
	this->graph = graph;
	graph->onGraphBeginTick += graphBeginTick;
	graph->onGraphEndTick += graphEndTick;
	onGraphLinked();
}

void OBSBlueprintNode::tick(float deltaSeconds)
{
	if(!haveTickedThisCycle) {
		haveTickedThisCycle = true;
		std::list<OBSBlueprintNode*> parentNodes;

		// Check for all input pins if they are connected and should propagate tick to their parent node
		for(OBSBlueprintInputPin* inputPin : inputPins) {
			if(inputPin->isConnected() && inputPin->shouldPropagateTick()) {
				if(inputPin->getConnector()->getFromPin() == nullptr || inputPin->getConnector()->getFromPin()->getParentNode() == nullptr) {
					GError("Input pin '%s' on node [%s] is connected, but no corresponding pin/node was found! Will not propagate tick on this pin...", inputPin->getDisplayName(), getDisplayName());
					continue;
				}
				// Add the parent node to the list
				parentNodes.push_back(inputPin->getConnector()->getFromPin()->getParentNode());
			}
		}

		// Remove parent node duplicates
		parentNodes.unique();

		// Call tick on all parent node
		for(OBSBlueprintNode* parentNode : parentNodes) {
			parentNode->tick(deltaSeconds);
		}

		// Propagate data to current node input pins
		for(OBSBlueprintInputPin* inputPin : inputPins) {
			if(inputPin->isConnected()) {
				inputPin->getConnector()->propagateData();
			}
		}

		// Call execute to perform current node process data
		execute(deltaSeconds);
	}
}

std::vector<OBSBlueprintConnector*> OBSBlueprintNode::getAllConnectors() const
{
	std::vector<OBSBlueprintConnector*> result;
	for(OBSBlueprintInputPin* pin : inputPins) {
		if(pin->isConnected()) {
			result.push_back(pin->getConnector());
		}
	}
	for(OBSBlueprintOutputPin* pin : outputPins) {
		if(pin->isConnected()) {
			result.insert(result.end(), pin->getConnectors().begin(), pin->getConnectors().end());
		}
	}
	return result;
}

void OBSBlueprintNode::addInputPin(OBSBlueprintInputPin *inputPin)
{
	inputPins.push_back(inputPin);
	GInfo("[%s] Input %s '%s' added to node", getDisplayName(), PinName[inputPin->getPinType()], inputPin->getDisplayName());
}

void OBSBlueprintNode::addOutputPin(OBSBlueprintOutputPin *outputPin)
{
	outputPins.push_back(outputPin);
	GInfo("[%s] Output %s '%s' added to node", getDisplayName(), PinName[outputPin->getPinType()], outputPin->getDisplayName());
}
