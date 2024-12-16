#pragma once
#include <QLineEdit>
#include <QWidget>

#include "Helpers/pin-helper.h"

class OBSGraphicsTypeField;
struct GUIContext;
class OBSBlueprintVariable;

class OBSGraphicsVariableDetails : public QWidget {
public:
	OBSGraphicsVariableDetails(GUIContext& context, QWidget* parent = nullptr);
	~OBSGraphicsVariableDetails() override;

private:

	void nameLineEditFinished() const;

	GUIContext& ctx;
	OBSBlueprintVariable* var;
	QLineEdit* nameLineEdit;
	OBSGraphicsTypeField* valueEdit;

	std::function<void(std::string, std::string)> onNameChanged = [this](const std::string&, const std::string& curr) {nameLineEdit->setText(curr.c_str());};
	std::function<void(QString)> onValueChanged = [this](const QString& value) {TypeConverter::FromString(var, value.toStdString());};
	QMetaObject::Connection connection;
};
