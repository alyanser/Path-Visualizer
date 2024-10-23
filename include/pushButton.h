#pragma once

#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QEvent>

class PushButton : public QPushButton {
	Q_OBJECT
	Q_PROPERTY(QColor backgroundColor WRITE setColor READ color)
public:
	explicit PushButton(const QString & text, QWidget * parent = nullptr);

	void setColor(const QColor & toCol) noexcept;
	[[nodiscard]]
	QColor color() const noexcept;

private:
	void addShadowEffect() noexcept;
	///
	QColor m_backgroundColor;
};

inline PushButton::PushButton(const QString & text, QWidget * parent) : QPushButton(text, parent) {
	addShadowEffect();
}

inline void PushButton::addShadowEffect() noexcept {
	auto shadowEffect = new QGraphicsDropShadowEffect(this);
	setGraphicsEffect(shadowEffect);
	shadowEffect->setBlurRadius(10);
	shadowEffect->setOffset(1, 1);
}

inline void PushButton::setColor(const QColor & newColor) noexcept {
	m_backgroundColor = newColor;
	setStyleSheet(QString("background-color:%1").arg(m_backgroundColor.name()));
}

inline QColor PushButton::color() const noexcept {
	return m_backgroundColor;
}