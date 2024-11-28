#include "obs-graphics-pin-details.h"

#include <QLabel>
#include <QVBoxLayout>

#include "Core/obs-blueprint-pin.h"
#include "GUI/gui-graph.h"
#include "Helpers/pin-helper.h"

OBSGraphicsPinDetails::OBSGraphicsPinDetails(GUIContext& context, OBSBlueprintPin *p,
	QWidget *parent, bool forceReadOnly ) : QWidget(parent), ctx(context), pin(p), readOnly(forceReadOnly)
{
	ctx.onDeletion += onGraphDeleted;
	pin->onConnectionChanged += onConnectionStateChanged;
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addSpacing(10);

	if(forceReadOnly || pin->isConnected()) {
		connectionPixmap = &PinColors::ConnectedIcon();
	}
	else {
		connectionPixmap = &PinColors::DisconnectedIcon();
	}

	connectionLabel = new QLabel(this);
	connectionLabel->setPixmap(*connectionPixmap);
	layout->addWidget(connectionLabel);
}

OBSGraphicsPinDetails::~OBSGraphicsPinDetails()
{
	if(!graphDeleted) {
		ctx.onDeletion -= onGraphDeleted;
		pin->onConnectionChanged -= onConnectionStateChanged;
	}
}
