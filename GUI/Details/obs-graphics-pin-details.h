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
	OBSGraphicsTypeField* field = nullptr;
	bool readOnly;
	QLabel* connectionLabel = nullptr;

	std::function<void()> pinConnectionStateChanged = [this] {
		connectionLabel->setPixmap(pin->isConnected() ? PinColors::ConnectedIcon() : PinColors::DisconnectedIcon());
		connectionLabel->update();

		if(!readOnly && mutex.try_lock()) {
			field->setEnabled(!pin->isConnected());
			mutex.unlock();
		}
	};

	std::function<void()> pinValueChanged = [this] {
		if(field && mutex.try_lock()) {
			field->setValue(TypeConverter::AsString(pin).c_str());
			mutex.unlock();
		}
	};

	std::function<void(QString)> fieldValueChanged = [this](const QString& newVal) {
		TypeConverter::FromString(pin, newVal.toStdString());
	};
};
