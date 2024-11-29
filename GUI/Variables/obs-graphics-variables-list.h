#pragma once
#include <QListWidget>

struct GUIContext;
class GUIVariablesWidget;

class OBSGraphicsVariablesList : public QListWidget {
public:

	OBSGraphicsVariablesList(GUIContext& context, GUIVariablesWidget* parent);
	~OBSGraphicsVariablesList() override;

	QListWidgetItem* selectedItem() const;
	void resetSelected() const;

protected:

	void mouseReleaseEvent(QMouseEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;

private:

	void updateContext() const;

	QMetaObject::Connection connection;

	GUIContext& ctx;
	GUIVariablesWidget* parentWidget;
};
