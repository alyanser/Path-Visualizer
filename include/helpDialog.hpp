

#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include <QStackedWidget>

class PushButton;
class QHBoxLayout;
class QLabel;

class StackedWidget : public QStackedWidget{
         Q_OBJECT
         enum class PagePosition { Starting, Middle, Ending };
         enum dimensions : size_t { Width = 800, Height = 400 };
public:
         explicit StackedWidget(QSize windowSize,QWidget * parent = nullptr);
         StackedWidget(const StackedWidget & other) = delete;
         StackedWidget(StackedWidget && other) = delete;
         StackedWidget & operator = (const StackedWidget & other) = delete;
         StackedWidget & operator = (StackedWidget && other) = delete;
private:

         [[nodiscard]] QWidget * populateDefinitionPage() noexcept;
         [[nodiscard]] QWidget * populateBlockGifPage() noexcept;
         [[nodiscard]] QWidget * populateNodeDragPage() noexcept;
         [[nodiscard]] QWidget * populateDistancePage() noexcept;
         [[nodiscard]] QWidget * populateTabShiftPage() noexcept;
         [[nodiscard]] QWidget * populateSpeedPage() noexcept;
         [[nodiscard]] QWidget * populateUpdateTab() noexcept;

         [[nodiscard]] QHBoxLayout * getBottomLayout(QWidget * parentWidget,PagePosition position) noexcept;
         [[nodiscard]] PushButton * getNextButton(QWidget * parentWidget) noexcept;
         [[nodiscard]] PushButton * getCloseButton(QWidget * parentWidget) noexcept;
         [[nodiscard]] PushButton * getPrevButton(QWidget * parentWidget) noexcept;
         [[nodiscard]] QLabel * getLabel(QWidget * parentWidget) const noexcept;

         void configureGeometry(QSize windowSize) noexcept;
         void connectWithWidgetClose(PushButton * button) noexcept;
};

#endif 
