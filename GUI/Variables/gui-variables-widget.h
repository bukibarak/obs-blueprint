#pragma once
#include <QWidget>

struct GUIContext;
class QPushButton;
class OBSGraphicsVariablesList;
class OBSBlueprintVariable;
class QListWidgetItem;
class OBSBlueprintGraph;

class GUIVariablesWidget : public QWidget {
public:
	GUIVariablesWidget(GUIContext& context, QWidget *parent = nullptr);
	~GUIVariablesWidget();

	bool tryCreate(OBSBlueprintVariable* variable);
	bool tryRename(QListWidgetItem* item);
	bool tryDelete(QListWidgetItem* item);
	bool tryDuplicate(QListWidgetItem* item);

	void showContextMenu(const QPoint& pos);

	OBSBlueprintVariable* itemToVariable(QListWidgetItem* item);

protected:

	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;

private:

	void onButtonClicked(bool checked);
	QMetaObject::Connection connection;

	OBSGraphicsVariablesList* variablesList = nullptr;
	QPushButton* addButton = nullptr;

	GUIContext& ctx;
	QHash<QListWidgetItem*, OBSBlueprintVariable*> variables;
	QHash<OBSBlueprintVariable*, std::function<void(std::string, std::string)>> renames;

	bool graphAlreadyDeleted = false;
	std::function<void()> onGraphDelete = [this]{graphAlreadyDeleted = true;};
};
