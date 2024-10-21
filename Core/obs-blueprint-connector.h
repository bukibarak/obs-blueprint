#pragma once
#include <obs-module.h>
#include "obs-blueprint-pin.h"

/**
 * OBS Blueprint connector class. Used to associate a parent output pin with a child input pin.
 * @see OBSBlueprintPin.
 */
class OBSBlueprintConnector {
public:
	/**
	 * Create a new connector between an output pin and an input pin.\n\n
	 * Both pin should \b not already be connected with another connector!\n
	 * Both pin \b MUST be on the same \a PinType.
	 * @param from The parent output pin.
	 * @param to The child input pin.
	 * @see PinType
	 */
	OBSBlueprintConnector(OBSBlueprintOutputPin* from, OBSBlueprintInputPin* to) : fromPin(from), toPin(to)
	{
		if(fromPin->connector != nullptr) // TODO std::runtime_error instead ?
			blog(LOG_ERROR, "Trying to create a connector on output pin that already have a connector assigned (%p)! Unexpected behavior may happen!", fromPin->connector);
		if(toPin->connector != nullptr)
			blog(LOG_ERROR, "Trying to create a connector on input pin that already have a connector assigned (%p)! Unexpected behavior may happen!", toPin->connector);
		if(fromPin->pinType != toPin->pinType)
			blog(LOG_ERROR, "Connecting output pin of type [%s] with input pin of type [%s], which are not same type! Unexpected behavior may happen!", PinName[fromPin->pinType], PinName[toPin->pinType]);

		fromPin->connector = this;
		toPin->connector = this;
	}

	/**
	 * OBS Blueprint connector destructor. When a connector is destroyed, remove it on the connected pin too.
	 */
	~OBSBlueprintConnector()
	{
		fromPin->connector = nullptr;
		toPin->connector = nullptr;
	}

	/**
	 * Get the parent output pin \a ptr.
	 * @return The parent output pin.
	 */
	OBSBlueprintOutputPin* getFromPin() const { return fromPin; }

	/**
	 * Get the child input pin \a ptr.
	 * @return The child input pin.
	 */
	OBSBlueprintInputPin* getToPin() const { return toPin; }

	/**
	 * Propagate the data from the parent output pin to the child input pin.\n\n
	 * \b NOTE: This function use C function \c memcpy() to copy the data between the pins.
	 */
	void propagateData()
	{
		unsigned int size = fromPin->rawValueSize;
		unsigned int otherSize = toPin->rawValueSize;
		if(size != otherSize) {
			blog(LOG_ERROR, "SIZE MISMATCH when trying to propagate data, unexpected behavior may happen! [%u] --> [%u]", size, otherSize);
			size = std::min(size, otherSize);
		}
		memcpy(toPin->rawValuePtr, fromPin->rawValuePtr, size);
	}

private:
	OBSBlueprintOutputPin* fromPin = nullptr;
	OBSBlueprintInputPin* toPin = nullptr;

};
