#pragma once
#include <QWidget>

class OBSBlueprintVariable;
class QListWidgetItem;
class OBSBlueprintGraph;

class GUIVariablesWidget : public QWidget {
public:
	GUIVariablesWidget(OBSBlueprintGraph* blueprintGraph, QWidget *parent = nullptr);

protected:

	void keyPressEvent(QKeyEvent *event) override;

private:

	QListWidgetItem* currentSelected = nullptr;

	OBSBlueprintGraph* graph;
	QHash<QListWidgetItem*, OBSBlueprintVariable*> variables;
};
