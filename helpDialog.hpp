

#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include <QStackedWidget>

class PushButton;
class QHBoxLayout;

class StackedWidget : public QStackedWidget{
         Q_OBJECT
         enum PagePosition {Starting,Middle,Ending};
public:
         StackedWidget(QWidget * parent = nullptr);
         ~StackedWidget() = default;
private:
         QWidget * populateDefinitionPage();
         QWidget * populateBlockGifPage();
         QWidget * populateNodeDragPage();
         QWidget * populateDistancePage();

         QHBoxLayout * getBottomLayout(QWidget * parentWidget,const PagePosition position);
         PushButton * getNextButton(QWidget * parentWidget);
         PushButton * getCloseButton(QWidget * parentWidget);
         PushButton * getPrevButton(QWidget * parentWidget);

         void connectWithWidgetClose(PushButton * button);
};

#endif // HELPDIALOG_HPP
