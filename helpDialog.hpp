

#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include <QStackedWidget>

class StackedWidget : public QStackedWidget{
         Q_OBJECT
public:
         StackedWidget(QWidget * parent = nullptr);
         ~StackedWidget() = default;
private:
         QWidget * frontPage;
         QWidget * middlePage;
         QWidget * lastPage;
         
         void populateFrontPage() const;
         void populateMiddlePage() const;
         void populateLastPage() const;
};

#endif // HELPDIALOG_HPP
