#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QMovie>
#include "helpDialog.hpp"
#include "pushButton.hpp"

StackedWidget::StackedWidget(QWidget * parent) : QStackedWidget(parent){
         // setGeometry(200,200,400,400); // TODO align in mid
         setFixedSize(600,480);

         addWidget(populateDefinitionPage());
         addWidget(populateBlockGifPage());
         addWidget(populateNodeDragPage());
         addWidget(populateTabShiftPage());
         addWidget(populateSpeedPage());
         addWidget(populateDistancePage());
}

QWidget * StackedWidget::populateDefinitionPage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget); 

         auto label = getLabel(parentWidget);

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
                  player->setFileName(":/anims/gifs/place.gif");
                  holder->setAlignment(Qt::AlignCenter);
                  holder->setMovie(player);
                  player->start();
                  mainGridLayout->addWidget(holder,0,0);
         }

         auto label = getLabel(parentWidget);
         mainGridLayout->addWidget(label,mainGridLayout->rowCount(),0);
         
         label->setText(R"(<br>You can click on any icon <small>with the exception of source and target nodes</small>
         and start dragging over other icons to change the state from a <strong>Block Node</strong> to <strong>Path 
         Node</strong> and vice versa. The visualizer will not pass through <strong>Block Nodes</strong>.)");

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Middle);
         mainGridLayout->addLayout(bottomLayout,mainGridLayout->rowCount(),0);

         return parentWidget;
}

QWidget * StackedWidget::populateNodeDragPage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget);

         {
                  auto holder = new QLabel(parentWidget);
                  auto player = new QMovie(holder);
                  player->setFileName(":/anims/gifs/nodeDrag.gif");
                  holder->setAlignment(Qt::AlignCenter);
                  holder->setMovie(player);
                  player->start();
                  mainGridLayout->addWidget(holder,0,0);
         }

         auto label = getLabel(parentWidget);
         mainGridLayout->addWidget(label,mainGridLayout->rowCount(),0);
         
         label->setText(R"(The position of <strong>Source <small>and</small> Target Nodes</strong> can be
         changed by dragging them to other part of the grid. They cannot be placed on top of each other.)");

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Middle);
         mainGridLayout->addLayout(bottomLayout,mainGridLayout->rowCount(),0);

         return parentWidget;
}

QWidget * StackedWidget::populateTabShiftPage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget);

         auto label = getLabel(parentWidget);
         mainGridLayout->addWidget(label,mainGridLayout->rowCount(),0);

         label->setText(R"(Current selected tab determines which algorithm to run. You may switch
         between the tabs to run a different algorithm on the same grid.)");

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Middle);
         mainGridLayout->addLayout(bottomLayout,mainGridLayout->rowCount(),0);

         return parentWidget;
}

QWidget * StackedWidget::populateSpeedPage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget);

         auto label = getLabel(parentWidget);
         mainGridLayout->addWidget(label,mainGridLayout->rowCount(),0);

         label->setText(R"(You can change the speed at which an algorithm operates by the <strong>slider</strong> 
         at bottom.)");

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Middle);
         mainGridLayout->addLayout(bottomLayout,mainGridLayout->rowCount(),0);

         return parentWidget;
}

QWidget * StackedWidget::populateDistancePage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget);

         auto label = getLabel(parentWidget);
         mainGridLayout->addWidget(label,mainGridLayout->rowCount(),0);

         label->setText(R"(The status bar <small>placed at the bottom</small> will display the current distance
         which the active node is at from the source node)");
         
         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Ending);
         mainGridLayout->addLayout(bottomLayout,mainGridLayout->rowCount(),0);

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
                  case Middle : break;
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

QLabel * StackedWidget::getLabel(QWidget * parentWidget) const {
         auto label = new QLabel(parentWidget);
         label->setWordWrap(true);
         return label;
}

PushButton * StackedWidget::getNextButton(QWidget * parentWidget){
         auto button = new PushButton("Next",parentWidget);
         button->setToolTip("Go to next page");

         connect(button,&PushButton::clicked,[this]{
                  setCurrentIndex(currentIndex() + 1);
         });
         
         return button;
}

PushButton * StackedWidget::getCloseButton(QWidget * parentWidget){
         auto button = new PushButton("Close",parentWidget);
         button->setToolTip("Close this help menu");
         connectWithWidgetClose(button);
         return button;
}

void StackedWidget::connectWithWidgetClose(PushButton * button){
         connect(button,&QPushButton::clicked,[this]{
                  qInfo() << "I'm here";
                  setCurrentIndex(0);
                  close();
         });
}