#pragma once
#include <mutex>

class OBSGraphicsMainWindow;
class OBSBlueprintGraph;

class GUIHandler {
public:

	GUIHandler(OBSBlueprintGraph* g);
	~GUIHandler();

	static void OBSEvent(enum obs_frontend_event event, void* ptr);
	void show() const;

private:

	std::condition_variable cond{};

	OBSGraphicsMainWindow* window = nullptr;
	OBSBlueprintGraph* graph;
};
