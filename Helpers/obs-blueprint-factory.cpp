#include "obs-blueprint-factory.h"

#include "Core/obs-blueprint-variable.h"
#include "Structs/obs-frame.h"

OBSBlueprintVariable * OBSBlueprintFactory::CreateAudioVideoVariable(const char* name)
{
	return nullptr; // TODO
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateAudioVariable(const char* name)
{
	return nullptr; // TODO
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateVideoVariable(const char* name)
{
	return OBSBlueprintVariable::CreateVariable<OBSFrame>(VIDEO_PIN, name);
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateBooleanVariable(const char* name)
{
	return OBSBlueprintVariable::CreateVariable<bool>(BOOLEAN_PIN, name);
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateByteVariable(const char *name)
{
	return OBSBlueprintVariable::CreateVariable<uint8_t>(BYTE_PIN, name);
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateIntVariable(const char *name)
{
	return OBSBlueprintVariable::CreateVariable<int32_t>(INT_PIN, name);
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateFloatVariable(const char *name)
{
	return OBSBlueprintVariable::CreateVariable<float>(FLOAT_PIN, name);
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateCharVariable(const char *name)
{
	return OBSBlueprintVariable::CreateVariable<char>(CHAR_PIN, name);
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateStringVariable(
	const char *name)
{
	return OBSBlueprintVariable::CreateVariable<std::string>(STRING_PIN, name);
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateColorVariable(
	const char *name)
{
	return OBSBlueprintVariable::CreateVariable<uint32_t>(COLOR_PIN, name);
}

OBSBlueprintVariable * OBSBlueprintFactory::CreateVariableFromPin(
	const PinType &type, const char *name)
{
	switch(type) {
	case AUDIOVIDEO_PIN:
		return CreateAudioVideoVariable(name);
	case AUDIO_PIN:
		return CreateAudioVariable(name);
	case VIDEO_PIN:
		return CreateVideoVariable(name);
	case BOOLEAN_PIN:
		return CreateBooleanVariable(name);
	case BYTE_PIN:
		return CreateByteVariable(name);
	case INT_PIN:
		return CreateIntVariable(name);
	case FLOAT_PIN:
		return CreateFloatVariable(name);
	case CHAR_PIN:
		return CreateCharVariable(name);
	case STRING_PIN:
		return CreateStringVariable(name);
	case COLOR_PIN:
		return CreateColorVariable(name);
	default:
		return nullptr;
	}
}


