#ifndef PUSHBUTTON_HPP
#define PUSHBUTTON_HPP

#include <QPushButton>

class PushButton : public QPushButton {
         Q_OBJECT
         Q_PROPERTY(QColor backgroundColor WRITE setColor READ color)
public:
         explicit PushButton(const QString & text,QWidget * parent = nullptr);
         
         void setColor(const QColor & toCol) noexcept;
         [[nodiscard]] QColor color() const noexcept;
private:
         void addShadowEffect() noexcept;
///
         QColor m_backgroundColor;
};

#endif