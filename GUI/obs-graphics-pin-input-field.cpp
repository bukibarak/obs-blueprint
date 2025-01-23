#include "obs-graphics-pin-input-field.h"

#include "obs-graphics-main-window.h"

OBSGraphicsPinInputField::OBSGraphicsPinInputField(GUIContext& context,
	OBSGraphicsPin *pin, QGraphicsItem *parent, Qt::WindowFlags wFlags)
	: QGraphicsProxyWidget(parent, wFlags), ctx(context), pin(pin), bpPin(pin->getBlueprintPin())
{
	// QGraphiscProxyWidget associated widget MUST have nullptr parent, see more on setWidget() at the end of constructor.
	field = new OBSGraphicsTypeField({bpPin->getPinType(), bpPin->graphicsOptions}, nullptr, TypeConverter::AsString(pin->getBlueprintPin()).c_str(), false);
	setScale(1.3);
	setZValue(1000);

	mutex.lock();
	bpPin->onConnectionChanged += pinConnectionStateChangedCallback;
	bpPin->onValueChanged += pinValueChanged;
	field->onValueChanged += fieldValueChanged;
	mutex.unlock();

	setWidget(field); // Widget will automatically be deleted by Qt when GraphicsProxy is deleted!
	// See more: https://doc.qt.io/qt-6/qgraphicsproxywidget.html#setWidget
}

OBSGraphicsPinInputField::~OBSGraphicsPinInputField()
{
	mutex.lock();
	bpPin->onConnectionChanged -= pinConnectionStateChangedCallback;
	bpPin->onValueChanged -= pinValueChanged;
	field->onValueChanged -= fieldValueChanged;
	mutex.unlock();
}
