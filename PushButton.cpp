#include "PushButton.hpp"

PushButton::PushButton(const QString & text,QWidget * parent) : QPushButton(text,parent){
         
}

void PushButton::setColor(const QColor & newColor){
         backgroundColor = newColor;
         setStyleSheet(QString("background-color:%1").arg(backgroundColor.name()));
}

QColor PushButton::color() const {
         return backgroundColor;
}
