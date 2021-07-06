#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QMovie>
#include "helpDialog.hpp"
#include "PushButton.hpp"

StackedWidget::StackedWidget(QWidget * parent) : QStackedWidget(parent){
         setGeometry(200,200,400,400); // TODO align in mid

         addWidget(populateDefinitionPage());
         addWidget(populateBlockGifPage());
         addWidget(populateNodeDragPage());
         addWidget(populateDistancePage());
}

QWidget * StackedWidget::populateDefinitionPage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget); 
         auto label = new QLabel(parentWidget);

         label->setText(R"(Welcome to the visualizer.<br><br><i>You may click on <strong>Next/Prev</strong> buttons 
         to navigate or click on <strong>Skip</strong> button to close the help menu directly.<br><br>This help menu will 
         make you familiar with the features of the visualizer.</i><br><br><br><strong>Information Button</strong> : 
         Displays information about the current algorithm.<br><strong>Run/Stop Button</strong> - Starts or stops current
         algorithm.<br><strong>Reset Button</strong> - Resets the grid to the initial state (Removes all blocks).
         <br><strong>Random Button</strong> - Generats a random grid.<br><strong>Help Button</strong> - Open this 
         help menu.<br><strong>Exit Button</strong> - Exits the visualizer.)");

         mainGridLayout->addWidget(label,0,0);

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Starting);
         mainGridLayout->addLayout(bottomLayout,1,0);

         return parentWidget;
}

QWidget * StackedWidget::populateBlockGifPage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget); 

         {
                  auto holder = new QLabel(parentWidget);
                  auto player = new QMovie(holder);
                  player->setFileName(":/anims/gifs/smaller.gif");
                  holder->setAlignment(Qt::AlignCenter);
                  holder->setMovie(player);
                  player->start();
                  mainGridLayout->addWidget(holder,0,0);
         }

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Middle);
         mainGridLayout->addLayout(bottomLayout,1,0);

         return parentWidget;
}

QWidget * StackedWidget::populateNodeDragPage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget); 

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Middle);
         mainGridLayout->addLayout(bottomLayout,1,0);

         return parentWidget;
}

QWidget * StackedWidget::populateDistancePage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget);
         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Ending);
         mainGridLayout->addLayout(bottomLayout,0,0);

         return parentWidget;
}

QHBoxLayout * StackedWidget::getBottomLayout(QWidget * parentWidget,const PagePosition position){
         auto bottomLayout = new QHBoxLayout(); // parent's main layout deletes

         auto skipButton = getCloseButton(parentWidget);
         auto prevButton = getPrevButton(parentWidget);
         auto nextButton = getNextButton(parentWidget);

         bottomLayout->addWidget(skipButton);
         bottomLayout->addWidget(prevButton);
         bottomLayout->addWidget(nextButton);

         switch(position){
                  case Starting : {
                           prevButton->setEnabled(false);
                           prevButton->setToolTip("No previous page");
                           break;
                  }
                  case Ending : {
                           nextButton->setEnabled(false);
                           nextButton->setToolTip("No next page");
                           break;
                  }
         }
         
         return bottomLayout;
}

PushButton * StackedWidget::getPrevButton(QWidget * parentWidget){
         auto button = new PushButton("Prev",parentWidget);
         button->setToolTip("Go to previous page");

         connect(button,&PushButton::clicked,[this]{
                  setCurrentIndex(currentIndex() - 1);
         });

         return button;
}

PushButton * StackedWidget::getNextButton(QWidget * parentWidget){
         auto button = new PushButton("Next");
         button->setToolTip("Go to next page");

         connect(button,&PushButton::clicked,[this]{
                  setCurrentIndex(currentIndex() + 1);
         });
         
         return button;
}

PushButton * StackedWidget::getCloseButton(QWidget * parentWidget){
         auto button = new PushButton("Close",this);
         button->setToolTip("Close this help menu");
         connectWithWidgetClose(button);
         return button;
}

void StackedWidget::connectWithWidgetClose(PushButton * button){
         connect(button,&QPushButton::clicked,[this]{
                  setCurrentIndex(0);
                  close();
         });
}