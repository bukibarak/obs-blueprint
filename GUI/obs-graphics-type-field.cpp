#include "obs-graphics-type-field.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>
#include <QLabel>

#include "Core/pin-type.h"
#include "Helpers/global-logger.h"

OBSGraphicsTypeField::OBSGraphicsTypeField(const PinType &pinType,
                                           QWidget *parent, const QString& value) : QWidget(parent), type(pinType)
{
	switch (pinType) {
	case VIDEO_PIN:
		labelField = new QLabel(this);
		break;
	case EXECUTION_PIN:
		buttonField = new QPushButton("Execute", this);
		connect(buttonField, &QPushButton::clicked, [this](const bool&) {onValueChanged.execute("EXECUTE");});
		break;
	case BOOLEAN_PIN:
		checkField = new QCheckBox(this);
		connect(checkField, &QCheckBox::checkStateChanged, [this](const Qt::CheckState& state) {onValueChanged.execute(state == Qt::Checked ? "1" : "0");});
		break;
	case BYTE_PIN:
		lineField = new QLineEdit(this);
		validatorField = new QIntValidator(0, 255, lineField);
		lineField->setValidator(validatorField);
		break;
	case INT_PIN:
		lineField = new QLineEdit(this);
		validatorField = new QIntValidator(lineField);
		lineField->setValidator(validatorField);
		break;
	case FLOAT_PIN:
		lineField = new QLineEdit(this);
		validatorField = new QDoubleValidator(lineField);
		validatorField->setLocale(QLocale("en_US"));
		lineField->setValidator(validatorField);
		break;
	case COLOR_PIN:
		lineField = new QLineEdit(this);
		validatorField = new QRegularExpressionValidator(QRegularExpression("^(0x|0X)?[a-fA-F0-9]{8}$"), lineField);
		lineField->setValidator(validatorField);
		break;
	case CHAR_PIN:
		lineField = new QLineEdit(this);
		lineField->setMaxLength(1);
		break;
	case STRING_PIN:
		lineField = new QLineEdit(this);
		break;
	default:
		labelField = new QLabel("???", this);
	}


	if(lineField != nullptr) {
		connect(lineField, &QLineEdit::editingFinished, [this] {onValueChanged.execute(lineField->text());});
		setFixedHeight(lineField->height() + 5);
	}

	if(!value.isEmpty())
		setValue(value);
}

void OBSGraphicsTypeField::setValue(const QString &value)
{
	switch (type) {
	case VIDEO_PIN:
		labelField->setText(value);
		break;
	case EXECUTION_PIN:
		GWarn("Cannot set field value of type EXECUTE_PIN, will be ignored...");
		break;
	case BOOLEAN_PIN:
		checkField->setChecked(value != "0");
		break;
	case BYTE_PIN:
	case INT_PIN:
	case FLOAT_PIN:
	case CHAR_PIN:
	case STRING_PIN:
	case COLOR_PIN:
		lineField->setText(value);
		break;
	default:
		labelField->setText("??? (" + value + ")");
	}

	onValueChanged.execute(value);
}

const QString & OBSGraphicsTypeField::getValue()
{
	switch (type) {
	case BOOLEAN_PIN:
		strValue = QString(checkField->isChecked() ? "1" : "0");
		break;
	case BYTE_PIN:
	case INT_PIN:
	case FLOAT_PIN:
	case CHAR_PIN:
	case STRING_PIN:
	case COLOR_PIN:
		strValue = lineField->text();
		break;
	default:
		GWarn("Cannot get value of type %s as string, will return empty string...", PinName[type]);
	}

	return strValue;
}
