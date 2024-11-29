#pragma once
#include <QGraphicsProxyWidget>

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
	bool graphDeleted = false;
	GUIContext& ctx;
	OBSGraphicsPin* pin;
	OBSBlueprintPin* bpPin;
	OBSGraphicsTypeField* typeField;

	std::function<void()> onGraphDeleted = [this]{graphDeleted = true;};

	std::function<void()> pinConnectionStateChangedCallback = [this] {
		if(!graphDeleted) {
			if (pin->getBlueprintPin()->isConnected()) {
				hide();

			} else {
				typeField->setValue(TypeConverter::AsString(pin->getBlueprintPin()).c_str());
				show();
			}
		}
	};

	std::function<void(QString)> valueChangedCallback = [this](const QString& value) {TypeConverter::FromString(pin->getBlueprintPin(), value.toStdString());};
};
