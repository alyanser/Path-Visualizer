#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QMovie>
#include "helpDialog.hpp"
#include "pushButton.hpp"

StackedWidget::StackedWidget(const QSize windowSize,QWidget * parent) : QStackedWidget(parent){
         setWindowTitle("Help Dialog - Visualizer");
         configureGeometry(windowSize);

         addWidget(populateDefinitionPage());
         addWidget(populateUpdateTab());
         addWidget(populateBlockGifPage());
         addWidget(populateNodeDragPage());
         addWidget(populateTabShiftPage());
         addWidget(populateSpeedPage());
         addWidget(populateDistancePage());
}

void StackedWidget::configureGeometry(const QSize windowSize){
         const int xCord = windowSize.width() / 2;
         const int yCord = windowSize.height() / 2;
         setGeometry(xCord - static_cast<int>(width) / 2,yCord - static_cast<int>(height) / 2,width,height);
         setFixedSize(width,height);
}

QWidget * StackedWidget::populateDefinitionPage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget); 

         auto topLabel = getLabel(parentWidget);
         auto bottomLabel = getLabel(parentWidget);

         topLabel->setText(R"(<strong>Welcome to the visualizer</srong>.<br><br><i>You may click on <strong>Next
         /Prev</strong> button to navigate or click on <strong>Skip</strong> button to close the help menu 
         directly.</i>)");

         bottomLabel->setText(R"(<strong>Information</strong> : Displays information about the current 
         algorithm<br><strong>Run/Stop/Continue</strong> - Starts, Continues or stops current algorithm<br><strong>
         Reset</strong> - Resets the grid to the initial state (also remvoes all placed blocks) <br><strong>
         Random</strong> - Generates a random grid.<br><strong>Help</strong> - Opens this help menu.<br><strong>
         Exit</strong> - Exits the visualizer.)");

         bottomLabel->setAlignment(Qt::AlignLeft);

         mainGridLayout->addWidget(topLabel,mainGridLayout->rowCount(),0);
         mainGridLayout->addWidget(bottomLabel,mainGridLayout->rowCount(),0);

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Starting);
         mainGridLayout->addLayout(bottomLayout,mainGridLayout->rowCount(),0);

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
         Node</strong> and vice versa. Active nodes will not pass through <strong>Block Nodes</strong>.)");

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

         {
                  auto tabShiftLabel = getLabel(parentWidget);
                  QPixmap pix(":/pixmaps/icons/tabs.png");
                  tabShiftLabel->setPixmap(pix);
                  assert(!tabShiftLabel->pixmap().isNull());
                  mainGridLayout->addWidget(tabShiftLabel,mainGridLayout->rowCount(),0);
         }

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

         {
                  auto speedLabel = getLabel(parentWidget);
                  QPixmap pix(":/pixmaps/icons/speed.png");
                  speedLabel->setPixmap(pix);
                  assert(!speedLabel->pixmap().isNull());
                  mainGridLayout->addWidget(speedLabel,mainGridLayout->rowCount(),0);
         }

         auto label = getLabel(parentWidget);
         mainGridLayout->addWidget(label,mainGridLayout->rowCount(),0);

         label->setText(R"(You can change the speed at which an algorithm operates by the <strong>slider</strong> 
         at bottom.)");

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Middle);
         mainGridLayout->addLayout(bottomLayout,mainGridLayout->rowCount(),0);

         return parentWidget;
}

QWidget * StackedWidget::populateUpdateTab(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget);

         auto label = getLabel(parentWidget);
         mainGridLayout->addWidget(label,mainGridLayout->rowCount(),0);

         label->setText(R"(<strong>Following are the planned updates/additions:-</strong><br><br> 
         - Resizable window<br> - Visual connections between nodes<br> - Weighted nodes (Currently BFS and 
         Dijkstra are no different)<br> - Addition of more SP algorithms <small>after addition of weighted nodes</small> 
         (BellmanFord, Floyd Warshall,..))");

         auto bottomLayout = getBottomLayout(parentWidget,PagePosition::Middle);
         mainGridLayout->addLayout(bottomLayout,mainGridLayout->rowCount(),0);

         return parentWidget;
}

QWidget * StackedWidget::populateDistancePage(){
         auto parentWidget = new QWidget(this);
         auto mainGridLayout = new QGridLayout(parentWidget);

         {
                  auto distanceLabel = getLabel(parentWidget);
                  QPixmap pix(":/pixmaps/icons/distance.png");
                  distanceLabel->setPixmap(pix);
                  assert(!distanceLabel->pixmap().isNull());
                  mainGridLayout->addWidget(distanceLabel,mainGridLayout->rowCount(),0);
         }

         auto label = getLabel(parentWidget);
         mainGridLayout->addWidget(label,mainGridLayout->rowCount(),0);

         label->setText(R"(The status bar <small>placed at the bottom</small> will display the current distance
         which the active node is at from the source node. <br><br>Voila! That's all for now.)");
         
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
         label->setAlignment(Qt::AlignCenter);
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
                  setCurrentIndex(0);
                  close();
         });
}