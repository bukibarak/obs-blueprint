#pragma once
#include <QGraphicsProxyWidget>
#include <QIntValidator>
#include <QLineEdit>

#include "Helpers/pin-helper.h"


class OBSGraphicsTypeField;
class OBSGraphicsPin;

class OBSGraphicsPinInputField : public QGraphicsProxyWidget {
public:
	enum { Type = UserType + 4 };
	OBSGraphicsPinInputField(OBSGraphicsPin* pin, QGraphicsItem* parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
	~OBSGraphicsPinInputField();
	int type() const override {return Type;}

private:
	OBSGraphicsPin* pin;
	OBSGraphicsTypeField* typeField;

	std::function<void(bool)> pinConnectionStateChangedCallback;

	std::function<void(QString)> valueChangedCallback = [this](const QString& value) {TypeConverter::FromString(pin->getBlueprintPin(), value.toStdString());};
};
