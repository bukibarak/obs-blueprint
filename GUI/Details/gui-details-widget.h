#pragma once
#include <QWidget>

class OBSGraphicsNodeDetails;
class OBSGraphicsVariableDetails;
class QVBoxLayout;
struct GUIContext;

class GUIDetailsWidget : public QWidget {
public:
	GUIDetailsWidget(GUIContext& context, QWidget *parent = nullptr);
	~GUIDetailsWidget() override;

protected:
	void hideEvent(QHideEvent *event) override;


private:
	QVBoxLayout* layout;
	OBSGraphicsVariableDetails* variableDetails = nullptr;
	OBSGraphicsNodeDetails* nodeDetails = nullptr;
	void selectionChanged();
	std::function<void()> onSelectionChanged = [this]{selectionChanged();};
	GUIContext& ctx;
};
