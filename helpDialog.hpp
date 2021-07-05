

#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include <QStackedWidget>

class QPushButton;

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
         void connectWithWidgetClose(QPushButton * button);
};

#endif // HELPDIALOG_HPP
