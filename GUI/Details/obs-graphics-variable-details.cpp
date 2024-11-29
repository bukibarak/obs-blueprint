#include "obs-graphics-variable-details.h"

#include <QLabel>
#include <QVBoxLayout>

#include "GUI/gui-graph.h"
#include "GUI/obs-graphics-type-field.h"
#include "GUI/obs-graphics-view.h"

OBSGraphicsVariableDetails::OBSGraphicsVariableDetails(GUIContext& context,
	QWidget *parent) : QWidget(parent), ctx(context), var(context.selectedVariable)
{
	ctx.onDeletion += onGraphDelete;

	QVBoxLayout *layout = new QVBoxLayout(this);

	QHBoxLayout* selectedLayout = new QHBoxLayout();
	QLabel* selectedLabel = new QLabel("Selected:", this);
	QLabel* selectedName = new QLabel("Variable", this);
	selectedName->setStyleSheet("font-style: italic;");
	selectedLayout->addWidget(selectedLabel);
	selectedLayout->addWidget(selectedName);
	layout->addLayout(selectedLayout);

	QHBoxLayout *typeLayout = new QHBoxLayout();
	typeLayout->setAlignment(Qt::AlignLeft);
	QLabel* typeLabel = new QLabel("Type:", this);
	QLabel* typeIcon = new QLabel( this);
	QLabel* typeName = new QLabel(QString(PinName[var->getPinType()]).split('_').at(0), this);
	typeIcon->setPixmap(PinColors::GetPixmap(var->getPinType()));
	typeLayout->addWidget(typeLabel);
	typeLayout->addSpacing(3);
	typeLayout->addWidget(typeIcon);
	typeLayout->addWidget(typeName);
	layout->addLayout(typeLayout);

	QHBoxLayout* nameLayout = new QHBoxLayout();
	QLabel* nameLabel = new QLabel("Name:", this);
	nameLineEdit = new QLineEdit(var->getDisplayName(), this);
	connection = connect(nameLineEdit, &QLineEdit::editingFinished, this, &OBSGraphicsVariableDetails::nameLineEditFinished);
	var->onRename += onNameChanged;
	nameLayout->addWidget(nameLabel);
	nameLayout->addWidget(nameLineEdit);
	layout->addLayout(nameLayout);

	layout->addSpacing(5);

	QHBoxLayout* valueLayout = new QHBoxLayout();
	valueLayout->setSizeConstraint(QLayout::SetNoConstraint);
	QLabel* valueLabel = new QLabel("Value:", this);
	valueEdit = new OBSGraphicsTypeField(var->getPinType(), this, TypeConverter::AsString(var).c_str());
	valueEdit->onValueChanged += onValueChanged;
	// valueEdit->setFixedWidth(valueEdit->width());
	// valueEdit->setFixedHeight(valueEdit->height());
	//valueEdit->setStyleSheet("border: 1px solid red;");
	valueLayout->addWidget(valueLabel);
	valueLayout->addWidget(valueEdit);
	valueLayout->addStretch();
	layout->addLayout(valueLayout);


	int labelWidth = qMax(selectedLabel->width(), typeLabel->width());
	labelWidth = qMax(labelWidth, nameLabel->width());
	labelWidth = qMax(labelWidth, valueLabel->width());
	selectedLabel->setFixedWidth(labelWidth);
	typeLabel->setFixedWidth(labelWidth);
	nameLabel->setFixedWidth(labelWidth);
	valueLabel->setFixedWidth(labelWidth);

	valueLabel->setStyleSheet("border: 1px solid green;");
	valueEdit->setStyleSheet("border: 1px solid red;");
}

OBSGraphicsVariableDetails::~OBSGraphicsVariableDetails()
{
	disconnect(connection);
	valueEdit->onValueChanged -= onValueChanged;

	if(!graphAlreadyDeleted) {
		ctx.onDeletion -= onGraphDelete;
		var->onRename -= onNameChanged;
	}
}

void OBSGraphicsVariableDetails::nameLineEditFinished() const
{
	const QString& text = nameLineEdit->text();
	if(text.isEmpty()) {
		nameLineEdit->setText(var->getDisplayName());
	}
	else {
		var->setDisplayName(text.toStdString());
		ctx.view->update();
	}
}
