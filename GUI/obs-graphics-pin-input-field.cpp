#include "obs-graphics-pin-input-field.h"

#include "obs-graphics-pin.h"
#include "obs-graphics-type-field.h"
#include "Helpers/pin-helper.h"

OBSGraphicsPinInputField::OBSGraphicsPinInputField(OBSGraphicsPin *pin, QGraphicsItem *parent,
                                                   Qt::WindowFlags wFlags) : QGraphicsProxyWidget(parent, wFlags), pin(pin)
{
	typeField = new OBSGraphicsTypeField(pin->getBlueprintPin()->getPinType(), nullptr, TypeConverter::AsString(pin->getBlueprintPin()).c_str());
	setScale(1.3);
	typeField->setMaximumWidth(80);
	// typeField->setAlignment(Qt::AlignRight);
	typeField->onValueChanged += valueChangedCallback;

	setWidget(typeField);

	pinConnectionStateChangedCallback = [this](bool connected) {
		if (connected) {
			hide();

		} else {
			typeField->setValue(TypeConverter::AsString(this->pin->getBlueprintPin()).c_str());
			show();
		}
	};

	pin->onConnectionStateChanged = &pinConnectionStateChangedCallback;
}

OBSGraphicsPinInputField::~OBSGraphicsPinInputField()
{
	pin->onConnectionStateChanged = nullptr;
}
