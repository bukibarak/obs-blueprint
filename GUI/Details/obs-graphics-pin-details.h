#pragma once
#include <QLabel>
#include <QMutex>

#include "GUI/obs-graphics-type-field.h"
#include "Helpers/pin-helper.h"

class OBSGraphicsTypeField;
class QLabel;
struct GUIContext;
class OBSBlueprintPin;

class OBSGraphicsPinDetails : public QWidget {
public:
	OBSGraphicsPinDetails(GUIContext& context, OBSBlueprintPin* pin, QWidget* parent = nullptr, bool forceReadOnly = false);
	~OBSGraphicsPinDetails() override;

private:

	QMutex mutex;

	GUIContext& ctx;
	OBSBlueprintPin* pin = nullptr;
	OBSGraphicsTypeField* value = nullptr;
	bool readOnly;
	bool graphDeleted = false;
	QLabel* connectionLabel = nullptr;

	std::function<void()> pinConnectionStateChanged = [this] {
		connectionLabel->setPixmap(pin->isConnected() ? PinColors::ConnectedIcon() : PinColors::DisconnectedIcon());
		connectionLabel->update();

		if(!readOnly && mutex.try_lock()) {
			value->setEnabled(!pin->isConnected());
			mutex.unlock();
		}
	};

	std::function<void()> pinValueChanged = [this] {
		if(value && mutex.try_lock()) {
			value->setValue(TypeConverter::AsString(pin).c_str());
			mutex.unlock();
		}
	};

	std::function<void()> onGraphDeleted = [this] {graphDeleted = true;};



};
