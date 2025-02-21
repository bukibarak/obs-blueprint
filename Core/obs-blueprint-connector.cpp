﻿#include "obs-blueprint-connector.h"

#include <stdexcept>

#include "obs-blueprint-node.h"
#include "obs-blueprint-pin.h"
#include "Structs/obs-frame.h"

OBSBlueprintConnector::OBSBlueprintConnector(OBSBlueprintOutputPin *from,
                                             OBSBlueprintInputPin *to) : fromPin(from), toPin(to)
{
	if(toPin->connector != nullptr) throw std::invalid_argument("Input pin is already connected");
	if(fromPin->pinType != toPin->pinType) throw std::invalid_argument("Pins are not of same type");

	if(fromPin->getParentNode() == toPin->getParentNode()) GWarn("From and to pin are linked to the same node, unwanted result may happen!");

	fromPin->connectors.push_back(this);
	toPin->connector = this;
	fromPin->onConnectionChanged.execute();
	toPin->onConnectionChanged.execute();
	// GInfo("CONNECT '%s' ------> '%s'", from->getDisplayName(), to->getDisplayName());
}

OBSBlueprintConnector::~OBSBlueprintConnector()
{
	fromPin->connectors.remove(this);
	toPin->connector = nullptr;
	fromPin->onConnectionChanged.execute();
	toPin->onConnectionChanged.execute();
#if DEBUG
	OBSBlueprintOutputPin* from = getFromPin();
	OBSBlueprintInputPin* to = getToPin();
	const char* fromParentName = from->getParentNode() ? from->getParentNode()->getDisplayName() : "NO PARENT NODE";
	const char* toParentName = to->getParentNode() ? to->getParentNode()->getDisplayName() : to->getParentGraph() ? "Main Graph" : "NO PARENT NODE";
	GInfo("DISCONNECT [%s] '%s' =======> '%s' [%s]", fromParentName, from->getDisplayName(), to->getDisplayName(), toParentName);
#endif

}

void OBSBlueprintConnector::propagateData() const
{
	unsigned int size = fromPin->rawValueSize;
	unsigned int otherSize = toPin->rawValueSize;
	if(size != otherSize) {
		GError("SIZE MISMATCH when trying to propagate data, unexpected behavior may happen! (%u != %u)", size, otherSize);
		size = std::min(size, otherSize);
	}
	if(fromPin->getPinType() == STRING_PIN) {
		toPin->setValue(fromPin->getValue<std::string>()); // TODO: cannot memcpy std::string!!! --> use const char* instead??
	}
	else if (fromPin->getPinType() == VIDEO_PIN) {
		const OBSFrame& f = fromPin->getValue<OBSFrame>();
		toPin->setValue(f);
	}
	else {
		memcpy(toPin->rawValuePtr, fromPin->rawValuePtr, size);
		toPin->onValueChanged.execute();
	}
}
