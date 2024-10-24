#include "obs-blueprint-connector.h"

#include <stdexcept>

#include "obs-blueprint-pin.h"

OBSBlueprintConnector::OBSBlueprintConnector(OBSBlueprintOutputPin *from,
	OBSBlueprintInputPin *to) : fromPin(from), toPin(to)
{
	if(fromPin->connector != nullptr) throw std::invalid_argument("From pin is already connected");
	if(toPin->connector != nullptr) throw std::invalid_argument("To pin is already connected");
	if(fromPin->pinType != toPin->pinType) throw std::invalid_argument("Pins are not of same type");

	fromPin->connector = this;
	toPin->connector = this;
	GInfo("Created connector '%s' ------> '%s'", from->getDisplayName(), to->getDisplayName());
}

OBSBlueprintConnector::~OBSBlueprintConnector()
{
	GInfo("Deleted connector '%s' ------> '%s'", fromPin->getDisplayName(), toPin->getDisplayName());
	fromPin->connector = nullptr;
	toPin->connector = nullptr;
}

void OBSBlueprintConnector::propagateData()
{
	unsigned int size = fromPin->rawValueSize;
	unsigned int otherSize = toPin->rawValueSize;
	if(size != otherSize) {
		GError("SIZE MISMATCH when trying to propagate data, unexpected behavior may happen! (%u != %u)", size, otherSize);
		size = std::min(size, otherSize);
	}
	memcpy(toPin->rawValuePtr, fromPin->rawValuePtr, size);
}
