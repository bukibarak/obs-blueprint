#include "gui-variables-widget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "Core/obs-blueprint-graph.h"
#include "Core/obs-blueprint-variable.h"

GUIVariablesWidget::GUIVariablesWidget(OBSBlueprintGraph *blueprintGraph,
                                       QWidget *parent) : QWidget(parent), graph(blueprintGraph)
{
	//setStyleSheet("background-color: red;");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);

	QPushButton *addButton = new QPushButton("+New");
	addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	addButton->setStyleSheet("margin: 5px;");

	QFrame* separator = new QFrame(this);
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Sunken);

	QListWidget* variablesList = new QListWidget();
	variablesList->setDragDropMode(QAbstractItemView::InternalMove);
	variablesList->setStyleSheet("margin: 5px;");
	for(auto variable : graph->getVariables()) {
		QListWidgetItem* item = new QListWidgetItem(variable->getDisplayName(), variablesList);
		variablesList->addItem(item);
		variables[item] = variable;
	}

	connect(variablesList, &QListWidget::currentItemChanged, [this](auto current, auto previous){currentSelected = current;});

	layout->addWidget(addButton, 0, Qt::AlignLeft);
	layout->addWidget(separator);
	layout->addWidget(variablesList);
	layout->addStretch();
}

void GUIVariablesWidget::keyPressEvent(QKeyEvent *event)
{
	QWidget::keyPressEvent(event);

	if(event->key() == Qt::Key_F2 && currentSelected != nullptr) {
		qDebug() << variables[currentSelected]->getDisplayName();
		// TODO RENAME DIALOG
	}
}
