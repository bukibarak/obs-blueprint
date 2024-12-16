#include "obs-graphics-pin-details.h"

#include <QVBoxLayout>

#include "GUI/obs-graphics-main-window.h"


OBSGraphicsPinDetails::OBSGraphicsPinDetails(GUIContext& context, OBSBlueprintPin *p,
                                             QWidget *parent, bool forceReadOnly ) : QWidget(parent), ctx(context), pin(p), readOnly(forceReadOnly)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	// layout->setSizeConstraint(QLayout::SetMaximumSize);
	layout->addSpacing(10);

	connectionLabel = new QLabel(this);
	connectionLabel->setPixmap(pin->isConnected() ? PinColors::ConnectedIcon() : PinColors::DisconnectedIcon());
	layout->addWidget(connectionLabel);
	layout->addSpacing(2);

	QLabel* nameLabel = new QLabel(pin->getDisplayName(), this);
	layout->addWidget(nameLabel);

	field = new OBSGraphicsTypeField(pin->getPinType(), this, TypeConverter::AsString(pin).c_str());
	field->setFixedWidth(field->width());
	if(forceReadOnly || pin->isConnected()) {
		field->setEnabled(false);
	}
	layout->addWidget(field);
	layout->addStretch();

	mutex.lock();
	pin->onConnectionChanged += pinConnectionStateChanged;
	pin->onValueChanged += pinValueChanged;
	field->onValueChanged += fieldValueChanged;
	mutex.unlock();
}

OBSGraphicsPinDetails::~OBSGraphicsPinDetails()
{
	mutex.lock();
	pin->onConnectionChanged -= pinConnectionStateChanged;
	pin->onValueChanged -= pinValueChanged;
	field->onValueChanged -= fieldValueChanged;
	mutex.unlock();
}
