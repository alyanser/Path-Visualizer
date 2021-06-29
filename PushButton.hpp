#ifndef PUSHBUTTON_HPP
#define PUSHBUTTON_HPP

#include <QPushButton>
#include <QDebug>

class PushButton : public QPushButton{
         Q_OBJECT
         Q_PROPERTY(QColor bgColor WRITE setColor READ color)
         QColor bgColor;
public:
         PushButton(const QString & text,QWidget * parent) : QPushButton(text,parent){}

         void setColor(const QColor & toCol){
                  bgColor = toCol;
                  setStyleSheet(QString("background-color:%1").arg(bgColor.name()));
         }

         QColor color() const{
                  return bgColor;
         }
};

#endif