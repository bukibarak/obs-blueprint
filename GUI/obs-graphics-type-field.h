#pragma once

#include <QWidget>

#include "Core/pin-type.h"
#include "Structs/multicast-delegate.h"

class QValidator;
class QLabel;
class QPushButton;
class QComboBox;
class QCheckBox;
class QLineEdit;

class OBSGraphicsTypeField : public QWidget {
public:
	OBSGraphicsTypeField(const PinType& pinType, QWidget* parent = nullptr, const QString& value = "");

	void setValue(const QString& value);
	const QString& getValue();

	multicastDelegate_OneParam<QString> onValueChanged;

private:
	PinType type;
	QLineEdit* lineField = nullptr;
	QCheckBox* checkField = nullptr;
	QComboBox* listField = nullptr;
	QPushButton* buttonField = nullptr;
	QLabel* labelField = nullptr;

	QString strValue{};

	QValidator* validatorField = nullptr;
};
