#pragma once
#include <QGraphicsProxyWidget>
#include <QIntValidator>
#include <QLineEdit>


class OBSGraphicsPin;

class OBSGraphicsPinInputField : public QGraphicsProxyWidget {
public:
	enum { Type = UserType + 4 };
	OBSGraphicsPinInputField(OBSGraphicsPin* pin, QGraphicsItem* parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
	~OBSGraphicsPinInputField();
	int type() const override {return Type;}

private:
	OBSGraphicsPin* pin;
	QLineEdit lineEdit;
	QIntValidator byteValidator{0, 255, &lineEdit};
	QIntValidator intValidator{&lineEdit};
	QDoubleValidator floatValidator{&lineEdit};
	QRegularExpressionValidator colorValidator{QRegularExpression("^(0x|0X)?[a-fA-F0-9]{8}$"), &lineEdit};

	std::function<void(bool)> pinConnectionStateChangedCallback;

	void lineEditChangedCallback();
};
