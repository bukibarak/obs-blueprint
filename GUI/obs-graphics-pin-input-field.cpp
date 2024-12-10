#include "obs-graphics-pin-input-field.h"

#include "gui-graph.h"

OBSGraphicsPinInputField::OBSGraphicsPinInputField(GUIContext& context,
	OBSGraphicsPin *pin, QGraphicsItem *parent, Qt::WindowFlags wFlags)
	: QGraphicsProxyWidget(parent, wFlags), ctx(context), pin(pin), bpPin(pin->getBlueprintPin())
{
	ctx.onDeletion += onGraphDeleted;


	field = new OBSGraphicsTypeField(bpPin->getPinType(), nullptr, TypeConverter::AsString(pin->getBlueprintPin()).c_str(), false);
	setScale(1.3);
	//field->setMaximumWidth(80);
	// typeField->setAlignment(Qt::AlignRight);


	bpPin->onConnectionChanged += pinConnectionStateChangedCallback;
	bpPin->onValueChanged += pinValueChanged;
	field->onValueChanged += fieldValueChanged;

	setWidget(field); // Widget will automatically be deleted by Qt when GraphicsProxy is deleted!
}

OBSGraphicsPinInputField::~OBSGraphicsPinInputField()
{
	mutex.lock();
	if(!graphDeleted) {
		ctx.onDeletion -= onGraphDeleted;
		bpPin->onConnectionChanged -= pinConnectionStateChangedCallback;
		field->onValueChanged -= fieldValueChanged;
	}
	mutex.unlock();
}
