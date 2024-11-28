#pragma once
#include <QWidget>

class OBSBlueprintNode;
struct GUIContext;

class OBSGraphicsNodeDetails : public QWidget {
public:
	OBSGraphicsNodeDetails(GUIContext& context, QWidget* parent = nullptr);
	~OBSGraphicsNodeDetails();

private:

	GUIContext& ctx;
	OBSBlueprintNode* node;
};
