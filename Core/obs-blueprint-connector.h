#pragma once

class OBSBlueprintPin;
class OBSBlueprintInputPin;
class OBSBlueprintOutputPin;

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
	OBSBlueprintConnector(OBSBlueprintOutputPin* from, OBSBlueprintInputPin* to);

	/**
	 * OBS Blueprint connector destructor. When a connector is destroyed, remove it on the connected pin too.
	 */
	~OBSBlueprintConnector();


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
	void propagateData();

private:
	OBSBlueprintOutputPin* fromPin = nullptr;
	OBSBlueprintInputPin* toPin = nullptr;
};
