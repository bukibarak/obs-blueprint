#pragma once
#include <list>
#include <vector>
#include "obs-blueprint-connector.h"
#include "Structs/multicast-delegate.h"

/**
 * Base class for OBS Blueprint nodes. This class should never be used directly, only child classes.
 */
class OBSBlueprintNode {

public:
	/**
	 * Internal function. Bind the node with the graph begin/end tick delegates.
	 * @param beginTickDelegate The graph \c onGraphBeginTick delegate.
	 * @param endTickDelegate The graph \c onGraphEndTick delegate.
	 */
	void setupGraphDelegates(multicastDelegate_OneParam<float>& beginTickDelegate, multicastDelegate_ZeroParam& endTickDelegate)
	{
		graphBeginTickDelegate = &beginTickDelegate;
		graphEndTickDelegate = &endTickDelegate;

		*graphBeginTickDelegate += graphBeginTick;
		*graphEndTickDelegate += graphEndTick;
	}

	/**
	 * OBS Blueprint node destructor. This will also delete all pins related to this node.
	 */
	virtual ~OBSBlueprintNode()
	{
		blog(LOG_DEBUG, "Blueprint node destructor called! (%p)", this);
		for(auto inputPin : inputPins) {
			delete inputPin;
		}
		for(auto outputPin : outputPins) {
			delete outputPin;
		}

		if(graphBeginTickDelegate != nullptr) {
			*graphBeginTickDelegate -= graphBeginTick;
		}
		if(graphEndTickDelegate != nullptr) {
			*graphEndTickDelegate -= graphEndTick;
		}
	}

	/**
	 * OBS Blueprint node internal \c tick() function. This function do the following:
	 * 1. Check if the node have already been ticked on this cycle.
	 * 2. Check on all input pins if they are connected and if they need to propagate the tick.
	 * 3. Call the connected parent nodes \c tick() function, if propagation is needed.
	 * 4. Propagate the data to all connected input pins.
	 * 5. Call virtual function \c execute() that will be managed by the child classes.
	 * @param deltaSeconds The time between this \a tick and the last \a tick.
	 */
	void tick(float deltaSeconds)
	{
		if(!haveTickedThisCycle) {
			haveTickedThisCycle = true;
			std::list<OBSBlueprintNode*> parentNodes;

			// Check for all input pins if they are connected and should propagate tick to their parent node
			for(OBSBlueprintInputPin* inputPin : inputPins) {
				if(inputPin->isConnected() && inputPin->shouldPropagateTick()) {
					if(inputPin->getConnector()->getFromPin() == nullptr || inputPin->getConnector()->getFromPin()->getParentNode() == nullptr) {
						blog(LOG_ERROR, "Node input pin is connected, but no corresponding pin and/or node was found!");
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
				if(inputPin != nullptr && inputPin->isConnected()) {
					inputPin->getConnector()->propagateData();
				}
			}

			// Call execute to perform current node process data
			execute(deltaSeconds);
		}
	}

	/**
	 * Pure virtual execute function. All child node classes \b must override this function and add the node execution behavior on it.
	 * @param deltaSeconds The time between this \a tick and the last \a tick.
	 */
	virtual void execute(float deltaSeconds) = 0;

	/** @return \c true if the graph have been ticked, \c false otherwise. */
	bool haveBeenTickedThisCycle() const { return haveTickedThisCycle; }

	/** @return \c true if the graph have been executed, \c false otherwise. */
	bool haveBeenExecutedThisCycle() const { return haveExecutedThisCycle; }

	/**
	 * Get all connector linked with this node
	 * @return A vector of all \c OBSBlueprintConnector* ptr.
	 */
	std::vector<OBSBlueprintConnector*> getAllConnectors() const
	{
		std::vector<OBSBlueprintConnector*> result;
		for(OBSBlueprintInputPin* pin : inputPins) {
			if(pin->isConnected()) {
				result.push_back(pin->getConnector());
			}
		}
		for(OBSBlueprintOutputPin* pin : outputPins) {
			if(pin->isConnected()) {
				result.push_back(pin->getConnector());
			}
		}
		return result;
	}

protected:

	/**
	 * Add a new OBS Blueprint input pin to this node.\n\n
	 * When using this function, you don't need to handle the pin deletion.
	 * @tparam T The pin value data type (not as a pointer).
	 * @param type The pin type. @see PinType.
	 * @param defaultValue The pin initial value. Will be copied to the pin value \a ptr.
	 * @return The input pin created object \a ptr.
	 */
	template<class T>OBSBlueprintInputPin* createInputPin(PinType type, const T& defaultValue)
	{
		OBSBlueprintInputPin* pin = OBSBlueprintInputPin::CreateAndInitialize(type, this, defaultValue);
		addInputPin(pin);
		return pin;
	}

	/**
	 * Add an existing OBS Blueprint input pin to this node.\n\n
	 * When using this function, you don't need to handle the pin deletion.
	 * @param inputPin The existing input pin.
	 */
	void addInputPin(OBSBlueprintInputPin* inputPin) {inputPins.push_back(inputPin);}

	/**
	 * Add a new OBS Blueprint output pin to this node.\n\n
	 * When using this function, you don't need to handle the pin deletion.
	 * @tparam T The pin value data type (not as a pointer).
	 * @param type The pin type. @see PinType.
	 * @param defaultValue The pin initial value. Will be copied to the pin value ptr.
	 * @return The output pin created object ptr.
	 */
	template<class T>OBSBlueprintOutputPin* createOutputPin(PinType type, const T& defaultValue)
	{
		OBSBlueprintOutputPin* pin = OBSBlueprintOutputPin::CreateAndInitialize(type, this, defaultValue);
		addOutputPin(pin);
		return pin;
	}

	/**
	 * Add an existing OBS Blueprint ouptut pin to this node.\n\n
	 * When using this function, you don't need to handle the pin deletion.
	 * @param outputPin The existing output pin.
	 */
	void addOutputPin(OBSBlueprintOutputPin* outputPin) {outputPins.push_back(outputPin);}


	/**
	 * Child nodes can override this function if required. Will be called when the graph begin his \a tick phase.\n
	 * Unlike \c tick() internal function, \c onGraphBeginTick() will be called every time the graph tick, even if the node is not connected or useless.
	 * @param deltaSeconds The time between this \a tick and the last \a tick.
	 */
	virtual void onGraphBeginTick(float deltaSeconds) {}

	/**
	 * Child nodes can override this function if required. Will be called after the graph finished his \a tick phase.\n
	 * \b NOTE: This will be called \b before the graph current video frame is rendered by OBS.
	 */
	virtual void onGraphEndTick() {}

	bool haveTickedThisCycle = false;
	bool haveExecutedThisCycle = false;

private:
	multicastDelegate_OneParam<float>* graphBeginTickDelegate = nullptr;
	multicastDelegate_ZeroParam* graphEndTickDelegate = nullptr;
	std::function<void()> graphEndTick = [this]() {
		onGraphEndTick();
		haveTickedThisCycle = false;
		haveExecutedThisCycle = false;
	};
	std::function<void(float)> graphBeginTick = [this](const float& s) {onGraphBeginTick(s);};


	std::vector<OBSBlueprintInputPin*> inputPins;
	std::vector<OBSBlueprintOutputPin*> outputPins;
};
