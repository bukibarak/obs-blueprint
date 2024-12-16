#include "gui-details-widget.h"

#include <QLabel>
#include <QVBoxLayout>

#include "obs-graphics-node-details.h"
#include "obs-graphics-variable-details.h"
#include "GUI/obs-graphics-main-window.h"

GUIDetailsWidget::GUIDetailsWidget(GUIContext &context, QWidget *parent) : QWidget(parent), ctx(context)
{
	ctx.detailsWidget = this;
	ctx.onSelectionChanged += onSelectionChanged;


	setStyleSheet("background-color: black;");
	layout = new QVBoxLayout(this);
	layout->setSizeConstraint(QLayout::SizeConstraint::SetMaximumSize);
	setLayout(layout);
	// layout->setSpacing(0);
	// layout->setContentsMargins(0,0,0,0);
	QLabel* label = new QLabel("Details:", this);
	label->setStyleSheet("font-weight:bold; text-decoration: underline; font-size: 24px;");
	layout->addWidget(label);

	layout->setAlignment(Qt::AlignTop);
}

GUIDetailsWidget::~GUIDetailsWidget()
{
	GDebug("[GUI] Deleting QWidget 'GUI Details'...");
	ctx.onSelectionChanged -= onSelectionChanged;
}

void GUIDetailsWidget::hideEvent(QHideEvent *event)
{
	QWidget::hideEvent(event);
	ctx.selectedNode = nullptr;
	ctx.selectedVariable = nullptr;
	selectionChanged();
}



void GUIDetailsWidget::selectionChanged()
{
	if(variableDetails != nullptr) {
		layout->removeWidget(variableDetails);
		delete variableDetails;
		variableDetails = nullptr;
	}
	if(nodeDetails != nullptr) {
		layout->removeWidget(nodeDetails);
		delete nodeDetails;
		nodeDetails = nullptr;
	}

	if(ctx.selectedVariable != nullptr) {
		variableDetails = new OBSGraphicsVariableDetails(ctx, this);
		layout->addWidget(variableDetails);
	}
	else if(ctx.selectedNode != nullptr) {
		nodeDetails = new OBSGraphicsNodeDetails(ctx, this);
		layout->addWidget(nodeDetails);
	}

	update();
}
