#include <QTabWidget>
#include <random>
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
#include <QStateMachine>
#include <QState>
#include <queue>
#include <stack>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QTabBar>
#include <QChar>
#include <QIcon>
#include <chrono>
#include "scene.hpp"
#include "pushButton.hpp"
#include "node.hpp"
#include "helpDialog.hpp"

namespace{
         #include "defines.hpp"
}

GraphicsScene::GraphicsScene(const QSize & size) : timerDelay(defDelay){
         innerScene = new QGraphicsScene(this);
         helpWidget = new StackedWidget();
         bar = new QTabWidget();
         bfsTimer = new QTimer(bar);
         dfsTimer = new QTimer(bar);
         dijkstraTimer = new QTimer(bar);
         pathTimer = new QTimer(bar);

         helpWidget->setObjectName("__helpWidget__"); // css

         setSceneRect(0,0,size.width(),size.height());
         setTimersIntervals(timerDelay);

         addWidget(bar);

         bar->setFixedWidth(size.width());
         bar->setFixedHeight(size.height()-25); //? fix

         allocDataStructures();
         memsetDs();
         populateBar();
         populateGridScene();

         bfsConnect();
         dfsConnect();
         dijkstraConnect();
         pathConnect();

         constexpr int helpShowDelay = 1000;
         QTimer::singleShot(helpShowDelay,helpWidget,&StackedWidget::show);

         connect(this,&GraphicsScene::runningStatusChanged,&Node::setRunningState);
         connect(this,SIGNAL(foundPath()),pathTimer,SLOT(start()));
}

GraphicsScene::~GraphicsScene(){
         delete bar;
         delete helpWidget;
}

// sets up top bar
void GraphicsScene::populateBar(){
         {        // bfs
                  auto bfsWidget = new QWidget(bar);
                  const QString algoName = "BFS";
                  bar->addTab(bfsWidget,algoName);
                  populateWidget(bfsWidget,algoName,::bfsInfo);
         }
         {        // dfs
                  auto dfsWidget = new QWidget(bar);
                  const QString algoName = "DFS";
                  bar->addTab(dfsWidget,algoName);
                  populateWidget(dfsWidget,algoName,::dfsInfo);
         }
         {        // dijistra 
                  auto dijkstraWidget = new QWidget(bar);
                  const QString algoName = "Dijkstra";
                  bar->addTab(dijkstraWidget,algoName);
                  populateWidget(dijkstraWidget,algoName,::dijkstraInfo);
         }
}

void GraphicsScene::allocDataStructures(){
         using std::make_unique;
         using std::vector;
         typedef std::pair<int,Node*> pIntNode;

         queue = make_unique<std::queue<std::pair<Node*,int>>>(); // {:distance}
         stack = make_unique<std::stack<std::pair<Node*,int>>>(); // {:distance}
         visited = make_unique<vector<vector<bool>>>();
         distance = make_unique<vector<vector<int>>>();
         pq = make_unique<std::priority_queue<pIntNode,vector<pIntNode>,std::greater<>>>(); // {distance:}
}

void GraphicsScene::memsetDs() const {
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

         auto sideLayout = new QVBoxLayout(); 
         sideLayout->setSpacing(5);
         mainLayout->addLayout(sideLayout,0,1);
         sideLayout->setAlignment(Qt::AlignTop);

         populateSideLayout(sideLayout,algoName,infoText);

         populateLegend(widget,sideLayout);
         populateBottomLayout(widget,mainLayout);
}

void GraphicsScene::populateSideLayout(QVBoxLayout * sideLayout,const QString & algoName,const QString & infoText){
         auto parentWidget = sideLayout->widget();
         auto infoButton = new PushButton("Information",parentWidget);
         auto statusButton = new PushButton("Run",parentWidget);
         auto resetButton = new PushButton("Reset",parentWidget);
         auto randomButton = new PushButton("Random",parentWidget);
         auto helpButton = new PushButton("Help",parentWidget);
         auto exitButton = new PushButton("Exit",parentWidget);

         sideLayout->addWidget(infoButton);
         sideLayout->addWidget(statusButton);
         sideLayout->addWidget(resetButton);
         sideLayout->addWidget(randomButton);
         sideLayout->addWidget(helpButton);
         sideLayout->addWidget(exitButton);
         sideLayout->insertSpacing(4,25);

         configureMachine(parentWidget,statusButton);

         connect(infoButton,&QPushButton::released,[algoName,infoText]{
                  QMessageBox::information(nullptr,algoName,infoText);
         });

         // emitted when an algorithm finishes
         connect(this,&GraphicsScene::resetButtons,statusButton,[this,statusButton]{
                  statusButton->setText("Run");
                  memsetDs();
         });

         connect(statusButton,&QPushButton::released,statusButton,[this,statusButton]{
                  // statemachine has already changed the state as it acts first
                  
                  if(running){
                           bool toStartNew = statusButton->text() == "Run";  // else continue
                           statusButton->setText("Stop");

                           if(toStartNew){
                                    cleanup();
                                    memsetDs();
                           }

                           switch(bar->currentIndex()){
                                    case 0 : bfsStart(toStartNew);break;
                                    case 1 : dfsStart(toStartNew);break;
                                    case 2 : dijkstraStart(toStartNew);break;
                                    default : assert(false);
                           }
                  }else{
                           stopTimers();
                           statusButton->setText("Continue");
                  }
         });

         connect(resetButton,&QPushButton::released,statusButton,[this,statusButton]{
                  statusButton->setText("Run");
                  resetGrid();
         });

         connect(randomButton,&PushButton::released,[this]{
                  resetGrid();
                  generateRandGridPattern();
         });
         
         connect(helpButton,&PushButton::released,&*helpWidget,&QStackedWidget::show);

         connect(exitButton,&QPushButton::released,this,[this]{
                  auto choice = QMessageBox::critical(nullptr,"Close","Quit",QMessageBox::No,QMessageBox::Yes);
                  if(choice == QMessageBox::Yes){
                           emit close();
                  }
         });
}

void GraphicsScene::resetGrid() const {
         stopTimers();
         emit resetButtons();
         memsetDs();
         for(int row = 0;row < rowCnt;row++){
                  for(int col = 0;col < colCnt;col++){
                           auto node = static_cast<Node*>(innerLayout->itemAt(row,col));
                           if(isSpecial(node)) continue;
                           node->setPathParent(nullptr);
                           bool runAnimations = false;
                           node->setType(Node::Inactive,runAnimations);
                  }
         }
         auto curTabIndex = bar->currentIndex();
         auto lineInfo = getStatusBar(curTabIndex);
         lineInfo->setText("Click on run Button on sidebar to display algorithm status");
}

void GraphicsScene::generateRandGridPattern(){
         sourceNodeCord = getRandomCord();
         while((targetNodeCord = getRandomCord()) == sourceNodeCord);

         sourceNode->setType(Node::Inactive);
         targetNode->setType(Node::Inactive);
         sourceNode = getNodeAt(sourceNodeCord.first,sourceNodeCord.second);
         targetNode = getNodeAt(targetNodeCord.first,targetNodeCord.second);
         updateSrcTarNodes();

         constexpr int maximumBlocks = 60; // / 200

         std::random_device rd;
         std::mt19937 gen(rd());
         std::uniform_int_distribution<int> binary(0,1);

         for(int placed = 0;placed < maximumBlocks;){
                  auto [randX,randY] = getRandomCord();
                  auto node = getNodeAt(randX,randY);

                  if(binary(gen) && !isSpecial(node)){
                           placed++;
                           node->setType(Node::Block);
                  }
         }
}

// manager custom properties - currently just statusButton
void GraphicsScene::configureMachine(QWidget * parentWidget,QPushButton * statusButton){
         auto machine = new QStateMachine(parentWidget);
         auto stoppedState = new QState(machine);
         auto runningState = new QState(machine);

         stoppedState->assignProperty(this,"running",false);
         runningState->assignProperty(this,"running",true);

         stoppedState->assignProperty(statusButton,"backgroundColor",QColor(Qt::green));
         runningState->assignProperty(statusButton,"backgroundColor",QColor(Qt::red));

         auto stopToRun = new QEventTransition(statusButton,QEvent::MouseButtonRelease,stoppedState);
         auto runToStop = new QEventTransition(statusButton,QEvent::MouseButtonRelease,runningState);
         auto resetTransition = new QSignalTransition(this,&GraphicsScene::resetButtons,runningState);
         auto colorAnimation = new QPropertyAnimation(statusButton,"backgroundColor",stoppedState);

         colorAnimation->setDuration(1000);
         stopToRun->addAnimation(colorAnimation);
         runToStop->addAnimation(colorAnimation);
         resetTransition->addAnimation(colorAnimation);
         stopToRun->setTargetState(runningState);
         runToStop->setTargetState(stoppedState);
         resetTransition->setTargetState(stoppedState);

         stoppedState->addTransition(stopToRun);
         runningState->addTransition(runToStop);

         machine->setInitialState(stoppedState);
         machine->start();
}

void GraphicsScene::populateLegend(QWidget * parentWidget,QVBoxLayout * sideLayout) const {
         sideLayout->addSpacing(25);

         auto legendLabel = new QLabel("Legend:",parentWidget);
         sideLayout->addWidget(legendLabel);

         legendLabel->setObjectName("legendTitle");  // for css
         addShadowEffect(legendLabel);

         sideLayout->addLayout(getLegendLayout(parentWidget,"source"));
         sideLayout->addLayout(getLegendLayout(parentWidget,"target"));
         sideLayout->addLayout(getLegendLayout(parentWidget,"active"));
         sideLayout->addLayout(getLegendLayout(parentWidget,"inactive"));
         sideLayout->addLayout(getLegendLayout(parentWidget,"visited"));
         sideLayout->addLayout(getLegendLayout(parentWidget,"block"));
         sideLayout->addLayout(getLegendLayout(parentWidget,"inpath"));
}

void GraphicsScene::populateBottomLayout(QWidget * parentWidget,QGridLayout * mainLayout) const {
         auto infoLine = new QLineEdit("Click on run Button on sidebar to display algorithm status",parentWidget);
         infoLine->setAlignment(Qt::AlignCenter); 
         infoLine->setReadOnly(true);

         {        // hefty -  improve later
                  connect(this,&GraphicsScene::foundPath,[infoLine]{
                           auto currentText = infoLine->text();
                           QString digits;
                           for(auto character : currentText){
                                    if(character.isNumber()) digits += character;
                           }
                           infoLine->setText(QString("Final Distance : %1").arg(digits));
                  });
         }
         {
                  auto shadowEffect = new QGraphicsDropShadowEffect(infoLine);
                  shadowEffect->setBlurRadius(10);
                  shadowEffect->setOffset(2,2);
                  infoLine->setGraphicsEffect(shadowEffect);
         }

         auto slider = new QSlider(Qt::Horizontal,parentWidget);
         slider->setRange(0,1000);
         slider->setValue(925);
         slider->setTracking(true);

         connect(slider,&QSlider::valueChanged,this,&GraphicsScene::setDelay);
         connect(slider,&QSlider::valueChanged,this,&GraphicsScene::animationDurationChanged);

         auto bottomLayout = new QHBoxLayout();
         bottomLayout->setAlignment(Qt::AlignCenter);
         bottomLayout->addWidget(infoLine);
         bottomLayout->addSpacing(40);
         bottomLayout->addWidget(new QLabel("Speed: "));
         bottomLayout->addWidget(slider);
         mainLayout->addLayout(bottomLayout,1,0);
}

void GraphicsScene::setRunning(const bool newState){
         running = newState;

         if(running){ 
                  const int exception = bar->currentIndex();
                  disableBarTabs(exception);
         }else{
                  enableAllBarTabs();
         }
         emit runningStatusChanged(running); // connected with node class
}

std::pair<int,int> GraphicsScene::getRandomCord() const {
         std::random_device rd;  
         std::mt19937 gen(rd());
         std::uniform_int_distribution<int> rowRange(0,rowCnt-1);
         std::uniform_int_distribution<int> colRange(0,colCnt-1);
         return std::make_pair(rowRange(gen),colRange(gen));
}

void GraphicsScene::addShadowEffect(QLabel * label) const {
         auto shadowEffect = new QGraphicsDropShadowEffect(label);
         shadowEffect->setBlurRadius(4);// px
         shadowEffect->setOffset(0.5,0.5); // x,y px
         label->setGraphicsEffect(shadowEffect);
}

//! alternative required
QHBoxLayout * GraphicsScene::getLegendLayout(QWidget * parentWidget,QString token) const {
         const QString pattern = R"(:/pixmaps/icons/%1.png)";
         QString labelText = token;
         labelText[0] = labelText[0].toUpper();

         auto layout = new QHBoxLayout(); // parent layout deletes
         auto label = new QLabel(labelText,parentWidget);
         auto icon = new QLabel(parentWidget);

         layout->addWidget(icon);
         layout->addWidget(label);

         if(token == "visited"){
                  token = "inactive";
                  auto opacityEffect = new QGraphicsOpacityEffect(icon);
                  opacityEffect->setOpacity(0.5);
                  icon->setGraphicsEffect(opacityEffect);
         }else{
                  addShadowEffect(icon);
         }
         addShadowEffect(label);
         
         QPixmap pixmap(pattern.arg(token));
         icon->setPixmap(pixmap);
         assert(!icon->pixmap().isNull());

         return layout;
}

void GraphicsScene::disableBarTabs(const int exception) const {
         for(int index = 0;index < bar->count();index++){
                  if(index != exception){
                           bar->setTabEnabled(index,false);
                  }
         }
}

void GraphicsScene::enableAllBarTabs() const {
         for(int index = 0;index < bar->count();index++){
                  bar->setTabEnabled(index,true);
         }
}

// algorithm state
bool GraphicsScene::isRunning() const {
         return running;
}

void GraphicsScene::setDelay(const uint32_t newDelay){
         timerDelay = std::abs((int64_t)1000 - newDelay);
         setTimersIntervals(timerDelay);
}

void GraphicsScene::setTimersIntervals(const uint32_t newDelay) const {
         dfsTimer->setInterval(newDelay);
         bfsTimer->setInterval(newDelay);
         dijkstraTimer->setInterval(newDelay);
         pathTimer->setInterval(newDelay);
}

void GraphicsScene::stopTimers() const {
         dfsTimer->stop();
         bfsTimer->stop();
         dijkstraTimer->stop();
         pathTimer->stop();
}

Node * GraphicsScene::getNewNode(const int row,const int col){
         auto node = new Node(row,col);

         connect(this,&GraphicsScene::animationDurationChanged,node,&Node::changeAnimationDuration);

         connect(node,&Node::sourceSet,[this,node,row,col]{
                  sourceNodeCord = {row,col};
                  sourceNode = node;
         });

         connect(node,&Node::targetSet,[this,node,row,col]{
                  targetNodeCord = {row,col};
                  targetNode = node;
         });

         return node;
}

Node * GraphicsScene::getNodeAt(const int row,const int col) const {
         return static_cast<Node*>(innerLayout->itemAt(row,col));
}

// returns true of {row,col} is a valid cordinate for grid
bool GraphicsScene::validCordinate(const int row,const int col) const {
         return row >= 0 && row < rowCnt && col >= 0 && col < colCnt;
}

// returns the bottom lineEdit (statusBar)
QLineEdit * GraphicsScene::getStatusBar(const int tabIndex) const {
         assert(tabIndex < bar->count());
         auto widget = bar->widget(tabIndex);
         auto abstractLayout = static_cast<QGridLayout*>(widget->layout())->itemAtPosition(1,0);
         return static_cast<QLineEdit*>(static_cast<QHBoxLayout*>(abstractLayout)->itemAt(0)->widget());
}

bool GraphicsScene::isSpecial(Node * currentNode) const {
         return currentNode == sourceNode || currentNode == targetNode;
}

void GraphicsScene::updateSrcTarNodes() const {
         assert(sourceNode && targetNode);
         sourceNode->setType(Node::Source);
         targetNode->setType(Node::Target);
}

bool GraphicsScene::isBlock(Node * currentNode) const {
         return currentNode->getType() == Node::Block;
}

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
         sourceNodeCord = getRandomCord();
         targetNodeCord = getRandomCord();

         sourceNode = getNodeAt(sourceNodeCord.first,sourceNodeCord.second);
         targetNode = getNodeAt(targetNodeCord.first,targetNodeCord.second);
         updateSrcTarNodes();
}

void GraphicsScene::cleanup() const {
         for(int row = 0;row < rowCnt;row++){
                  for(int col = 0;col < colCnt;col++){
                           auto node = static_cast<Node*>(innerLayout->itemAt(row,col));
                           if(isBlock(node)) continue;
                           bool runAnimations = false;
                           node->setType(Node::Inactive,runAnimations);
                  }
         }
         updateSrcTarNodes();
}

// follows the parent pointer of target node until it reaches source
void GraphicsScene::pathConnect() const {
         auto moveUp = [this,currentNode = targetNode,newStart = true]() mutable {
                  if(newStart){
                           currentNode = targetNode;
                  }
                  newStart = false;

                  if(!currentNode){
                           newStart = true;
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

void GraphicsScene::bfsStart(const bool newStart) const {
         if(newStart){
                  queue->push({sourceNode,0});
                  auto [startX,startY] = sourceNode->getCord();
                  (*visited)[startX][startY] = true;
         }
         bfsTimer->start();
}

void GraphicsScene::dfsStart(const bool newStart) const {
         if(newStart){
                  stack->push({sourceNode,0});
                  auto [startX,startY] = sourceNode->getCord();
                  (*visited)[startX][startY] = true;
         }
         dfsTimer->start();
}

void GraphicsScene::dijkstraStart(const bool newStart) const {
         if(newStart){
                  pq->push({0,sourceNode});
                  auto [startX,startY] = sourceNode->getCord();
                  (*distance)[startX][startY] = 0;
         }
         dijkstraTimer->start();
}

// tab index : 0
void GraphicsScene::bfsConnect() const {
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
                           emit foundPath();
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

         connect(bfsTimer,&QTimer::timeout,implementation);
}

// tab index : 1
void GraphicsScene::dfsConnect() const {
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
                           emit foundPath();
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

         connect(dfsTimer,&QTimer::timeout,implementation);
}

// tab index : 2
void GraphicsScene::dijkstraConnect() const {
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
                           emit foundPath();
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

         connect(dijkstraTimer,&QTimer::timeout,implementation);
}
