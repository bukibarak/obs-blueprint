#include "obs-graphics-variables-list.h"

#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>

#include "gui-variables-widget.h"
#include "GUI/gui-graph.h"
#include "GUI/obs-graphics-scene.h"

OBSGraphicsVariablesList::OBSGraphicsVariablesList(GUIContext& context, GUIVariablesWidget *parent)
	: QListWidget(parent),
	  ctx(context),
	  parentWidget(parent)
{
	setDragDropMode(InternalMove);
	setStyleSheet("margin: 5px;");
	connection = connect(this, &QListWidget::itemSelectionChanged, this, &OBSGraphicsVariablesList::updateContext);
}

OBSGraphicsVariablesList::~OBSGraphicsVariablesList()
{
	disconnect(connection);
}

QListWidgetItem * OBSGraphicsVariablesList::selectedItem() const
{
	if(selectedItems().isEmpty()) return nullptr;
	return selectedItems().first();
}

void OBSGraphicsVariablesList::updateContext() const
{
	QListWidgetItem *selected = selectedItem();
	OBSBlueprintVariable* selectedVar = parentWidget->itemToVariable(selected);
	if(ctx.selectedVariable != selectedVar) {
		ctx.selectedVariable = selectedVar;
		ctx.onSelectionChanged.execute();
	}
}

void OBSGraphicsVariablesList::resetSelected() const
{
	selectionModel()->clear();
}

void OBSGraphicsVariablesList::mouseReleaseEvent(QMouseEvent *event)
{
	QListWidget::mouseReleaseEvent(event);

	QListWidgetItem *selected = selectedItem();

	if(event->button() == Qt::RightButton) {

		if(selected != nullptr) {
			QMenu menu;

			QAction duplicate{"Duplicate", this};
			duplicate.setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

			QAction rename{"Rename", this};
			rename.setShortcut(QKeySequence(Qt::Key_F2));

			QAction del{"Delete", this};
			del.setShortcut(QKeySequence(Qt::Key_Delete));
			menu.addAction(&duplicate);
			menu.addAction(&rename);
			menu.addAction(&del);

			QAction* action = menu.exec(event->globalPosition().toPoint());

			if(action == &duplicate) {
				parentWidget->tryDuplicate(selected);
			}
			else if(action == &rename) {
				parentWidget->tryRename(selected);
			}
			else if(action == &del) {
				parentWidget->tryDelete(selected);
			}
		}
		else {
			parentWidget->showContextMenu(event->globalPosition().toPoint());
		}
	}
}

void OBSGraphicsVariablesList::focusInEvent(QFocusEvent *event)
{
	QListWidget::focusInEvent(event);
	ctx.scene->resetSelected();
}
