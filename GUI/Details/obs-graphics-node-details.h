#pragma once
#include <QWidget>

class OBSBlueprintNode;
struct GUIContext;

class OBSGraphicsNodeDetails : public QWidget {
public:
	OBSGraphicsNodeDetails(GUIContext& context, QWidget* parent = nullptr);
	~OBSGraphicsNodeDetails() override;

private:

	GUIContext& ctx;
	OBSBlueprintNode* node;
};
