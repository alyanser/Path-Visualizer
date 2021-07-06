#ifndef PUSHBUTTON_HPP
#define PUSHBUTTON_HPP

#include <QPushButton>

class PushButton : public QPushButton{
         Q_OBJECT
         Q_PROPERTY(QColor backgroundColor WRITE setColor READ color)
         QColor backgroundColor;
public:
         PushButton(const QString & text,QWidget * parent = nullptr);
         
         void setColor(const QColor & toCol);
         QColor color() const;
};

#endif