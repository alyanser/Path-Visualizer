#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QEvent>
#include "pushButton.hpp"

PushButton::PushButton(const QString & text,QWidget * parent) : QPushButton(text,parent){
         addShadowEffect();
}

void PushButton::addShadowEffect() noexcept {
         auto shadowEffect = new QGraphicsDropShadowEffect(this);
         setGraphicsEffect(shadowEffect);
         shadowEffect->setBlurRadius(10);
         shadowEffect->setOffset(1,1);
}

void PushButton::setColor(const QColor & newColor) noexcept {
         m_backgroundColor = newColor;
         setStyleSheet(QString("background-color:%1").arg(m_backgroundColor.name()));
}

QColor PushButton::color() const noexcept {
         return m_backgroundColor;
}