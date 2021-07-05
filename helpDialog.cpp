#include "helpDialog.hpp"

StackedWidget::StackedWidget(QWidget * parent) : QStackedWidget(parent){
         frontPage = new QWidget(this);
         middlePage = new QWidget(this);
         
         populateFrontPage();
         populateMiddlePage();
         populateLastPage();
}

void StackedWidget::populateFrontPage() const {
         
}

void StackedWidget::populateMiddlePage() const {
         
}

void StackedWidget::populateLastPage() const {
         
}
