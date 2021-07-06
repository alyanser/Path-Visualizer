#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QMovie>
#include "helpDialog.hpp"
#include "PushButton.hpp"

StackedWidget::StackedWidget(QWidget * parent) : QStackedWidget(parent){
         setGeometry(200,200,400,400); // TODO align in mid
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
         bottomLayout->setAlignment(Qt::AlignTop);

         auto label = new QLabel(frontWidget);
         label->setText(R"(Welcome to the visualizer.<br><br><i>You may click on <strong>Next/Prev</strong> buttons 
         to navigate or click on <strong>Skip</strong> button to close the help menu directly.<br><br>This help menu will 
         make you familiar with the features of the visualizer.</i><br><br><br><strong>Information Button</strong> : 
         Displays information about the current algorithm.<br><strong>Run/Stop Button</strong> - Starts or stops current
         algorithm.<br><strong>Reset Button</strong> - Resets the grid to the initial state (Removes all blocks).
         <br><strong>Random Button</strong> - Generats a random grid.<br><strong>Help Button</strong> - Open this 
         help menu.<br><strong>Exit Button</strong> - Exits the visualizer.)");

         layout->addWidget(label,0,0,1,2);
         layout->addLayout(bottomLayout,1,0);

         {
                  auto skipButton = getSkipButton();
                  auto prevButton = getPrevButton();
                  auto nextButton = getNextButton();

                  prevButton->setEnabled(false);

                  bottomLayout->addWidget(skipButton); 
                  bottomLayout->addWidget(prevButton);
                  bottomLayout->addWidget(nextButton);
         }
}

void StackedWidget::populateMiddlePage(){
         auto layout = new QGridLayout(middleWidget); 
         auto bottomLayout = new QHBoxLayout();

         {
                  auto holder = new QLabel(middleWidget);
                  auto player = new QMovie(holder);
                  player->setFileName(":/anims/gifs/smaller.gif");
                  holder->setAlignment(Qt::AlignCenter);
                  holder->setMovie(player);
                  player->start();
                  layout->addWidget(holder,0,0);
         }
         
         {        
                  layout->addLayout(bottomLayout,1,0);
                  
                  auto skipButton = getSkipButton();
                  auto prevButton = getPrevButton();
                  auto nextButton = getNextButton();
                  
                  bottomLayout->addWidget(skipButton); 
                  bottomLayout->addWidget(prevButton);
                  bottomLayout->addWidget(nextButton);
         }
}

void StackedWidget::populateLastPage(){
         auto layout = new QGridLayout(lastWidget); 
         auto bottomLayout = new QHBoxLayout();
         layout->addLayout(bottomLayout,1,0);

         {        
                  auto skipButton = getSkipButton();
                  auto prevButton = getPrevButton();
                  auto finishBut = new PushButton("Finish",lastWidget);
                  finishBut->setToolTip("Close this help menu");

                  connectWithWidgetClose(finishBut);

                  bottomLayout->addWidget(skipButton); 
                  bottomLayout->addWidget(prevButton);
                  bottomLayout->addWidget(finishBut);
         }
}

void StackedWidget::connectWithWidgetClose(PushButton * button){
         connect(button,&QPushButton::clicked,[this]() -> void{
                  setCurrentIndex(0);
                  close();
         });
}

PushButton * StackedWidget::getPrevButton(){
         auto button = new PushButton("Prev");
         button->setToolTip("Go to previous page.");

         connect(button,&PushButton::clicked,[this]{
                  setCurrentIndex(currentIndex() - 1);
         });
         return button;
}

PushButton * StackedWidget::getNextButton(){
         auto button = new PushButton("Next");
         button->setToolTip("Go to next page");

         connect(button,&PushButton::clicked,[this]{
                  setCurrentIndex(currentIndex() + 1);
         });
         
         return button;
}

PushButton * StackedWidget::getSkipButton(){
         auto button = new PushButton("Skip",this);
         button->setToolTip("Close this help menu");
         connectWithWidgetClose(button);
         return button;
}
