#include "helpDialog.hpp"
#include "PushButton.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QTextEdit>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLineEdit>
#include <QLabel>

StackedWidget::StackedWidget(QWidget * parent) : QStackedWidget(parent){
         setGeometry(200,200,400,400); //TODO align in mid
         frontWidget = new QWidget(this); 
         middleWidget = new QWidget(this); 
         lastWidget = new QWidget(this);

         populateFrontPage(); 
         populateMiddlePage();
         populateLastPage();

         addWidget(frontWidget);
         addWidget(middleWidget);
         addWidget(lastWidget);
}

void StackedWidget::populateFrontPage(){
         auto layout = new QGridLayout(frontWidget);
         auto bottomLayout = new QHBoxLayout();

         auto label = new QLabel(frontWidget);
         label->setText(R"(Hello! Click next/prev to navigate through the help menu or click skip to close it.)");
         layout->addWidget(label,0,0,1,2);
         layout->addLayout(bottomLayout,1,0);

         {
                  auto skipButton = new PushButton("Skip",frontWidget);
                  auto prevButton = new PushButton("Prev",frontWidget);
                  auto nextButton = new PushButton("Next",frontWidget);

                  bottomLayout->addWidget(skipButton); 
                  bottomLayout->addWidget(prevButton);
                  bottomLayout->addWidget(nextButton);

                  prevButton->setEnabled(false);

                  connectWithWidgetClose(skipButton);

                  connect(nextButton,&PushButton::clicked,[this]{
                           setCurrentIndex(currentIndex() + 1);
                  });
         }
}

void StackedWidget::populateMiddlePage(){
         auto layout = new QGridLayout(middleWidget); 
         auto bottomLayout = new QHBoxLayout();
         layout->addLayout(bottomLayout,1,0);

         {        
                  auto skipButton = new QPushButton("Skip",middleWidget);
                  auto prevButton = new QPushButton("Prev",middleWidget);
                  auto nextButton = new QPushButton("Next",middleWidget);
                  
                  bottomLayout->addWidget(skipButton); 
                  bottomLayout->addWidget(prevButton);
                  bottomLayout->addWidget(nextButton);

                  connectWithWidgetClose(skipButton);

                  connect(nextButton,&PushButton::clicked,[this]{
                           setCurrentIndex(currentIndex() + 1);
                  });

                  connect(prevButton,&PushButton::clicked,[this]{
                           setCurrentIndex(currentIndex() - 1);
                  });
         }
}

void StackedWidget::populateLastPage(){
         auto layout = new QGridLayout(lastWidget); 
         auto bottomLayout = new QHBoxLayout();
         layout->addLayout(bottomLayout,1,0);

         {        
                  auto skipButton = new QPushButton("Skip",lastWidget);
                  auto prevButton = new QPushButton("Prev",lastWidget);
                  auto finishBut = new QPushButton("Finish",lastWidget);

                  bottomLayout->addWidget(skipButton); 
                  bottomLayout->addWidget(prevButton);
                  bottomLayout->addWidget(finishBut);

                  connectWithWidgetClose(skipButton);

                  connectWithWidgetClose(finishBut);

                  connect(prevButton,&PushButton::clicked,[this]{
                           setCurrentIndex(currentIndex() - 1);
                  });
         }
}
void StackedWidget::connectWithWidgetClose(QPushButton * button){
         connect(button,&QPushButton::clicked,[this]{
                  setCurrentIndex(0);
                  close();
         });
}
