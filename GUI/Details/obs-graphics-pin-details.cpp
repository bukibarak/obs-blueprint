#include "obs-graphics-pin-details.h"

#include <QVBoxLayout>

#include "GUI/gui-graph.h"

OBSGraphicsPinDetails::OBSGraphicsPinDetails(GUIContext& context, OBSBlueprintPin *p,
	QWidget *parent, bool forceReadOnly ) : QWidget(parent), ctx(context), pin(p), readOnly(forceReadOnly)
{
	ctx.onDeletion += onGraphDeleted;

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

	value = new OBSGraphicsTypeField(pin->getPinType(), this, TypeConverter::AsString(pin).c_str());
	value->setFixedWidth(value->width());
	if(forceReadOnly || pin->isConnected()) {
		value->setEnabled(false);
	}
	layout->addWidget(value);
	layout->addStretch();

	pin->onConnectionChanged += pinConnectionStateChanged;
	pin->onValueChanged += pinValueChanged;
}

OBSGraphicsPinDetails::~OBSGraphicsPinDetails()
{
	mutex.lock();
	if(!graphDeleted) {
		ctx.onDeletion -= onGraphDeleted;
		pin->onConnectionChanged -= pinConnectionStateChanged;
		pin->onValueChanged -= pinValueChanged;
	}
}
