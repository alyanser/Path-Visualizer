#ifndef PUSHBUTTON_HPP
#define PUSHBUTTON_HPP

#include <QPushButton>

class PushButton : public QPushButton{
         Q_OBJECT
         Q_PROPERTY(QColor backgroundColor WRITE setColor READ color)
public:
         explicit PushButton(const QString & text,QWidget * parent = nullptr);
         PushButton(const PushButton & other) = delete;
         PushButton(PushButton && other) = delete;
         PushButton & operator = (const PushButton & other) = delete;
         PushButton & operator = (PushButton && other) = delete;
         
         void setColor(const QColor & toCol) noexcept;
         [[nodiscard]] QColor color() const noexcept;
private:
         void addShadowEffect() noexcept;
///
         QColor backgroundColor;
};

#endif