#pragma once

class OBSBlueprintPin;
class OBSBlueprintInputPin;
class OBSBlueprintOutputPin;

/**
 * \c OBSBlueprintConnector class. Used to associate an \c OBSBlueprintOutputPin \a (from) and an \c OBSBlueprintInputPin \a (to) together.
 * @see OBSBlueprintInputPin
 * @see OBSBlueprintOutputPin
 */
class OBSBlueprintConnector {
public:
	/**
	 * Create a new connector between an output pin \a (from) and an input pin \a (to).\n\n
	 * Both pin \b MUST be on the same \a PinType. \n
	 * Input pin \b MUST not already be connected.
	 * @param from The parent \c OBSBlueprintOutputPin* .
	 * @param to The child \c OBSBlueprintInputPin* .
	 * @see OBSBlueprintInputPin
	 * @see OBSBlueprintOutputPin
	 * @see PinType
	 */
	OBSBlueprintConnector(OBSBlueprintOutputPin* from, OBSBlueprintInputPin* to);

	/**
	 * OBS Blueprint connector destructor. When a connector is destroyed, removes it on the pins too.
	 */
	~OBSBlueprintConnector();


	/**
	 * Get the connector output pin \a ptr.
	 * @return The \c OBSBlueprintOutputPin* \a (from).
	 * @see OBSBlueprintOutputPin
	 */
	OBSBlueprintOutputPin* getFromPin() const { return fromPin; }

	/**
	 * Get the connector input pin \a ptr.
	 * @return The \c OBSBlueprintInputPin* \a (to).
	 * @see OBSBlueprintInputPin
	 */
	OBSBlueprintInputPin* getToPin() const { return toPin; }

	/**
	 * Propagate the data from the \c OBSBlueprintOutputPin to the \c OBSBlueprintInputPin .\n\n
	 * \b NOTE: This function use C function \c memcpy() to copy the data between the pins.
	 * @see OBSBlueprintInputPin
	 * @see OBSBlueprintOutputPin
	 */
	void propagateData() const;

private:
	OBSBlueprintOutputPin* fromPin = nullptr;
	OBSBlueprintInputPin* toPin = nullptr;
};
