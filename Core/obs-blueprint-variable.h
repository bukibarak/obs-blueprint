﻿#pragma once
#include "pin-type.h"
#include <functional>
#include <string>
#include "Helpers/global-logger.h"

class OBSBlueprintGraph;

class OBSBlueprintVariable {
public:

	OBSBlueprintVariable() = delete;
	OBSBlueprintVariable(const OBSBlueprintVariable&) = delete;
	OBSBlueprintVariable(OBSBlueprintVariable&&) = delete;
	OBSBlueprintVariable& operator=(const OBSBlueprintVariable&) = delete;
	OBSBlueprintVariable& operator=(OBSBlueprintVariable&&) = delete;

	template<class T> static OBSBlueprintVariable* CreateVariable(const PinType& pinType, const char* name = "")
	{
		OBSBlueprintVariable* variable = new OBSBlueprintVariable(name);
		variable->init<T>(pinType);
#if DEBUG
		GInfo("Created new variable '%s' of type [%s]", name, PinName[pinType]);
#endif
		return variable;
	}

	~OBSBlueprintVariable()
	{
		if(onDestructor) onDestructor();
		else GWarn("Couldn't delete variable value ptr, possible memory leak!");
		GInfo("Variable '%s' [%s] deleted!", displayName.c_str(), PinName[pinType]);
	}

	const PinType& getPinType() const {return pinType;}
	template<class T> T* getValuePtr() { return static_cast<T*>(rawValuePtr); }
	template<class T>const T& getValue() { return *static_cast<T*>(rawValuePtr); }
	template<class T> void setValue(const T& value) { *static_cast<T*>(rawValuePtr) = value; }

	const char* getDisplayName() const {return displayName.c_str();}

	void setupGraph(OBSBlueprintGraph* graph) {parentGraph = graph;}
	OBSBlueprintGraph* getParentGraph() const {return parentGraph;}

private:

	OBSBlueprintVariable(const char* name) : displayName(name) {}

	template<class T>void init(const PinType& type)
	{
		pinType = type;
		rawValuePtr = new T();
		rawValueSize = sizeof(T);
		onDestructor = [this]{delete static_cast<T*>(rawValuePtr);};
	}

	PinType pinType = UNKNOWN_PIN;
	void* rawValuePtr = nullptr;
	unsigned int rawValueSize = 0;
	const std::string displayName;
	std::function<void()> onDestructor;
	OBSBlueprintGraph* parentGraph = nullptr;
};