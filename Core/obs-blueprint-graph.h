#pragma once

#include <mutex>


#include "Structs/multicast-delegate.h"

class OBSBlueprintVariable;
struct pixel;
class OBSBlueprintInputPin;
class OBSBlueprintOutputPin;
class OBSBlueprintNode;
class OBSBlueprintConnector;
class GUIGraph;

/**
 * \c OBSBlueprintGraph class. Can contain multiple \c OBSBlueprintNode linked with \c OBSBlueprintConnector .
 * @see OBSBlueprintNode
 * @see OBSBlueprintConnector
 */
class OBSBlueprintGraph {
public:

	/* -------- C function mapped with interface -------- */
	OBSBlueprintGraph();
	~OBSBlueprintGraph();
	void sourcePropertiesClick();
	void tick(float deltaSeconds);
	pixel* getRenderPixels() const;
	uint32_t getWidth() const;
	uint32_t getHeight() const;


	/* -------- C++ only functions -------- */

	void addVariable(OBSBlueprintVariable* variable);
	void deleteVariable(OBSBlueprintVariable* variable);
	const std::list<OBSBlueprintVariable*>& getVariables() const {return graphVariables;}

	/**
	 * Create a \c new connector between two pins on the graph.
	 * @param from the parent \c OBSBlueprintOutputPin*
	 * @param to the child \c OBSBlueprintInputPin*
	 * @return the created \c OBSBlueprintConnector* \a ptr.
	 * @see OBSBlueprintConnector
	 * @see OBSBlueprintInputPin
	 * @see OBSBlueprintOutputPin
	 */
	OBSBlueprintConnector* createConnector(OBSBlueprintOutputPin* from, OBSBlueprintInputPin* to);

	/**
	 * Remove and \c delete an existing connector on the graph.
	 * @param connector the \c OBSBlueprintConnector* to remove.
	 * @see OBSBlueprintConnector::~OBSBlueprintConnector
	 */
	void deleteConnector(OBSBlueprintConnector* connector);

	/**
	 * Add a node on the graph.
	 * @param node The \c OBSBlueprintNode* that will be added to the graph.
	 * @see OBSBlueprintNode
	 */
	void addNode(OBSBlueprintNode* node);

	/**
	 * Remove and \c delete a node on the graph.
	 * @param node The \c OBSBlueprintNode* to remove.
	 * @see OBSBlueprintNode::~OBSBlueprintNode
	 */
	void deleteNode(OBSBlueprintNode* node);

	/**
	 * Get the list of all nodes currently on the graph.
	 * @return The \c std::list const reference of all \c OBSBlueprintNode*
	 * @see OBSBlueprintNode
	 */
	const std::list<OBSBlueprintNode*>& getNodes() const{return graphNodes;}

	/**
	 * Get the list of all connectors currently on the graph.
	 * @return The \c std::list const reference of all \c OBSBlueprintConnector*
	 * @see OBSBlueprintConnector
	 */
	const std::list<OBSBlueprintConnector*>& getConnectors() const {return graphConnectors;}

	/**
	 * Get the graph video pin (the pin data will be used to render the source by OBS)
	 * @return The graph video \c OBSBlueprintInputPin* with value of type \c video_frame
	 * @see video_frame
	 * @see OBSBlueprintInputPin
	 */
	OBSBlueprintInputPin* getMainVideoInputPin() const {return mainVideoInput;}

	/**
	 * Get the graph total execution time since creation. The timer is only increased when the graph is ticked by OBS.
	 * @return The execution time since creation
	 */
	float getGraphTotalTime() const {return graphTime;}

	/** Add callbacks to this delegate if required. It will be called on each \a tick \b before the graph nodes are executed. */
	multicastDelegate_OneParam<float> onGraphBeginTick;

	/** Add callbacks to this delegate if required. It will be called on each \a tick \b after the graph nodes are executed. */
	multicastDelegate_ZeroParam onGraphEndTick;

	std::mutex& mutex() { return mtx; }

private:

	std::mutex mtx;

	float graphTime = 0.0f;

	GUIGraph* guiGraph = nullptr;

	std::list<OBSBlueprintNode*> graphNodes;
	std::list<OBSBlueprintConnector*> graphConnectors;
	std::list<OBSBlueprintVariable*> graphVariables;

	OBSBlueprintInputPin* mainVideoInput = nullptr;
};

