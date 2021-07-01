#include <QTabWidget>
#include <QSignalTransition>
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
#include <queue>
#include <QTimer>
#include "scene.hpp"
#include "PushButton.hpp"
#include "node.hpp"
#include "defines.hpp"

// for inner scene
const int rowCnt = 12,colCnt = 25; 
const int xCord[] {-1,1,0,0};
const int yCord[] {0,0,1,-1};

GraphicsScene::GraphicsScene(const QSize & size) : bar(new QTabWidget()), innerScene(new QGraphicsScene(this)){
         sourceNode = targetNode = nullptr;
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

         {        // bfs
                  auto bfsWidget = new QWidget(bar);
                  const QString algoName = "BFS";
                  bar->addTab(bfsWidget,algoName);
                  populateWidget(bfsWidget,algoName,bfsInfo /*inside defines header*/ ); 
         }

         {        // dfs
                  auto dfsWidget = new QWidget(bar);
                  const QString algoName = "DFS";
                  bar->addTab(dfsWidget,algoName);
                  populateWidget(dfsWidget,algoName,dfsInfo /*inside defines header*/ );
         }

         {        // dijkstra
                  auto dijWidget = new QWidget(bar);
                  const QString algoName = "dijkstra";
                  bar->addTab(dijWidget,algoName);
                  populateWidget(dijWidget,algoName,dijInfo /*inside defines header*/ );
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

                  auto infoBut = new PushButton("Information",widget);
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
                           statusStart->assignProperty(statusBut,"bgColor",QColor(Qt::green));
                           statusEnd->assignProperty(statusBut,"bgColor",QColor(Qt::red));
                           statusStart->assignProperty(statusBut,"text","Run");
                           statusEnd->assignProperty(statusBut,"text","Stop");

                           auto startToEnd = new QEventTransition(statusBut,QEvent::MouseButtonPress,machine);
                           auto endToStart = new QEventTransition(statusBut,QEvent::MouseButtonPress,machine);
                           auto endedTransition = new QSignalTransition(this,&GraphicsScene::reached,statusEnd);
                           auto colorAnimation = new QPropertyAnimation(statusBut,"bgColor",widget);

                           colorAnimation->setDuration(1000);
                           startToEnd->addAnimation(colorAnimation);
                           endToStart->addAnimation(colorAnimation);
                           startToEnd->setTargetState(statusEnd);
                           endToStart->setTargetState(statusStart);
                           endedTransition->setTargetState(statusStart);
                           endedTransition->addAnimation(colorAnimation);

                           statusStart->addTransition(startToEnd);
                           statusEnd->addTransition(endToStart);
                           machine->start();
                  }

                  connect(infoBut,&QPushButton::clicked,[algoName,infoText]{
                           QMessageBox::information(nullptr,algoName,infoText);
                  });

                  connect(statusBut,&QPushButton::clicked,[this]{
                           //TODO animation
                           cleanup();
                           switch(bar->currentIndex()){
                                    case 0 : bfs();break;
                                    case 1 : dfs();break;
                                    case 2 : dijkstra();break;
                                    default : assert(false);
                           }
                  });

                  connect(resetBut,&QPushButton::clicked,[this]{
                           for(int row = 0;row < rowCnt;row++){
                                    for(int col = 0;col < colCnt;col++){
                                             auto node = static_cast<Node*>(innerLayout->itemAt(row,col));
                                             node->setPathParent(nullptr);
                                             node->setType(Node::Inactive);
                                    }
                           }
                           sourceNode->setType(Node::Source);
                           targetNode->setType(Node::Target);

                           auto curTabIndex = bar->currentIndex();
                           auto lineInfo = getStatusBar(curTabIndex);
                           lineInfo->setText("Click on run button on sidebar to display algorithm status");
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
                  infoLine->setAlignment(Qt::AlignCenter); // text align
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

// sets the type - used in cleanup && reset button
void GraphicsScene::updateSrcTarNodes(){
         assert(sourceNode && targetNode);
         sourceNode->setType(Node::Source);
         targetNode->setType(Node::Target);
}

// checks whether the algorithm can proceed through the current node
bool GraphicsScene::isBlock(Node * currentNode) const{
         return currentNode->getType() == Node::Block;
}

// sets up the inner grid scene
void GraphicsScene::populateGridScene(){
         auto holder = new QGraphicsWidget();
         innerLayout = new QGraphicsGridLayout(holder);
         holder->setLayout(innerLayout);
         innerScene->addItem(holder);
         innerLayout->setSpacing(25);
         
         for(int row = 0;row < rowCnt;row++){
                  for(int col = 0;col < colCnt;col++){
                           auto node = new Node(row,col); // scene destructs
                           innerLayout->addItem(node,row,col);
                  }
         }

         startCord = {0,0};
         endCord = {rowCnt-1,colCnt-1};

         sourceNode = getNodeAt(startCord.first,startCord.second);
         targetNode = getNodeAt(endCord.first,endCord.second);
         updateSrcTarNodes();
         innerScene->setFocus();
}

// clears everything except the user specified blocks
void GraphicsScene::cleanup(){
         for(int row = 0;row < rowCnt;row++){
                  for(int col = 0;col < colCnt;col++){
                           auto node = static_cast<Node*>(innerLayout->itemAt(row,col));
                           if(isBlock(node)) continue;
                           node->setType(Node::Inactive);
                  }
         }
         updateSrcTarNodes();
}

/// implementations - gets called when statusBut is clicked ///

// follows the parent pointer in node class and marks the visited node to show path
void GraphicsScene::getPath() const{
         Node * currentNode = targetNode;
         while(currentNode){
                  currentNode->setType(Node::Inpath);
                  currentNode = currentNode->getPathParent();
         }
}

// tab index : 0
void GraphicsScene::bfs() const{
         auto infoLine = getStatusBar(0);
         std::queue<std::pair<Node*,int>> queue; // {node,distance}
         std::vector<std::vector<bool>> visited(rowCnt,std::vector<bool>(colCnt));

         auto [startX,startY] = sourceNode->getCord();
         visited[startX][startY] = true;
         
         queue.push({sourceNode,0});

         while(!queue.empty()){
                  auto [currentNode,currentDistance] = queue.front();
                  queue.pop();

                  auto [currentRow,currentCol] = currentNode->getCord();
                  currentNode->setType(Node::Active);

                  auto nodeParent = currentNode->getPathParent();
                  if(nodeParent){
                           nodeParent->setType(Node::Visited);
                  }

                  infoLine->setText(QString("Current Distance : %1").arg(currentDistance));

                  if(currentNode == targetNode){
                           getPath();
                           return;
                  }

                  for(int direction = 0;direction < 4;direction++){
                           int toRow = currentRow + xCord[direction];
                           int toCol = currentCol + yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    auto togoNode = getNodeAt(toRow,toCol);

                                    if(isBlock(togoNode) || visited[toRow][toCol]) continue;
                                    
                                    visited[toRow][toCol] = true;
                                    togoNode->setPathParent(currentNode);
                                    queue.push({togoNode,currentDistance+1});
                           }
                  }
         }
}

// tab index : 1
void GraphicsScene::dfs() const{
         auto infoLine = getStatusBar(1);
         std::vector<std::vector<bool>> visited(rowCnt,std::vector<bool>(colCnt));

         bool targetFound = false; 

         std::function<void(Node*,int)> dfsHelper = [&](Node * currentNode,int currentDistance){
                  auto [curX,curY] = currentNode->getCord();
                  if(targetFound) return;

                  visited[curX][curY] = true;

                  currentNode->setType(Node::Active);
                  infoLine->setText(QString("Current Distance: %1").arg(currentDistance));

                  auto pathParent = currentNode->getPathParent();
                  if(pathParent){
                           pathParent->setType(Node::Visited);
                  }

                  if(currentNode == targetNode){
                           targetFound = true;
                           return;
                  }

                  visited[curX][curY] = true;

                  for(int direction = 0;direction < 4;direction++){
                           int toRow = curX + xCord[direction];
                           int toCol = curY + yCord[direction];

                           if(!validCordinate(toRow,toCol) || visited[toRow][toCol]) continue;

                           auto togoNode = getNodeAt(toRow,toCol);
                           if(isBlock(togoNode)) continue;
                           
                           togoNode->setPathParent(currentNode);
                           dfsHelper(togoNode,currentDistance+1);
                  }
         };

         dfsHelper(sourceNode,0);

         if(targetFound){
                  getPath();
         }
}

// tab index : 2
void GraphicsScene::dijkstra() const{
         auto infoLine = getStatusBar(2);
         std::vector<std::vector<int>> distance(rowCnt,std::vector<int>(colCnt,INT_MAX));

         std::priority_queue<std::pair<int,Node*>,std::vector<std::pair<int,Node*>>,std::greater<>> pq;

         auto [startX,startY] = sourceNode->getCord();
         
         distance[startX][startY] = 0;
         pq.push({0,sourceNode});

         while(!pq.empty()){
                  auto [currentDistance,currentNode] = pq.top();
                  pq.pop();

                  auto [curX,curY] = currentNode->getCord();

                  if(distance[curX][curY] != currentDistance) continue;

                  auto nodeParent = currentNode->getPathParent();
                  
                  if(nodeParent){
                           assert(nodeParent->getType() != Node::Block);
                           nodeParent->setType(Node::Visited);
                  }

                  assert(currentNode->getType() != Node::Block);
                  currentNode->setType(Node::Active);

                  infoLine->setText(QString("Current Distance: %1").arg(currentDistance));

                  if(currentNode == targetNode){
                           getPath();
                           reached();
                           return;
                  }

                  for(int direction = 0;direction < 4;direction++){
                           int toRow = curX + xCord[direction];
                           int toCol = curY + yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    auto togoNode = getNodeAt(toRow,toCol);
                                    if(isBlock(togoNode)) continue;

                                    int & destDistance = distance[toRow][toCol]; 
                                    const int newDistance = currentDistance + 1;

                                    if(newDistance < destDistance){
                                             destDistance = newDistance;
                                             togoNode->setPathParent(currentNode);
                                             pq.push({newDistance,togoNode});
                                    }
                           }
                  }
         }
}

