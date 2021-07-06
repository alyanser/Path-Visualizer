#include "pushButton.hpp"
#include <QGraphicsDropShadowEffect>

PushButton::PushButton(const QString & text,QWidget * parent) : QPushButton(text,parent){
         addShadowEffect();
}

void PushButton::addShadowEffect(){
         auto shadowEffect = new QGraphicsDropShadowEffect(this);
         setGraphicsEffect(shadowEffect);
         shadowEffect->setBlurRadius(10);
         shadowEffect->setOffset(1,1);
}

void PushButton::setColor(const QColor & newColor){
         backgroundColor = newColor;
         setStyleSheet(QString("background-color:%1").arg(backgroundColor.name()));
}

QColor PushButton::color() const {
         return backgroundColor;
}
