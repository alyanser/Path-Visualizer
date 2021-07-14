#ifndef PUSHBUTTON_HPP
#define PUSHBUTTON_HPP

#include <QPushButton>

class PushButton : public QPushButton{
         Q_OBJECT
         Q_PROPERTY(QColor backgroundColor WRITE setColor READ color)
         QColor backgroundColor;
public:
         explicit PushButton(const QString & text,QWidget * parent = nullptr);
         ~PushButton() = default;
         PushButton(const PushButton & other) = delete;
         PushButton(PushButton && other) = delete;
         PushButton & operator = (const PushButton & other) = delete;
         PushButton & operator = (PushButton && other) = delete;
         
         void setColor(const QColor & toCol);
         QColor color() const;
private:
         inline static int counter;
         void addShadowEffect();
};

#endif