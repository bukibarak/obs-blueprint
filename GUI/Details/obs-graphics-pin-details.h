#pragma once
#include <QLabel>
#include <QWidget>

#include "Core/obs-blueprint-pin.h"
#include "Helpers/pin-helper.h"

class QLabel;
struct GUIContext;
class OBSBlueprintPin;

class OBSGraphicsPinDetails : public QWidget {
public:
	OBSGraphicsPinDetails(GUIContext& context, OBSBlueprintPin* pin, QWidget* parent = nullptr, bool forceReadOnly = false);
	~OBSGraphicsPinDetails();

private:
	GUIContext& ctx;
	OBSBlueprintPin* pin;
	bool readOnly;
	bool graphDeleted = false;
	const QPixmap* connectionPixmap;
	QLabel* connectionLabel;

	std::function<void()> onConnectionStateChanged = [this] {
		if(!readOnly) {
			if(pin->isConnected()) connectionPixmap = &PinColors::ConnectedIcon();
			else connectionPixmap = &PinColors::DisconnectedIcon();
			connectionLabel->setPixmap(*connectionPixmap);
			connectionLabel->update();
		}
	};
	std::function<void()> onGraphDeleted = [this] {graphDeleted = true;};

};
