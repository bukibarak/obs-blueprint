#include "gui-variables-widget.h"

#include <QHBoxLayout>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>

#include "obs-graphics-variables-list.h"
#include "Core/obs-blueprint-graph.h"
#include "GUI/obs-graphics-main-window.h"
#include "GUI/obs-graphics-scene.h"
#include "Helpers/obs-blueprint-factory.h"
#include "Helpers/pin-helper.h"

GUIVariablesWidget::GUIVariablesWidget(GUIContext& context,
                                       QWidget *parent) : QWidget(parent), ctx(context)
{
	setFocusPolicy(Qt::ClickFocus);
	ctx.varsWidget = this;

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);

	addButton = new QPushButton("+New");
	addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	addButton->setStyleSheet("margin: 5px;");
	connection = connect(addButton, &QAbstractButton::clicked, this, &GUIVariablesWidget::onButtonClicked);

	QFrame* separator = new QFrame(this);
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Sunken);

	variablesList = new OBSGraphicsVariablesList(ctx, this);
	ctx.varsList = variablesList;
	for(auto variable : ctx.graph->getVariables()) {
		tryCreate(variable);
	}

	layout->addWidget(addButton, 0, Qt::AlignLeft);
	layout->addWidget(separator);
	layout->addWidget(variablesList);
	layout->addStretch();
}

GUIVariablesWidget::~GUIVariablesWidget()
{
	GDebug("[GUI] Deleting QWidget 'GUI Variables'...");
	if(connection)
		disconnect(connection);

	for(auto pairs : renames.asKeyValueRange())
		if(pairs.first)
			pairs.first->onRename -= pairs.second;
}

bool GUIVariablesWidget::tryCreate(OBSBlueprintVariable *variable)
{
	QListWidgetItem* item = new QListWidgetItem(PinColors::GetIcon(variable->getPinType()), variable->getDisplayName(), variablesList);
	if(item != nullptr) {
		variablesList->addItem(item);
		variables[item] = variable;

		renames[variable] = [item](const std::string&, const std::string& curr) {item->setText(curr.c_str());};
		variable->onRename += renames[variable];
		return true;
	}
	return false;
}

bool GUIVariablesWidget::tryRename(QListWidgetItem *item)
{
	if(item == nullptr || !variables.contains(item))
		return false;

	OBSBlueprintVariable* variable = variables[item];
	bool ok;
	QString title{"Rename " + QString(PinName[variable->getPinType()]) + " '" + QString(variable->getDisplayName()) + "'"};
	QString text = QInputDialog::getText(this, title, "Variable name:", QLineEdit::Normal, variable->getDisplayName(), &ok);

	if(ok && !text.isEmpty()) {
		variable->setDisplayName(text.toStdString());
		return true;
	}
	return false;
}

bool GUIVariablesWidget::tryDelete(QListWidgetItem *item)
{
	if(item == nullptr || !variables.contains(item))
		return false;

	OBSBlueprintVariable* variable = variables[item];
	bool ok = true;
	if(ctx.graph->isVariableUsed(variable)) {
		// TODO: maybe debug list of nodes that will be deleted (or at least amount of nodes)
		QString label{"The variable '" + QString(variable->getDisplayName()) + "' (" + QString(PinName[variable->getPinType()]) + ") is still in use.\nDo you want to force delete it? All related nodes will be deleted too."};
		QMessageBox::StandardButton response = QMessageBox::warning(this, "Variable in use!", label, QMessageBox::Yes | QMessageBox::No);
		ok = response == QMessageBox::Yes;
	}
	if(ok) {
		variable->onRename -= renames[variable]; // TODO: Really useful ? variable is deleted in next line
		variablesList->removeItemWidget(item);
		variables.remove(item);
		renames.remove(variable);
		delete item;
		// variablesList->updateContext();
		ctx.graph->deleteVariable(variable);
		return true;
	}
	return false;
}

bool GUIVariablesWidget::tryDuplicate(QListWidgetItem *item)
{
	if(item == nullptr || !variables.contains(item))
		return false;

	OBSBlueprintVariable* original = variables[item];
	OBSBlueprintVariable* copy = OBSBlueprintFactory::CreateVariableFromPin(original->getPinType(), std::string(original->getDisplayName() + std::string(" (copy)")).c_str());
	if(copy != nullptr) {
		ctx.graph->addVariable(copy);
		return tryCreate(copy);
	}
	return false;
}

void GUIVariablesWidget::showContextMenu(const QPoint &pos)
{
	QMenu menu;

	QAction* video = menu.addAction("Video variable");
	QAction* boolean = menu.addAction("Boolean variable");
	QAction* byte = menu.addAction("Byte variable");
	QAction* integer = menu.addAction("Integer variable");
	QAction* floating = menu.addAction("Float variable");
	QAction* character = menu.addAction("Character variable");
	QAction* string = menu.addAction("String variable");
	QAction* color = menu.addAction("Color variable");

	if(QAction* selected = menu.exec(pos)) {
		QString name = QInputDialog::getText(this, "New variable name", "Variable name:", QLineEdit::Normal, "My variable");
		if(name.isEmpty())
			name = "My variable";
		std::string stdName = name.toStdString();
		const char* cname = stdName.c_str();
		OBSBlueprintVariable* created = nullptr;
		if(selected == video)
			created = OBSBlueprintFactory::CreateVideoVariable(cname);
		else if(selected == boolean)
			created = OBSBlueprintFactory::CreateBooleanVariable(cname);
		else if(selected == byte)
			created = OBSBlueprintFactory::CreateByteVariable(cname);
		else if(selected == integer)
			created = OBSBlueprintFactory::CreateIntVariable(cname);
		else if(selected == floating)
			created = OBSBlueprintFactory::CreateFloatVariable(cname);
		else if(selected == character)
			created = OBSBlueprintFactory::CreateCharVariable(cname);
		else if(selected == string)
			created = OBSBlueprintFactory::CreateStringVariable(cname);
		else if(selected == color)
			created = OBSBlueprintFactory::CreateColorVariable(cname);

		if(created != nullptr) {
			ctx.graph->addVariable(created);
			tryCreate(created);
		}
	}
}

OBSBlueprintVariable * GUIVariablesWidget::itemToVariable(QListWidgetItem *item)
{
	if(variables.contains(item))
		return variables[item];
	return nullptr;
}

void GUIVariablesWidget::keyPressEvent(QKeyEvent *event)
{
	QWidget::keyPressEvent(event);

	if(variablesList != nullptr && variablesList->selectedItem() != nullptr) {
		if(event->key() == Qt::Key_F2) {
			tryRename(variablesList->selectedItem());
		}
		else if(event->key() == Qt::Key_Delete) {
			tryDelete(variablesList->selectedItem());
		}
		else if(event->keyCombination() == QKeyCombination(Qt::CTRL | Qt::Key_D)) {
			tryDuplicate(variablesList->selectedItem());
		}
	}
}

void GUIVariablesWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);

	if(variablesList != nullptr) {
		variablesList->resetSelected();
	}
}

void GUIVariablesWidget::focusInEvent(QFocusEvent *event)
{
	QWidget::focusInEvent(event);
	ctx.scene->resetSelected();
}

void GUIVariablesWidget::onButtonClicked(bool checked)
{
	QPoint pos = addButton->pos();
	pos.setX(pos.x() + addButton->size().width() + 5);
	pos.setY(pos.y() + 5);
	showContextMenu(mapToGlobal(pos));
}
