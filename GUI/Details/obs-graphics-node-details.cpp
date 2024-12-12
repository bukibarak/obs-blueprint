#include "obs-graphics-node-details.h"

#include <QVBoxLayout>

#include "obs-graphics-pin-details.h"
#include "Core/obs-blueprint-node.h"
#include "GUI/gui-graph.h"

OBSGraphicsNodeDetails::OBSGraphicsNodeDetails(GUIContext &context,
        QWidget *parent) : QWidget(parent), ctx(context), node(ctx.selectedNode)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* selectedLayout = new QHBoxLayout();
	QLabel* selectedLabel = new QLabel("Selected:", this);
	QLabel* selectedName = new QLabel("Node", this);
	selectedName->setStyleSheet("font-style: italic;");
	selectedLayout->addWidget(selectedLabel);
	selectedLayout->addWidget(selectedName);
	layout->addLayout(selectedLayout);

	QHBoxLayout* nameLayout = new QHBoxLayout();
	QLabel* nameLabel = new QLabel("Name:", this);
	QLabel* nameValue = new QLabel(node->getDisplayName(), this);
	nameValue->setStyleSheet("font-weight: bold;");
	nameLayout->addWidget(nameLabel);
	nameLayout->addWidget(nameValue);
	layout->addLayout(nameLayout);

	QLabel* outputLabel = new QLabel("Output pins:", this);
	outputLabel->setStyleSheet("text-decoration: underline;");
	layout->addSpacing(5);
	layout->addWidget(outputLabel);
	for(auto pin : node->getOutputPins()) {
		OBSGraphicsPinDetails* pinDetails = new OBSGraphicsPinDetails(ctx, pin, this);
		layout->addWidget(pinDetails);
	}

	QLabel* inputLabel = new QLabel("Input pins:", this);
	inputLabel->setStyleSheet("text-decoration: underline;");
	layout->addSpacing(5);
	layout->addWidget(inputLabel);
	for(auto pin : node->getInputPins()) {
		OBSGraphicsPinDetails* pinDetails = new OBSGraphicsPinDetails(ctx, pin, this);
		layout->addWidget(pinDetails);
	}

	int labelWidth = qMax(selectedLabel->width(), nameLabel->width());
	selectedLabel->setFixedWidth(labelWidth);
	nameLabel->setFixedWidth(labelWidth);

	layout->addStretch();
}
