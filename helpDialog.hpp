

#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include <QStackedWidget>

class PushButton;

class StackedWidget : public QStackedWidget{
         Q_OBJECT
public:
         StackedWidget(QWidget * parent = nullptr);
         ~StackedWidget() = default;
private:
         QWidget * frontWidget;
         QWidget * middleWidget;
         QWidget * lastWidget;
         
         void populateFrontPage();
         void populateMiddlePage();
         void populateLastPage();
         void connectWithWidgetClose(PushButton * button);
         PushButton * getNextButton();
         PushButton * getSkipButton();
         PushButton * getPrevButton();
};

#endif // HELPDIALOG_HPP
