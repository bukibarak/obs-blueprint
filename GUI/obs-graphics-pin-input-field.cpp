#include "obs-graphics-pin-input-field.h"

#include "gui-graph.h"

OBSGraphicsPinInputField::OBSGraphicsPinInputField(GUIContext& context,
	OBSGraphicsPin *pin, QGraphicsItem *parent, Qt::WindowFlags wFlags)
	: QGraphicsProxyWidget(parent, wFlags), ctx(context), pin(pin), bpPin(pin->getBlueprintPin())
{
	ctx.onDeletion += onGraphDeleted;
	bpPin->onConnectionChanged += pinConnectionStateChangedCallback;

	typeField = new OBSGraphicsTypeField(bpPin->getPinType(), nullptr, TypeConverter::AsString(pin->getBlueprintPin()).c_str(), false);
	setScale(1.3);
	typeField->setMaximumWidth(80);
	// typeField->setAlignment(Qt::AlignRight);
	typeField->onValueChanged += valueChangedCallback;

	setWidget(typeField); // Widget will automatically be deleted by Qt when GraphicsProxy is deleted!
}

OBSGraphicsPinInputField::~OBSGraphicsPinInputField()
{
	if(!graphDeleted) {
		ctx.onDeletion -= onGraphDeleted;
		bpPin->onConnectionChanged -= pinConnectionStateChangedCallback;
		typeField->onValueChanged -= valueChangedCallback;
	}
}
