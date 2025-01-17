#pragma once

#include "pin-type.h"
#include "Helpers/enum-to-string.h"
#include "Helpers/global-logger.h"
#include "Structs/graphics-options.h"
#include "Structs/multicast-delegate.h"

class OBSBlueprintConnector;
class OBSBlueprintGraph;
class OBSBlueprintNode;

/**
 * Base class for \c OBSBlueprintPin. This class should never be used directly, only child classes \c OBSBlueprintInputPin and \c OBSBlueprintOutputPin.
 * @see OBSBlueprintInputPin
 * @see OBSBlueprintOutputPin
 */
class OBSBlueprintPin {
	friend class OBSBlueprintConnector;
public:
	/**
	 * Get the pin type.
	 * @return The pin type.
	 */
	const PinType& getPinType() const {return pinType;}

	/**
	 * Get the node associated with this pin.
	 * @return The pin's owner node. Can be \c nullptr if the pin is not owned by a node.
	 * @see OBSBlueprintNode.
	 */
	OBSBlueprintNode* getParentNode() const {return parentNode;}

	/**
	 * Get the graph associated with this pin.
	 * @return The pin's owner graph. Can be \c nullptr if the pin is not owned by a graph.
	 * @see OBSBlueprintGraph.
	 */
	OBSBlueprintGraph* getParentGraph() const {return parentGraph;}


	/**
	 * Get the pointer of the value stored in this pin.\n
	 * \b NOTE: You should \b never delete the pointer by yourself, this will be done automatically when the pin is deleted.
	 * @tparam T The pin data type (not as a pointer).
	 * @return The pin value ptr in the given data type.
	 */
	template<class T> T* getValuePtr() { return static_cast<T*>(rawValuePtr); }

	/**
	 * Get a const reference of the value stored in this pin.
	 * @tparam T The pin data type (not as a pointer).
	 * @return A copy of the pin value in the given data type.
	 */
	template<class T>const T& getValue() { return *static_cast<T*>(rawValuePtr); }

	/**
	 * Set the pin ptr value to a copy (?) of the given value.
	 * @tparam T  The pin data type (not as a pointer).
	 * @param value The new value that will be copied to the pin value ptr.
	 */
	template<class T> void setValue(const T& value) { *static_cast<T*>(rawValuePtr) = value; onValueChanged.execute(); }
	// {
	// 	unsigned int valueSize = sizeof(T);
	// 	if(valueSize != rawValueSize) GError("SIZE MISMATCH when trying to set value, unexpected behavior may happen! %u (given) != %u (stored)", sizeof(T), rawValueSize);
	// 	memcpy(rawValuePtr, &value, std::min(valueSize, rawValueSize));
	// }

	/**
	 * Get the pin display name string reference (const to prevent editing name)
	 * @return The pin display name.
	 */
	const char* getDisplayName() const { return displayName.c_str(); }

	virtual bool isConnected() const = 0;

	/** Pin graphical options managed by GUI (Qt) */
	PinGraphicsOptions graphicsOptions;

	/** This delegate is called by each time the connection change, see \c OBSBlueprintConnector . Used to update UI */
	multicastDelegate_ZeroParam onConnectionChanged;

	/** This delegate is called each time the value change. Used to update UI */
	multicastDelegate_ZeroParam onValueChanged;

protected:

	// Constructors (protected to prevent direct use of the class)
	OBSBlueprintPin(const PinType& type, OBSBlueprintNode* parent, std::string name) : displayName(std::move(name)), pinType(type), parentNode(parent) {}
	OBSBlueprintPin(const PinType& type, OBSBlueprintGraph* parent, std::string name) : displayName(std::move(name)), pinType(type), parentGraph(parent) {}

	// Call onDestructor in order to delete value ptr
	virtual ~OBSBlueprintPin()
	{
		if(onDestructor) onDestructor();
		else GWarn("Couldn't delete pin value ptr, possible memory leak!");
		GInfo("%s '%s' destroyed", EnumStr::PinType[pinType], getDisplayName());
	}

	template<class T> void initializeValue(const T& defaultValue)
	{
		rawValuePtr = new T(defaultValue);
		rawValueSize = sizeof(T);
		onDestructor = [this] {
			T* test = static_cast<T*>(rawValuePtr);
			delete test;
		};
	}

	std::function<void()> onDestructor;

private:

	std::string displayName;
	PinType pinType = UNKNOWN_PIN;

	void* rawValuePtr = nullptr;
	unsigned int rawValueSize = 0;

	OBSBlueprintNode* parentNode = nullptr;
	OBSBlueprintGraph* parentGraph = nullptr;
};

/**
 * \c OBSBlueprintOutputPin class. Use static function \c OBSBlueprintOutputPin::CreateAndInitialize() to create a new pin.
 * @see OBSBlueprintOutputPin::CreateAndInitialize()
 */
class OBSBlueprintOutputPin : public OBSBlueprintPin {
	friend class OBSBlueprintConnector;
public:

	/**
	 * Create and initialize a new OBS Blueprint output pin.\n\n
	 * Unless specific usage, this should \b not be called directly. Use \c OBSBlueprintNode::createOutputPin() instead. \n
	 * If you still need to use this function, do not forget to call \c OBSBlueprintNode::addOutputPin() afterward.
	 * @tparam T The pin value data type (not as a pointer).
	 * @param type The pin type. @see PinType.
	 * @param parent The node associated to the output pin.
	 * @param initialValue The pin initial value. Will be copied to the pin value ptr.
	 * @param displayName The pin name.
	 * @return The output pin created object ptr.
	 * @see OBSBlueprintNode::createOutputPin()
	 * @see OBSBlueprintNode::addOutputPin()
	 */
	template<class T> static OBSBlueprintOutputPin* CreateAndInitialize(const PinType& type, OBSBlueprintNode* parent, const T& initialValue, const std::string& displayName = "")
	{
		OBSBlueprintOutputPin* pin = new OBSBlueprintOutputPin(type, parent, displayName);
		pin->initializeValue(initialValue);
		GInfo("Created and initialized output pin '%s' of type %s", displayName.c_str(), EnumStr::PinType[type]);
		return pin;
	}

	/**
	 * Get a list of all connectors linked to this pin.
	 * @return A \c std::list const reference of all \c OBSBlueprintConnector*
	 */
	const std::list<OBSBlueprintConnector*>& getConnectors() const { return connectors; }

	/** @return \c true if the pin have at least one connector, \c false otherwise. */
	bool isConnected() const  { return !connectors.empty(); }

private:

	std::list<OBSBlueprintConnector*> connectors;

	// Private constructor, use static function instead
	OBSBlueprintOutputPin(const PinType& type, OBSBlueprintNode* parent, const std::string& name) : OBSBlueprintPin(type, parent, name){}
};

/**
 * \c OBSBlueprintInputPin class. Use static function \c OBSBlueprintInputPin::CreateAndInitialize() to create a new pin.
 * @see OBSBlueprintInputPin::CreateAndInitialize()
 */
class OBSBlueprintInputPin : public OBSBlueprintPin {
	friend class OBSBlueprintConnector;
public:
	/**
	 * Create and initialize a new OBS Blueprint input pin.\n\n
	 * Unless specific usage, this should \b not be called directly. Use \c OBSBlueprintNode::createInputPin() instead.\n
	 * If you still need to use this function, do not forget to call \c OBSBlueprintNode::addInputPin() afterward.
	 * @tparam T The pin value data type (not as a pointer).
	 * @param type The pin type. @see PinType.
	 * @param parent The node associated to the input pin.
	 * @param initialValue The pin initial value. Will be copied to the pin value ptr.
	 * @param displayName The pin name.
	 * @return The input pin created object ptr.
	 * @see OBSBlueprintNode::createInputPin()
	 * @see OBSBlueprintNode::addInputPin()
	 */
	template<class T> static OBSBlueprintInputPin* CreateAndInitialize(const PinType& type, OBSBlueprintNode* parent, const T& initialValue, const std::string& displayName = "")
	{
		OBSBlueprintInputPin* pin = new OBSBlueprintInputPin(type, parent, displayName);
		pin->initializeValue(initialValue);
		GInfo("Created and initialized input pin '%s' of type %s", displayName.c_str(), EnumStr::PinType[type]);
		return pin;
	}

	/**
	 * Create and initialize a new OBS Blueprint input pin associated with a graph.\n
	 * Unlike nodes, the graph need to handle pin deletion (i.e.: on graph destructor)
	 * @tparam T The pin value data type (not as a pointer).
	 * @param type The pin type. @see PinType.
	 * @param parent The graph associated to the input pin.
	 * @param initialValue The pin initial value. Will be copied to the pin value ptr.
	 * @param displayName The pin name.
	 * @return The input pin created object ptr.
	 */
	template<class T> static OBSBlueprintInputPin* CreateAndInitialize(const PinType& type, OBSBlueprintGraph* parent, const T& initialValue, const std::string& displayName = "")
	{
		OBSBlueprintInputPin* pin = new OBSBlueprintInputPin(type, parent, displayName);
		pin->initializeValue(initialValue);
		GInfo("Created and initialized graph input pin '%s' of type %s", displayName.c_str(), EnumStr::PinType[type]);
		return pin;
	}

	/**
	 * Get the pin's connector.
	 * @return The \c OBSBlueprintConnector* associated with this pin. Can be \c nullptr if the pin is not connected.
	 * @see OBSBlueprintConnector
	 */
	OBSBlueprintConnector* getConnector() const {return connector;}

	/** @return \c true if the pin is connected, \c false otherwise. */
	bool isConnected() const { return connector != nullptr; }

	/**
	 * Override this function to indicate if the pin should propagate the \a tick to the parent node connected to it.\n
	 * Not ticking unnecessary parent nodes can improve performance.
	 */
	std::function<bool()> shouldPropagateTick = [] { return true; };

private:

	OBSBlueprintConnector* connector = nullptr;

	// Private constructor, use static function instead
	OBSBlueprintInputPin(const PinType& type, OBSBlueprintNode* parent, const std::string& name) : OBSBlueprintPin(type, parent, name){}
	// Private constructor, use static function instead
	OBSBlueprintInputPin(const PinType& type, OBSBlueprintGraph* parent, const std::string& name) : OBSBlueprintPin(type, parent, name){}
};
