#pragma once
#include "Core/pin-type.h"

class OBSBlueprintVariable;

class OBSBlueprintFactory {
public:

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
	static OBSBlueprintVariable* CreateVariableFromPin(const PinType& type, const char* name = "");
};
