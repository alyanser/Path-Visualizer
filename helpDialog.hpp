

#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include <QStackedWidget>

class PushButton;
class QHBoxLayout;
class QLabel;

class StackedWidget : public QStackedWidget{
         Q_OBJECT
         enum PagePosition {Starting,Middle,Ending};
public:
         explicit StackedWidget(const QSize windowSize,QWidget * parent = nullptr);
         ~StackedWidget() = default;
         StackedWidget(const StackedWidget & other) = delete;
         StackedWidget(StackedWidget && other) = delete;
         StackedWidget & operator = (const StackedWidget & other) = delete;
         StackedWidget & operator = (StackedWidget && other) = delete;
private:
         constexpr inline static size_t width = 800;
         constexpr inline static size_t height = 400;

         QWidget * populateDefinitionPage();
         QWidget * populateBlockGifPage();
         QWidget * populateNodeDragPage();
         QWidget * populateDistancePage();
         QWidget * populateTabShiftPage();
         QWidget * populateSpeedPage();
         QWidget * populateUpdateTab();

         QHBoxLayout * getBottomLayout(QWidget * parentWidget,const PagePosition position);
         PushButton * getNextButton(QWidget * parentWidget);
         PushButton * getCloseButton(QWidget * parentWidget);
         PushButton * getPrevButton(QWidget * parentWidget);
         QLabel * getLabel(QWidget * parentWidget) const;

         void configureGeometry(const QSize windowSize);
         void connectWithWidgetClose(PushButton * button);
};

#endif 
