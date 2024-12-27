#pragma once
#include <QGraphicsProxyWidget>
#include <QMutex>

#include "obs-graphics-type-field.h"
#include "Helpers/pin-helper.h"

struct GUIContext;

class OBSGraphicsPinInputField : public QGraphicsProxyWidget {
public:
	enum { Type = UserType + 4 };
	OBSGraphicsPinInputField(GUIContext& context, OBSGraphicsPin* pin, QGraphicsItem* parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
	~OBSGraphicsPinInputField() override;
	int type() const override {return Type;}

private:

	QMutex mutex{};

	GUIContext& ctx;
	OBSGraphicsPin* pin = nullptr;
	OBSBlueprintPin* bpPin = nullptr;
	OBSGraphicsTypeField* field = nullptr;

	std::function<void()> pinConnectionStateChangedCallback = [this] {
		if (pin->getBlueprintPin()->isConnected()) {
			hide();

		} else if(field && mutex.try_lock()) {
			field->setValue(TypeConverter::AsString(pin->getBlueprintPin()).c_str());
			show();
			mutex.unlock();
		}
	};

	std::function<void()> pinValueChanged = [this] {
		if(field && mutex.try_lock()) {
			field->setValue(TypeConverter::AsString(pin->getBlueprintPin()).c_str());
			mutex.unlock();
		}
	};

	std::function<void(QString)> fieldValueChanged = [this](const QString& value) {TypeConverter::FromString(pin->getBlueprintPin(), value.toStdString());};
};
