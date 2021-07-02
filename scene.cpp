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
const uint32_t defDelay = 25;
const int xCord[] {-1,1,0,0};
const int yCord[] {0,0,1,-1};

GraphicsScene::GraphicsScene(const QSize & size) : timerDelay(defDelay), bar(new QTabWidget()), innerScene(new QGraphicsScene(this)){
         setSceneRect(0,0,size.width(),size.height());

         // bar is set as parent - auto deletion of timers
         bfsTimer = new QTimer(bar);
         dfsTimer = new QTimer(bar);
         dijkstraTimer = new QTimer(bar);
         pathTimer = new QTimer(bar);

         setTimersIntervals(timerDelay);

         addWidget(bar);
         bar->setFixedWidth(size.width());
         bar->setFixedHeight(size.height()-25); //? fix

         allocDataStructures();
         memsetDs(); 
         populateBar();
         populateGridScene();

         // connects the timers' timeout signal with lambdas inside these methods
         bfsConnect();
         dfsConnect();
         dijkstraConnect();
         pathConnect();
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
                  const QString algoName = "Dijkstra";
                  bar->addTab(dijWidget,algoName);
                  populateWidget(dijWidget,algoName,dijkstraInfo /*inside defines header*/ );
         }
}

void GraphicsScene::allocDataStructures(){
         using std::make_unique;
         using std::vector;

         queue = make_unique<std::queue<std::pair<Node*,int>>>(); // {:distance}
         stack = make_unique<std::stack<std::pair<Node*,int>>>(); // {:distance}
         visited = make_unique<vector<vector<bool>>>(); 
         distance = make_unique<vector<vector<int>>>();
         using nodePair = std::pair<int,Node*>;
         pq = make_unique<std::priority_queue<nodePair,vector<nodePair>,std::greater<>>>(); // {distance:}
}

void GraphicsScene::memsetDs(){
         while(!queue->empty()) queue->pop();
         while(!stack->empty()) stack->pop();
         while(!pq->empty()) pq->pop();
         distance->assign(rowCnt,std::vector<int>(colCnt,INT_MAX));
         visited->assign(rowCnt,std::vector<bool>(colCnt,false));
}

void GraphicsScene::populateWidget(QWidget * widget,const QString & algoName,const QString & infoText){
         auto mainLayout = new QGridLayout(widget);
         mainLayout->setAlignment(Qt::AlignTop);
         mainLayout->setSpacing(10);

         auto view = new QGraphicsView(innerScene,widget); 
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
                           //! setting widget gives weird layout - TODO fix later
                           QMessageBox::information(nullptr,algoName,infoText);
                  });

                  // emitted when an algorithm finishes 
                  connect(this,&GraphicsScene::resetButtons,statusBut,[this,statusBut]{
                           statusBut->setText("Run");
                           memsetDs();
                  });

                  connect(statusBut,&QPushButton::clicked,statusBut,[this,statusBut]{
                           const QString & currentText = statusBut->text();
                           bool newStart = false;

                           if(currentText == "Stop"){
                                    statusBut->setText("Continue");
                                    stopTimers();
                                    return;
                           }else if(currentText == "Run"){ 
                                    cleanup(); // remove any previous items
                                    statusBut->setText("Stop");
                                    newStart = true;
                           }else{
                                    statusBut->setText("Stop");
                           }

                           switch(bar->currentIndex()){
                                    case 0 : bfsStart(newStart);break;
                                    case 1 : dfsStart(newStart);break;
                                    case 2 : dijkstraStart(newStart);break;
                                    default : assert(false);
                           }

                  });

                  connect(this,&GraphicsScene::runningStatusChanged,&Node::setRunningState);

                  // different from resetbuttons signal as that doesn't reset whole grid
                  connect(resetBut,&QPushButton::clicked,statusBut,[this,statusBut]{
                           statusBut->setText("Run");
                           stopTimers();
                           emit resetButtons();
                           memsetDs();
                           for(int row = 0;row < rowCnt;row++){
                                    for(int col = 0;col < colCnt;col++){
                                             auto node = static_cast<Node*>(innerLayout->itemAt(row,col));
                                             if(isSpecial(node)) continue; 
                                             node->setPathParent(nullptr);
                                             node->setType(Node::Inactive);
                                    }
                           }
                           auto curTabIndex = bar->currentIndex();
                           auto lineInfo = getStatusBar(curTabIndex);
                           lineInfo->setText("Click on run button on sidebar to display algorithm status");
                  });

                  connect(exitBut,&QPushButton::clicked,this,[this]{
                           auto choice = QMessageBox::critical(nullptr,"Close","Quit",QMessageBox::No,QMessageBox::Yes);
                           if(choice == QMessageBox::Yes){
                                    emit close();
                           }
                  });
         }

         {        // bottom bar 
                  auto infoLine = new QLineEdit("Click on run button on sidebar to display algorithm status",widget);
                  infoLine->setAlignment(Qt::AlignCenter); // text align
                  infoLine->setReadOnly(true);
                  
                  auto slider = new QSlider(Qt::Horizontal,widget);
                  slider->setRange(0,1000);
                  slider->setValue(925);
                  slider->setTracking(true);

                  connect(slider,&QSlider::valueChanged,this,&GraphicsScene::setDelay);
         
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
         runningState = newState;
         emit runningStatusChanged(runningState); // connected with node class
}

// algorithm state
bool GraphicsScene::isRunning() const{
         return runningState;
}

// linked with timers
void GraphicsScene::setDelay(const uint32_t & newDelay){
         // slide bar has more value when it is set on the fuller side convert before
         timerDelay = std::abs(1000ll - newDelay); // ll to avoid unsigned 
         setTimersIntervals(timerDelay);
}

// sets intervals - linked with slide bar
void GraphicsScene::setTimersIntervals(const uint32_t & newDelay) const{
         dfsTimer->setInterval(newDelay);
         bfsTimer->setInterval(newDelay);
         dijkstraTimer->setInterval(newDelay);
         pathTimer->setInterval(newDelay);
}

// stops all timers - removes pending shots if any
void GraphicsScene::stopTimers() const{
         dfsTimer->stop();
         bfsTimer->stop();
         dijkstraTimer->stop();
         pathTimer->stop();
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
void GraphicsScene::pathConnect() const{
         auto moveUp = [this,currentNode = targetNode,counter = 0]() mutable {
                  if(!counter++){ // called for the first time - TODO find better way
                           currentNode = targetNode;
                  }
                  if(!currentNode){
                           counter = 0; // reset counter for the next start
                           pathTimer->stop();
                           return;
                  }else if(!isSpecial(currentNode)){
                           currentNode->setType(Node::Inpath);
                  }
                  currentNode = currentNode->getPathParent();
         };
         connect(pathTimer,&QTimer::timeout,targetNode,moveUp,Qt::UniqueConnection);
}

/// starters ///

// called when an algorithm succeeds
void GraphicsScene::getPath() const{
         pathTimer->start();
}

// called when start is pressed on bfs Tab
void GraphicsScene::bfsStart(const bool & newStart) const{
         if(newStart){ 
                  queue->push({sourceNode,0});
                  auto [startX,startY] = sourceNode->getCord();
                  (*visited)[startX][startY] = true;
         }
         bfsTimer->start();
}

// called when start is pressed on dfs tab
void GraphicsScene::dfsStart(const bool & newStart) const{
         if(newStart){
                  stack->push({sourceNode,0});
                  auto [startX,startY] = sourceNode->getCord();
                  (*visited)[startX][startY] = true;
         }
         dfsTimer->start();
}

// called when start is pressed on dijistraTab
void GraphicsScene::dijkstraStart(const bool & newStart) const{
         if(newStart){ 
                  pq->push({0,sourceNode});
                  auto [startX,startY] = sourceNode->getCord();
                  (*distance)[startX][startY] = 0;
         }
         dijkstraTimer->start();
}

/// connections - only called once to set connections with respective timers ///
//TODO move out of functions and connect directly

// tab index : 0
// connects bfstimer with the 'implementation' lambda
void GraphicsScene::bfsConnect() const{
         auto infoLine = getStatusBar(0);

         auto implementation = [this,infoLine = infoLine]{
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
         };
         // calls std::move on implemenation - connected just once
         connect(bfsTimer,&QTimer::timeout,implementation);
}

// tab index : 1
// connects dfsTimer with the 'implementation' lambda
void GraphicsScene::dfsConnect() const{
         auto infoLine = getStatusBar(1);
         
         auto implementation = [this,infoLine = infoLine]{

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
         };
         // calls std::move on implementation - connected just once
         connect(dfsTimer,&QTimer::timeout,implementation);
}

// tab index : 2
// connects dijistraTimer with 'implementation' lambda
void GraphicsScene::dijkstraConnect() const{
         auto infoLine = getStatusBar(2);

         auto implementation = [this,infoLine = infoLine]{
                  if(pq->empty()){
                           dijkstraTimer->stop();
                           infoLine->setText("Could not reach destination.");
                           emit resetButtons();
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
                           dijkstraTimer->stop();
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
         };
         // cals std::move on implementation - connected just once
         connect(dijkstraTimer,&QTimer::timeout,implementation);         
}
