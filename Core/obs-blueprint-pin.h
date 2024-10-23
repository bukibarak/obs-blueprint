#pragma once
#include <functional>

class OBSBlueprintConnector;
class OBSBlueprintGraph;
class OBSBlueprintNode;
class GUIPin;

/**
 * All OBS Blueprint pin types that can be used with GUI graph.
 * Pins with type \c UNKNOWN_PIN will not be displayed.
 *
 * All PinType have a corresponding type. Please see ??? for more details.
 */
enum PinType {
	UNKNOWN_PIN,
	AUDIOVIDEO_PIN,
	AUDIO_PIN,
	VIDEO_PIN,
	BYTE_PIN,
	INT_PIN,
	FLOAT_PIN,
	CHAR_PIN,
	STRING_PIN,
	COLOR_PIN,
};

constexpr const char* PinName[] = {
	"UNKNOWN_PIN",
	"AUDIOVIDEO_PIN",
	"AUDIO_PIN",
	"VIDEO_PIN",
	"BYTE_PIN",
	"INT_PIN",
	"FLOAT_PIN",
	"CHAR_PIN",
	"STRING_PIN",
	"COLOR_PIN"
};


/**
 * Base class for OBS Blueprint pins. This class should never be used directly, only child classes.
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
	PinType getPinType() const {return pinType;}

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
	 * Get the pin's connector.
	 * @return The connector associated with this pin. Can be \c nullptr if the pin is not connected.
	 * @see OBSBlueprintConnector.
	 */
	OBSBlueprintConnector* getConnector() const {return connector;}

	/**
	 * Check if a pin is associated with a connector.
	 * @return \c true if there is a connector associated, \c false otherwise.
	 * @see OBSBlueprintConnector.
	 */
	bool isConnected() const { return connector != nullptr; }


	/**
	 * Get the pointer of the value stored in this pin.\n
	 * \b NOTE: You should NEVER delete the pointer by yourself, this will be done automatically when the pin is deleted.
	 * @tparam T The pin data type (not as a pointer).
	 * @return The pin value ptr in the given data type.
	 */
	template<class T> T* getValuePtr() { return static_cast<T*>(rawValuePtr); }

	/**
	 * Get a copy of the value stored in this pin.
	 * @tparam T The pin data type (not as a pointer).
	 * @return A copy of the pin value in the given data type.
	 */
	template<class T> T getValue() { return *static_cast<T*>(rawValuePtr); }

	/**
	 * Set the pin ptr value to a copy of the given value.
	 * @tparam T  The pin data type (not as a pointer).
	 * @param value The new value that will be copied to the pin value ptr.
	 */
	template<class T> void setValue(const T& value) { *static_cast<T*>(rawValuePtr) = value; }

protected:

	// Constructors (protected to prevent direct use of the class)
	OBSBlueprintPin(OBSBlueprintNode* parent) : parentNode(parent) {}
	OBSBlueprintPin(OBSBlueprintGraph* parent) : parentGraph(parent) {}

	// Call onDestructor in order to delete value ptr
	virtual ~OBSBlueprintPin() {if(onDestructor) onDestructor();}

	template<class T> void initializeValue(const T& defaultValue)
	{
		rawValuePtr = new T(defaultValue);
		rawValueSize = sizeof(T);
		onDestructor = [this]() {delete static_cast<T*>(rawValuePtr);};
	}

	std::function<void()> onDestructor;
	PinType pinType = UNKNOWN_PIN;

private:

	void* rawValuePtr = nullptr;
	unsigned int rawValueSize = 0;

	OBSBlueprintNode* parentNode = nullptr;
	OBSBlueprintGraph* parentGraph = nullptr;
	OBSBlueprintConnector* connector = nullptr;
};

/**
 * OBS Blueprint output pin class. Use static function \c OBSBlueprintOutputPin::CreateAndInitialize() to create a new pin.
 * @see OBSBlueprintOutputPin::CreateAndInitialize()
 */
class OBSBlueprintOutputPin : public OBSBlueprintPin {
public:

	/**
	 * Create and initialize a new OBS Blueprint output pin.\n\n
	 * Unless specific usage, this should \b not be called directly. Use \c OBSBlueprintNode::createOutputPin() instead. \n
	 * If you still need to use this function, do not forget to call \c OBSBlueprintNode::addOutputPin() afterward.
	 * @tparam T The pin value data type (not as a pointer).
	 * @param type The pin type. @see PinType.
	 * @param parent The node associated to the output pin.
	 * @param initialValue The pin initial value. Will be copied to the pin value ptr.
	 * @return The output pin created object ptr.
	 * @see OBSBlueprintNode::createOutputPin()
	 * @see OBSBlueprintNode::addOutputPin()
	 */
	template<class T> static OBSBlueprintOutputPin* CreateAndInitialize(const PinType& type, OBSBlueprintNode* parent, const T& initialValue)
	{
		OBSBlueprintOutputPin* pin = new OBSBlueprintOutputPin(type, parent);
		pin->initializeValue(initialValue);
		return pin;
	}

private:

	// Private constructor, use static function instead
	OBSBlueprintOutputPin(const PinType& type, OBSBlueprintNode* parent) : OBSBlueprintPin(parent) {pinType = type;}
};

/**
 * OBS Blueprint input pin class. Use static function \c OBSBlueprintInputPin::CreateAndInitialize() to create a new pin.
 * @see OBSBlueprintInputPin::CreateAndInitialize()
 */
class OBSBlueprintInputPin : public OBSBlueprintPin {
public:
	/**
	 * Create and initialize a new OBS Blueprint input pin.\n\n
	 * Unless specific usage, this should \b not be called directly. Use \c OBSBlueprintNode::createInputPin() instead.\n
	 * If you still need to use this function, do not forget to call \c OBSBlueprintNode::addInputPin() afterward.
	 * @tparam T The pin value data type (not as a pointer).
	 * @param type The pin type. @see PinType.
	 * @param parent The node associated to the input pin.
	 * @param initialValue The pin initial value. Will be copied to the pin value ptr.
	 * @return The input pin created object ptr.
	 * @see OBSBlueprintNode::createInputPin()
	 * @see OBSBlueprintNode::addInputPin()
	 */
	template<class T> static OBSBlueprintInputPin* CreateAndInitialize(const PinType& type, OBSBlueprintNode* parent, const T& initialValue)
	{
		OBSBlueprintInputPin* pin = new OBSBlueprintInputPin(type, parent);
		pin->initializeValue(initialValue);
		return pin;
	}

	/**
	 * Create and initialize a new OBS Blueprint input pin associated with a graph.\n
	 * Unlike nodes, the graph need to handle pin deletion (i.e.: on graph destructor)
	 * @tparam T The pin value data type (not as a pointer).
	 * @param type The pin type. @see PinType.
	 * @param parent The graph associated to the input pin.
	 * @param initialValue The pin initial value. Will be copied to the pin value ptr.
	 * @return The input pin created object ptr.
	 */
	template<class T> static OBSBlueprintInputPin* CreateAndInitialize(const PinType& type, OBSBlueprintGraph* parent, const T& initialValue)
	{
		OBSBlueprintInputPin* pin = new OBSBlueprintInputPin(type, parent);
		pin->initializeValue(initialValue);
		return pin;
	}

	/**
	 * Override this function to indicate if the pin should propagate the \a tick to the parent node connected to it.
	 * Not ticking unnecessary parent nodes can improve performance.
	 */
	std::function<bool()> shouldPropagateTick = []() { return true; };

private:
	// Private constructor, use static function instead
	OBSBlueprintInputPin(const PinType& type, OBSBlueprintNode* parent) : OBSBlueprintPin(parent) {pinType = type;}
	// Private constructor, use static function instead
	OBSBlueprintInputPin(const PinType& type, OBSBlueprintGraph* parent) : OBSBlueprintPin(parent) {pinType = type;}
};
