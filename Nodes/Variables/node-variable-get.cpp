#include "node-variable-get.h"

#include "Core/obs-blueprint-variable.h"
#include "Structs/obs-frame.h"

NodeVariableGet::NodeVariableGet(OBSBlueprintVariable *linkedVariable)
	: OBSBlueprintNode("GET " + std::string(linkedVariable->getDisplayName())), variable(linkedVariable)
{
	graphicsOptions.isCompactNode = true;
	variable->onRename += variableRenamed;

	switch(variable->getPinType()) {
	case VIDEO_PIN:
		pin = createOutputPin(VIDEO_PIN, OBSFrame::EmptyFrame, "video");
		break;
	case BOOLEAN_PIN:
		pin = createOutputPin(BOOLEAN_PIN, bool(), "bool");
		break;
	case BYTE_PIN:
		pin = createOutputPin(BYTE_PIN, uint8_t(), "byte");
		break;
	case INT_PIN:
		pin = createOutputPin(INT_PIN, int32_t(), "int");
		break;
	case FLOAT_PIN:
		pin = createOutputPin(FLOAT_PIN, float(), "float");
		break;
	case CHAR_PIN:
		pin = createOutputPin(CHAR_PIN, char(), "char");
		break;
	case STRING_PIN:
		pin = createOutputPin(STRING_PIN, std::string(), "string");
		break;
	case COLOR_PIN:
		pin = createOutputPin(COLOR_PIN, uint32_t(), "color");
		break;
	default:
		GError("[GET %s] Couldn't create node pin because it is of type %s which is unsupported", variable->getDisplayName(), EnumStr::PinType[variable->getPinType()]);
	}
}

NodeVariableGet::~NodeVariableGet()
{
	variable->onRename -= variableRenamed;
}

void NodeVariableGet::execute(float deltaSeconds)
{
	if(pin == nullptr) {
		GError("[GET %s] Node was not correctly initialized, couldn't execute", variable->getDisplayName());
	}
	else if(pin->getPinType() != variable->getPinType()) {
		GError("[GET %s] Pin type mismatch, cannot copy variable into pin", variable->getDisplayName());
	}
	else {
		switch(variable->getPinType()) {
		case VIDEO_PIN:
			pin->setValue(variable->getValue<OBSFrame>());
			break;
		case BOOLEAN_PIN:
			pin->setValue(variable->getValue<bool>());
			break;
		case BYTE_PIN:
			pin->setValue(variable->getValue<uint8_t>());
			break;
		case INT_PIN:
			pin->setValue(variable->getValue<int32_t>());
			break;
		case FLOAT_PIN:
			pin->setValue(variable->getValue<float>());
			break;
		case CHAR_PIN:
			pin->setValue(variable->getValue<char>());
			break;
		case STRING_PIN:
			pin->setValue(variable->getValue<std::string>());
			break;
		case COLOR_PIN:
			pin->setValue(variable->getValue<uint32_t>());
			break;
		default:
			GError("[GET %s] Couldn't copy variable value to pin because it is of type %s which is unsupported", variable->getDisplayName(), EnumStr::PinType[variable->getPinType()]);
			return;
		}
		haveExecutedThisCycle = true;
	}
}
