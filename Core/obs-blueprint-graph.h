#pragma once

#include "obs-blueprint-pin.h"
#include "Structs/multicast-delegate.h"
#include "Structs/pixel.h"

class GUIGraph;

class OBSBlueprintGraph {
public:
	OBSBlueprintGraph();
	~OBSBlueprintGraph();

	/* C function mapped with interface */
	void sourcePropertiesClick();
	void tick(float deltaSeconds);
	pixel* getRenderPixels() const;
	uint32_t getWidth() const;
	uint32_t getHeight() const;

	/* C++ only functions */

	/**
	 * Create a new connector between two pins on the graph.
	 * @param from the parent output pin.
	 * @param to the child input pin.
	 */
	OBSBlueprintConnector* createConnector(OBSBlueprintOutputPin* from, OBSBlueprintInputPin* to);

	/**
	 * Remove an existing connector on the graph.
	 * @param connector the connector to remove.
	 */
	void deleteConnector(OBSBlueprintConnector* connector);

	/**
	 * Add a node to this graph.
	 * @param node The node that will be added to the graph.
	 */
	void addNode(OBSBlueprintNode* node);

	/**
	 * Remove the node from the graph and delete it.
	 * @param node The node that will be removed.
	 */
	void deleteNode(OBSBlueprintNode* node);

	const std::list<OBSBlueprintNode*>& getNodes() const {return graphNodes;}
	const std::list<OBSBlueprintConnector*>& getConnectors() const {return graphConnectors;}
	OBSBlueprintInputPin* getMainVideoInputPin() const {return mainVideoInput;}

	multicastDelegate_OneParam<float> onGraphBeginTick;
	multicastDelegate_ZeroParam onGraphEndTick;

private:

	GUIGraph* guiGraph = nullptr;

	std::list<OBSBlueprintNode*> graphNodes;
	std::list<OBSBlueprintConnector*> graphConnectors;

	OBSBlueprintInputPin* mainVideoInput = nullptr;
};

