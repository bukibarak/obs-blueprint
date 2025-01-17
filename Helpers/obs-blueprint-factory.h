#pragma once
#include <vector>


class OBSBlueprintNode;
enum PinType : unsigned char;
class OBSBlueprintVariable;

/**
 * Helper to create new nodes.
 * @tparam T \b Must be a subclass of \c OBSBlueprintNode
 */
template<class T>
class NodeFactory {
public:

	/** Create a new node with default values. */
	static OBSBlueprintNode* Create() { return T::CreateDefault(); }

	/** List of all input pin types accepted by this node. */
	static std::vector<PinType> Inputs() { return T::InputPins(); }

	/** List of all output pin types accepted by this node. */
	static std::vector<PinType> Outputs() { return T::OutputPins(); }
};

/** Helper to create new OBS Blueprint objects, like \c OBSBlueprintVariable (mostly implemented) and \c OBSBlueprintNode (not implemented yet) */
class OBSBlueprintFactory {
public:

	// TODO: How to create node from factory with default parameters obtained from template class, +- like vector.emplace_back()
	template<class T>
	static OBSBlueprintNode* CreateNode()
	{
		return NodeFactory<T>::Create();
	}

	static OBSBlueprintVariable* CreateAudioVideoVariable(const char* name = ""); // TODO
	static OBSBlueprintVariable* CreateAudioVariable(const char* name = ""); // TODO
	static OBSBlueprintVariable* CreateVideoVariable(const char* name = "");
	static OBSBlueprintVariable* CreateBooleanVariable(const char* name = "");
	static OBSBlueprintVariable* CreateByteVariable(const char* name = "");
	static OBSBlueprintVariable* CreateIntVariable(const char* name = "");
	static OBSBlueprintVariable* CreateFloatVariable(const char* name = "");
	static OBSBlueprintVariable* CreateCharVariable(const char* name = "");
	static OBSBlueprintVariable* CreateStringVariable(const char* name = "");
	static OBSBlueprintVariable* CreateColorVariable(const char* name = "");
	static OBSBlueprintVariable* CreateVariable(const PinType& type, const char* name = "");
};
