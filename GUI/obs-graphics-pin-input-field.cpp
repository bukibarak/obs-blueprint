#include "obs-graphics-pin-input-field.h"

#include "obs-graphics-pin.h"
#include "Helpers/pin-helper.h"

OBSGraphicsPinInputField::OBSGraphicsPinInputField(OBSGraphicsPin *pin, QGraphicsItem *parent,
                                                   Qt::WindowFlags wFlags) : QGraphicsProxyWidget(parent, wFlags), pin(pin)
{
	setScale(1.3);
	lineEdit.setText(QString::fromStdString(PinConverter::AsString(pin->getBlueprintPin())));
	lineEdit.setMaximumWidth(80);
	lineEdit.setAlignment(Qt::AlignRight);
	connect(&lineEdit, &QLineEdit::editingFinished, this, &OBSGraphicsPinInputField::lineEditChangedCallback);
	PinType type = pin->getBlueprintPin()->getPinType();
	if(type == BYTE_PIN) lineEdit.setValidator(&byteValidator);
	else if(type == INT_PIN) lineEdit.setValidator(&intValidator);
	else if(type == COLOR_PIN) lineEdit.setValidator(&colorValidator);
	else if(type == CHAR_PIN) lineEdit.setMaxLength(1);
	else if(type == FLOAT_PIN) {
		floatValidator.setLocale(QLocale("en_US"));
		lineEdit.setValidator(&floatValidator);
	}
	setWidget(&lineEdit);

	pinConnectionStateChangedCallback = [this](bool connected) {
		if (connected) {
			hide();

		} else {
			lineEdit.setText(QString::fromStdString(PinConverter::AsString(this->pin->getBlueprintPin())));
			show();
		}
	};

	pin->onConnectionStateChanged = &pinConnectionStateChangedCallback;
}

OBSGraphicsPinInputField::~OBSGraphicsPinInputField()
{
	disconnect(&lineEdit, &QLineEdit::editingFinished, this, &OBSGraphicsPinInputField::lineEditChangedCallback);
	pin->onConnectionStateChanged = nullptr;
}

void OBSGraphicsPinInputField::lineEditChangedCallback()
{
	PinConverter::FromString(pin->getBlueprintPin(), lineEdit.text().toStdString());
}
