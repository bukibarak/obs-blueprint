#include "obs-graphics-type-field.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>
#include <QLabel>
#include <QVBoxLayout>

#include "Helpers/global-logger.h"

OBSGraphicsTypeField::OBSGraphicsTypeField(const PinType &pinType,
	QWidget *parent, const QString& value, bool withLayout) : QWidget(parent), type(pinType)
{
	switch (pinType) {
	case VIDEO_PIN:
		labelField = new QLabel(this);
		break;
	case EXECUTION_PIN:
		buttonField = new QPushButton("Execute", this);
		connection = connect(buttonField, &QPushButton::clicked, [this](const bool&) {onValueChanged.execute("EXECUTE");});
		break;
	case BOOLEAN_PIN:
		checkField = new QCheckBox(this);
		connection = connect(checkField, &QCheckBox::stateChanged, [this](const int& state) {onValueChanged.execute(state == Qt::Checked ? "1" : "0");});
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

	if(withLayout) {
		QLayout* layout = new QHBoxLayout(this);
		layout->setContentsMargins(0,0,0,0);
		if(labelField != nullptr) {
			layout->addWidget(labelField);
		}
		else if(buttonField != nullptr) {
			layout->addWidget(buttonField);
		}
		else if(checkField != nullptr) {
			layout->addWidget(checkField);
		}
		else if(lineField != nullptr) {
			layout->addWidget(lineField);
		}
		else if(listField != nullptr) {
			layout->addWidget(listField);
		}
	}

	if(lineField != nullptr) {
		lineField->setMinimumWidth(100);
		connection = connect(lineField, &QLineEdit::editingFinished, [this] {
			onValueChanged.execute(lineField->text());
		});
	}

	if(!value.isEmpty())
		setValue(value);
}

OBSGraphicsTypeField::~OBSGraphicsTypeField()
{
	disconnect(connection);
}

void OBSGraphicsTypeField::setValue(const QString &value)
{
	switch (type) {
	case VIDEO_PIN:
		if(labelField)
			labelField->setText(value);
		break;
	case EXECUTION_PIN:
		GWarn("Cannot set field value of type EXECUTE_PIN, will be ignored...");
		break;
	case BOOLEAN_PIN:
		if(checkField)
			checkField->setChecked(value != "0");
		break;
	case BYTE_PIN:
	case INT_PIN:
	case FLOAT_PIN:
	case CHAR_PIN:
	case STRING_PIN:
	case COLOR_PIN:
		if(lineField)
			lineField->setText(value);
		break;
	default:
		if(labelField)
			labelField->setText("??? (" + value + ")");
	}

	// onValueChanged.execute(value);
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
