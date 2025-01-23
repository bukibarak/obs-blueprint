#pragma once

#include <QComboBox>
#include <QWidget>

#include "Core/pin-type.h"
#include "Structs/graphics-options.h"
#include "Structs/multicast-delegate.h"

class QValidator;
class QLabel;
class QPushButton;
class QComboBox;
class QCheckBox;
class QLineEdit;

struct FieldFormat {
	const PinType& pinType;
	PinGraphicsOptions* options = nullptr;

	FieldFormat(const PinType& type) : pinType(type) {}
	FieldFormat(const PinType& type, PinGraphicsOptions& options) : pinType(type), options(&options) {}
};

class OBSGraphicsTypeField : public QWidget {
public:
	OBSGraphicsTypeField(const FieldFormat& format, QWidget* parent = nullptr, const QString& value = "", bool withLayout = true);
	~OBSGraphicsTypeField() override;

	void setValue(const QString& value);
	QString getValue() const;

	multicastDelegate_OneParam<QString> onValueChanged;

private:
	QMetaObject::Connection connection;
	FieldFormat fieldFormat;
	QLineEdit* lineField = nullptr;
	QCheckBox* checkField = nullptr;
	QComboBox* listField = nullptr;
	QPushButton* buttonField = nullptr;
	QLabel* labelField = nullptr;
	QSignalBlocker* preventQtSignals = nullptr;

	QValidator* validatorField = nullptr;

	std::function<void()> comboBoxOptionsChangedCallback = [this] {
		listField->clear();
		if (fieldFormat.options->comboBoxOptions.empty()) {
			listField->addItem("");
		}
		else {
			for (const auto& option : fieldFormat.options->comboBoxOptions) {
				listField->addItem(option.first.c_str());
			}
		}
	};
};
