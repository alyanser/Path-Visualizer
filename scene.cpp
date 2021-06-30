#include <QTabWidget>
#include <tuple>
#include <queue>
#include <QToolButton>
#include <QGraphicsLineItem>
#include <QGraphicsLayoutItem>
#include <QEventTransition>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QWidget>
#include <QList>
#include <QStateMachine>
#include <QState>
#include "scene.hpp"
#include "PushButton.hpp"
#include "node.hpp"
#include "defines.hpp"

// for inner scene
const int rowCnt = 12,colCnt = 25; 
int xCord[] {-1,1,0,0};
int yCord[] {0,0,1,-1};

GraphicsScene::GraphicsScene(const QSize & size) : bar(new QTabWidget()), innerScene(new QGraphicsScene(this)){
         setSceneRect(0,0,size.width(),size.height());

         addWidget(bar);
         bar->setFixedWidth(size.width());
         bar->setFixedHeight(size.height()-25); //? fix
         populateBar();
         populateGridScene();

}

GraphicsScene::~GraphicsScene(){
         delete bar;
}

// sets up top bar
void GraphicsScene::populateBar(){

         {        
                  auto bfsWidget = new QWidget(bar);
                  const QString algoName = "BFS";
                  bar->addTab(bfsWidget,algoName);
                  populateWidget(bfsWidget,algoName,bfsInfo /*inside defines header*/ ); 
         }

         {
                  auto dfsWidget = new QWidget(bar);
                  const QString algoName = "DFS";
                  bar->addTab(dfsWidget,algoName);
                  populateWidget(dfsWidget,algoName,dfsInfo /*inside defines header*/ );
         }
}

// sets up the widget used by tabwidget
void GraphicsScene::populateWidget(QWidget * widget,const QString & algoName,const QString & infoText){
         auto mainLayout = new QGridLayout(widget);
         mainLayout->setAlignment(Qt::AlignTop);
         mainLayout->setSpacing(10);

         auto view = new QGraphicsView(innerScene,widget); // different view but same scene
         mainLayout->addWidget(view,0,0);

         {        // right side bar
                  auto sideLayout = new QVBoxLayout();
                  mainLayout->addLayout(sideLayout,0,1);
                  sideLayout->setAlignment(Qt::AlignTop);

                  auto infoBut = new PushButton(QString("About %1").arg(algoName),widget);
                  auto statusBut = new PushButton("Run",widget);
                  auto resetBut = new PushButton("Reset",widget);
                  auto exitBut = new PushButton("Exit",widget);
         
                  sideLayout->addWidget(infoBut);
                  sideLayout->addWidget(statusBut);
                  sideLayout->addWidget(resetBut);
                  sideLayout->addWidget(exitBut);
                  sideLayout->insertSpacing(3,100);

                  {        // state machine for buttons
                           auto machine = new QStateMachine(widget);
                           auto statusStart = new QState(machine);
                           auto statusEnd = new QState(machine);

                           machine->setInitialState(statusStart);
                           statusStart->assignProperty(statusBut,"text","Run");
                           statusStart->assignProperty(statusBut,"bgColor",QColor(Qt::green));
                           statusEnd->assignProperty(statusBut,"text","Stop");
                           statusEnd->assignProperty(statusBut,"bgColor",QColor(Qt::red));

                           auto startToEnd = new QEventTransition(statusBut,QEvent::MouseButtonPress,machine);
                           auto endToStart = new QEventTransition(statusBut,QEvent::MouseButtonPress,machine);
                           auto colorAnimation = new QPropertyAnimation(statusBut,"bgColor",widget);

                           colorAnimation->setDuration(2500);
                           startToEnd->addAnimation(colorAnimation);
                           endToStart->addAnimation(colorAnimation);
                           startToEnd->setTargetState(statusEnd);
                           endToStart->setTargetState(statusStart);

                           statusStart->addTransition(startToEnd);
                           statusEnd->addTransition(endToStart);
                           machine->start();
                  }

                  connect(infoBut,&QPushButton::clicked,[algoName,infoText]{
                           QMessageBox::information(nullptr,algoName,infoText);
                  });

                  connect(statusBut,&QPushButton::clicked,[this]{
                           // 0 bfs 1 dfs 2 dij
                           switch(bar->currentIndex()){
                                    case 0 : bfs();break;
                                    case 1 : dfs();break;
                                    default : assert(false);
                           }
                  });

                  connect(resetBut,&QPushButton::clicked,[this]{
                           reset();
                  });

                  connect(exitBut,&QPushButton::clicked,[this]{
                           auto choice = QMessageBox::critical(nullptr,"Close","Quit",QMessageBox::No,QMessageBox::Yes);
                           if(choice == QMessageBox::Yes){
                                    emit close();
                           }
                  });
         }

         {        // bottom bar : displays the current algorithm status will be used by grid scene
                  auto infoLine = new QLineEdit("Click on run button on sidebar to display algorithm status");
                  infoLine->setAlignment(Qt::AlignCenter);
                  infoLine->setReadOnly(true);

                  auto bottomLayout = new QHBoxLayout(); 
                  bottomLayout->setAlignment(Qt::AlignLeft);
                  bottomLayout->addWidget(infoLine);
                  mainLayout->addLayout(bottomLayout,1,0);
         }
}

// returns the node ref present inside the grid
Node * GraphicsScene::getNodeAt(const int & row,const int & col) const{
         return static_cast<Node*>(innerLayout->itemAt(row,col));
}

// returns true of {row,col} is a valid cordinate for grid
bool GraphicsScene::validCordinate(const int & row,const int & col) const{
         return row >= 0 && row < rowCnt && col >= 0 && col < colCnt;
}

// returns the bottom lineEdit which displays the current algorithm state
//? improve later
QLineEdit * GraphicsScene::getStatusBar(const int & tabIndex) const{
         assert(tabIndex < bar->count());
         auto widget = bar->widget(tabIndex);
         auto absLayout = static_cast<QGridLayout*>(widget->layout())->itemAtPosition(1,0);
         return static_cast<QLineEdit*>(static_cast<QHBoxLayout*>(absLayout)->itemAt(0)->widget());
}

// sets up the inner grid scene
void GraphicsScene::populateGridScene(){
         auto holder = new QGraphicsWidget();
         innerLayout = new QGraphicsGridLayout(holder);
         holder->setLayout(innerLayout);
         innerScene->addItem(holder);
         innerLayout->setSpacing(25);

         startCord = {0,0};
         endCord = {rowCnt-1,colCnt-1};
         
         for(int row = 0;row < rowCnt;row++){
                  for(int col = 0;col < colCnt;col++){
                           auto node = new Node(); // scene destructs
                           innerLayout->addItem(node,row,col);
                  }
         }
         getNodeAt(startCord.first,startCord.second)->setType(Node::Starter);
         getNodeAt(endCord.first,endCord.second)->setType(Node::Ender);
}

// resets the inner grid
void GraphicsScene::reset(){
         for(int row = 0;row < rowCnt;row++){
                  for(int col = 0;col < colCnt;col++){
                           auto node = static_cast<Node*>(innerLayout->itemAt(row,col));
                           node->setType(Node::Inactive);
                  }
         }
         getNodeAt(startCord.first,startCord.second)->setType(Node::Starter);
         getNodeAt(endCord.first,endCord.second)->setType(Node::Ender);
}

/// implementations gets called when statusBut is clicked ///

// tab index : 0
void GraphicsScene::bfs(){
         std::queue<std::tuple<int,int,int>> queue; // {x,y,distance}
         std::vector<std::vector<bool>> visited(rowCnt,std::vector<bool>(colCnt));

         auto infoLine = getStatusBar(0);
         auto [startX,startY] = startCord;
         auto [endX,endY] = endCord;

         queue.push({startX,startY,0});
         visited[startX][startY] = true;

         while(!queue.empty()){
                  auto [currentRow,currentCol,currentDistance] = queue.front();
                  getNodeAt(currentRow,currentCol)->setType(Node::Active);
                  infoLine->setText(QString("Current Distance : %1").arg(currentDistance));
                  queue.pop();

                  if(currentRow == endX && currentCol == endY){
                           return;
                  }

                  for(int direction = 0;direction < 4;direction++){
                           int toRow = currentRow + xCord[direction];
                           int toCol = currentCol + yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    if(!visited[toRow][toCol]){
                                             visited[toRow][toCol] = true;
                                             queue.push({toRow,toCol,currentDistance+1});
                                    }
                           }
                  }
         }
         // here when there are blocks all around. not possible for now
         assert(false);
}

// tab index : 1
void GraphicsScene::dfs(){
         auto infoLine = getStatusBar(1);
         std::vector<std::vector<bool>> visited(rowCnt,std::vector<bool>(colCnt));

         auto [startX,startY] = startCord;
         auto [endX,endY] = endCord;

         bool reached = false; // whether the endCord is reached or not

         std::function<void(int,int,int)> dfsHelper = [&,endX = endX,endY = endY](int curX,int curY,int curDist){
                  if(reached || !validCordinate(curX,curY)) return;
                  else if(visited[curX][curY]) return;

                  infoLine->setText(QString("Current Distance: %1").arg(curDist));

                  if(curX == endX && curY == endY){
                           reached = true;
                           qInfo() << curDist;
                           return;
                  }

                  visited[curX][curY] = true;

                  for(int direction = 0;direction < 4;direction++){
                           int toRow = curX + xCord[direction];
                           int toCol = curY + yCord[direction];
                           dfsHelper(toRow,toCol,curDist+1);
                  }
         };

         dfsHelper(startX,startY,0);
}