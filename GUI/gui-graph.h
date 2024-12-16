#pragma once
#include <functional>
#include <mutex>

class OBSGraphicsMainWindow;
class OBSBlueprintGraph;

class GUIHandler {
public:

	GUIHandler(OBSBlueprintGraph* g);
	~GUIHandler();

	void show() const;

private:

	std::condition_variable cond{};

	OBSGraphicsMainWindow* window = nullptr;
	OBSBlueprintGraph* graph;

	std::function<void()> mainWindowDeleted = [this]{window = nullptr;};
};
