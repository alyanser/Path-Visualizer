#include <QTabWidget>
#include <QSlider>
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
#include <memory>
#include <QGraphicsProxyWidget>
#include <QWidget>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <queue>
#include <stack>
#include <QTimer>
#include <iostream>
#include "scene.hpp"
#include "PushButton.hpp"
#include "node.hpp"
#include "defines.hpp"

// for inner scene
const int rowCnt = 12,colCnt = 25; 
const uint32_t defaultSpeed = 75;
const int xCord[] {-1,1,0,0};
const int yCord[] {0,0,1,-1};

GraphicsScene::GraphicsScene(const QSize & size) : speed(defaultSpeed), bar(new QTabWidget()), innerScene(new QGraphicsScene(this)){
         sourceNode = targetNode = nullptr;
         setSceneRect(0,0,size.width(),size.height());
         on = false;
         bfsTimer = new QTimer(bar);
         dfsTimer = new QTimer(bar);
         dijTimer = new QTimer(bar);

         bfsTimer->setInterval(speed);
         dfsTimer->setInterval(speed);
         dijTimer->setInterval(speed);
         
         addWidget(bar);
         bar->setFixedWidth(size.width());
         bar->setFixedHeight(size.height()-25); //? fix

         setDataStructures();
         memsetDs(); 
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

//! self deletion
void GraphicsScene::setDataStructures(){
         using std::make_unique;
         using std::vector;

         queue = make_unique<std::queue<std::pair<Node*,int>>>(); // {node,dist}
         stack = make_unique<std::stack<std::pair<Node*,int>>>(); // {node,dist}
         visited = make_unique<vector<vector<bool>>>(); 
         distance = make_unique<vector<vector<int>>>();
         using nodePair = std::pair<int,Node*>;
         pq = make_unique<std::priority_queue<nodePair,vector<nodePair>,std::greater<>>>(); // {dist,node}
}

void GraphicsScene::memsetDs(){
         while(!queue->empty()) queue->pop();
         while(!stack->empty()) stack->pop();
         while(!pq->empty()) pq->pop();
         distance->assign(rowCnt,std::vector<int>(colCnt,INT_MAX));
         visited->assign(rowCnt,std::vector<bool>(colCnt,false));
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

                           statusStart->assignProperty(this,"on",false);
                           statusEnd->assignProperty(this,"on",true);

                           auto startToEnd = new QEventTransition(statusBut,QEvent::MouseButtonPress,machine);
                           auto endToStart = new QEventTransition(statusBut,QEvent::MouseButtonPress,machine);
                           auto endedTransition = new QSignalTransition(this,&GraphicsScene::resetButtons,statusEnd);
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

                  connect(statusBut,&QPushButton::clicked,[this,statusBut]{
                           const QString & currentText = statusBut->text();
                           bool newStart = false;

                           if(currentText == "Stop"){
                                    statusBut->setText("Continue");
                                    stopTimers();
                                    return;
                           }else if(currentText == "Run"){ // remove any previous items
                                    statusBut->setText("Stop");
                                    cleanup();
                                    newStart = true;
                           }else{
                                    statusBut->setText("Stop");
                           }

                           switch(bar->currentIndex()){
                                    case 0 : bfs(newStart);break;
                                    case 1 : dfs(newStart);break;
                                    case 2 : dijkstra(newStart);break;
                                    default : assert(false);
                           }

                  });

                  connect(this,&GraphicsScene::runningStatusChanged,&Node::setRunningState);

                  connect(resetBut,&QPushButton::clicked,[this,statusBut]{
                           statusBut->setText("Run");
                           stopTimers();
                           emit resetButtons();
                           memsetDs(); // remove any pending items in data structures
                           for(int row = 0;row < rowCnt;row++){
                                    for(int col = 0;col < colCnt;col++){
                                             auto node = static_cast<Node*>(innerLayout->itemAt(row,col));
                                             if(isSpecial(node)) continue; // do not remove source or target node
                                             node->setPathParent(nullptr);
                                             node->setType(Node::Inactive);
                                    }
                           }
                           // reset bottom status bar text
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
                  auto infoLine = new QLineEdit("Click on run button on sidebar to display algorithm status",widget);
                  infoLine->setAlignment(Qt::AlignCenter); // text align
                  infoLine->setReadOnly(true);
                  
                  auto slider = new QSlider(Qt::Horizontal,widget);
                  slider->setMaximum(1000);
                  slider->setMinimum(50);
                  slider->setValue(defaultSpeed);

                  auto bottomLayout = new QHBoxLayout(); 
                  bottomLayout->setAlignment(Qt::AlignCenter);
                  bottomLayout->addWidget(infoLine);
                  bottomLayout->addSpacing(40);
                  bottomLayout->addWidget(new QLabel("Speed: "));
                  bottomLayout->addWidget(slider);
                  mainLayout->addLayout(bottomLayout,1,0);
         }
}

/// utility ///

void GraphicsScene::setRunning(const bool & newState){
         on = newState;
         emit runningStatusChanged(on);
}

bool GraphicsScene::isRunning() const{
         return on;
}

// stops all timers - removes pending shots if any
void GraphicsScene::stopTimers() const{
         dfsTimer->stop();
         bfsTimer->stop();
         dijTimer->stop();
}

// returns a new node and connects signals to update when source and target is changed
Node * GraphicsScene::getNewNode(const int & row,const int & col){
         auto node = new Node(row,col);

         connect(node,&Node::sourceSet,[&sourceNode = sourceNode,node]{
                  sourceNode = node;
         });

         connect(node,&Node::targetSet,[&targetNode = targetNode,node]{
                  targetNode = node;
         });

         return node;
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

// returns true if current node is either sourceNode or targetNode - more status in update
bool GraphicsScene::isSpecial(Node * currentNode) const{
         return currentNode == sourceNode || currentNode == targetNode;
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
                           auto node = getNewNode(row,col);
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
                  if(!isSpecial(currentNode)){
                           currentNode->setType(Node::Inpath);
                  }
                  currentNode = currentNode->getPathParent();
         }
}

// tab index : 0
void GraphicsScene::bfs(const bool & newStart) const{

         bfsTimer->start();

         auto infoLine = getStatusBar(0);

         if(newStart){
                  queue->push({sourceNode,0});
                  auto [startX,startY] = sourceNode->getCord();
                  (*visited)[startX][startY] = true;
         }

         connect(bfsTimer,&QTimer::timeout,[this,infoLine = infoLine]{ 
                  if(queue->empty()){
                           bfsTimer->stop();
                           infoLine->setText("Could not reach destination.");
                           emit resetButtons();
                           return;
                  }
                  auto [currentNode,currentDistance] = queue->front();
                  queue->pop();
                  auto [currentRow,currentCol] = currentNode->getCord();

                  if(!isSpecial(currentNode)){
                           currentNode->setType(Node::Active);
                  }

                  auto nodeParent = currentNode->getPathParent();

                  if(nodeParent && !isSpecial(nodeParent)){
                           nodeParent->setType(Node::Visited);
                  }

                  infoLine->setText(QString("Current Distance : %1").arg(currentDistance));

                  if(currentNode == targetNode){
                           bfsTimer->stop();
                           getPath();
                           emit resetButtons();
                           return;
                  }

                  for(int direction = 0;direction < 4;direction++){
                           int toRow = currentRow + xCord[direction];
                           int toCol = currentCol + yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    auto togoNode = getNodeAt(toRow,toCol);

                                    if(isBlock(togoNode) || (*visited)[toRow][toCol]) continue;
                                    
                                    (*visited)[toRow][toCol] = true;
                                    togoNode->setPathParent(currentNode);
                                    queue->push({togoNode,currentDistance+1});
                           }
                  }
         });
}

// tab index : 1
void GraphicsScene::dfs(const bool & newStart) const{
         dfsTimer->start();
         auto infoLine = getStatusBar(1);

         if(newStart){
                  stack->push({sourceNode,0});
                  auto [startX,startY] = sourceNode->getCord();
                  (*visited)[startX][startY] = true;
         }

         connect(dfsTimer,&QTimer::timeout,[this,infoLine = infoLine]{

                  if(stack->empty()){
                           dfsTimer->stop();
                           infoLine->setText("Could not reach destination.");
                           emit resetButtons();
                           return;
                  }

                  auto [currentNode,currentDistance] = stack->top();
                  stack->pop();

                  infoLine->setText(QString("Current Distance : %1").arg(currentDistance));

                  if(!isSpecial(currentNode)){
                           currentNode->setType(Node::Active);
                  }

                  if(currentNode == targetNode){
                           dfsTimer->stop();
                           getPath();
                           emit resetButtons();
                           return;
                  }

                  auto [currentRow,currentCol] = currentNode->getCord();
                  auto nodeParent = currentNode->getPathParent();

                  if(nodeParent && !isSpecial(nodeParent)){
                           nodeParent->setType(Node::Visited);
                  }

                  for(int direction = 0;direction < 4;direction++){
                           int toRow = currentRow + xCord[direction];
                           int toCol = currentCol + yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    auto togoNode = getNodeAt(toRow,toCol);

                                    if(isBlock(togoNode) || (*visited)[toRow][toCol]) continue;
                                    
                                    (*visited)[toRow][toCol] = true;
                                    togoNode->setPathParent(currentNode);
                                    stack->push({togoNode,currentDistance+1});
                           }
                  }
         });
}

// tab index : 2
void GraphicsScene::dijkstra(const bool & newStart) const{
         dijTimer->start();
         auto infoLine = getStatusBar(2);
         
         if(newStart){
                  pq->push({0,sourceNode});
                  auto [startX,startY] = sourceNode->getCord();
                  (*distance)[startX][startY] = 0;
         }

         connect(dijTimer,&QTimer::timeout,[this,infoLine = infoLine]{

                  if(pq->empty()){
                           qInfo() << "hello there";
                           QMessageBox::warning(nullptr,"No path","Could not reach destination.");
                           dijTimer->stop();
                           return;
                  }

                  auto [currentDistance,currentNode] = pq->top();
                  pq->pop();

                  auto [curX,curY] = currentNode->getCord();

                  if((*distance)[curX][curY] != currentDistance) return;

                  if(!isSpecial(currentNode)){
                           currentNode->setType(Node::Active);
                  }

                  auto nodeParent = currentNode->getPathParent();
                  
                  if(nodeParent && !isSpecial(nodeParent)){
                           nodeParent->setType(Node::Visited);
                  }

                  infoLine->setText(QString("Current Distance: %1").arg(currentDistance));

                  if(currentNode == targetNode){
                           dijTimer->stop();
                           getPath();
                           emit resetButtons();
                           return;
                  }

                  for(int direction = 0;direction < 4;direction++){
                           int toRow = curX + xCord[direction];
                           int toCol = curY + yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    auto togoNode = getNodeAt(toRow,toCol);

                                    if(isBlock(togoNode)) continue;

                                    int & destDistance = (*distance)[toRow][toCol]; 
                                    const int newDistance = currentDistance + 1;

                                    if(newDistance < destDistance){
                                             destDistance = newDistance;
                                             togoNode->setPathParent(currentNode);
                                             pq->push({newDistance,togoNode});
                                    }
                           }
                  }
         });
}

