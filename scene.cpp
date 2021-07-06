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
#include "scene.hpp"
#include "pushButton.hpp"
#include "node.hpp"
#include "helpDialog.hpp"

namespace{
         #include "defines.hpp"
}

// for inner scene
constexpr int rowCnt = 10,colCnt = 20;
constexpr uint32_t defDelay = 100; // ms
constexpr int xCord[] {-1,1,0,0};
constexpr int yCord[] {0,0,1,-1};

GraphicsScene::GraphicsScene(const QSize & size) : timerDelay(defDelay){
         innerScene = new QGraphicsScene(this);
         helpWidget = new StackedWidget();
         bar = new QTabWidget();
         bfsTimer = new QTimer(bar);
         dfsTimer = new QTimer(bar);
         dijkstraTimer = new QTimer(bar);
         pathTimer = new QTimer(bar);

         bar->setTabShape(QTabWidget::TabShape::Triangular); //?

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
                  populateWidget(bfsWidget,algoName,::bfsInfo /*inside defines header*/ );
         }
         {        // dfs
                  auto dfsWidget = new QWidget(bar);
                  const QString algoName = "DFS";
                  bar->addTab(dfsWidget,algoName);
                  populateWidget(dfsWidget,algoName,::dfsInfo /*inside defines header*/ );
         }
         {        // dijistra 
                  auto dijkstraWidget = new QWidget(bar);
                  const QString algoName = "Dijkstra";
                  bar->addTab(dijkstraWidget,algoName);
                  populateWidget(dijkstraWidget,algoName,::dijkstraInfo /*inside defines header*/ );
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
         auto infobutton = new PushButton("Information",parentWidget);
         auto statusbutton = new PushButton("Run",parentWidget);
         auto resetbutton = new PushButton("Reset",parentWidget);
         auto randombutton = new PushButton("Random",parentWidget);
         auto helpbutton = new PushButton("Help",parentWidget);
         auto exitbutton = new PushButton("Exit",parentWidget);

         sideLayout->addWidget(infobutton);
         sideLayout->addWidget(statusbutton);
         sideLayout->addWidget(resetbutton);
         sideLayout->addWidget(randombutton);
         sideLayout->addWidget(helpbutton);
         sideLayout->addWidget(exitbutton);
         sideLayout->insertSpacing(4,25);

         configureMachine(parentWidget,statusbutton);

         connect(infobutton,&QPushButton::released,[algoName,infoText]{
                  QMessageBox::information(nullptr,algoName,infoText);
         });

         // emitted when an algorithm finishes
         connect(this,&GraphicsScene::resetButtons,statusbutton,[this,statusbutton]{
                  statusbutton->setText("Run");
                  memsetDs();
         });

         connect(statusbutton,&QPushButton::released,statusbutton,[this,statusbutton]{
                  const QString & currentText = statusbutton->text();
                  bool newStart = false;

                  if(currentText == "Stop"){
                           statusbutton->setText("Continue");
                           stopTimers();
                           setRunning(false);
                           return;
                  }else if(currentText == "Run"){
                           cleanup(); 
                           statusbutton->setText("Stop");
                           newStart = true;
                  }else{
                           statusbutton->setText("Stop");
                  }

                  switch(bar->currentIndex()){
                           case 0 : bfsStart(newStart);break;
                           case 1 : dfsStart(newStart);break;
                           case 2 : dijkstraStart(newStart);break;
                           default : assert(false);
                  }
         });

         connect(resetbutton,&QPushButton::released,statusbutton,[this,statusbutton]{
                  statusbutton->setText("Run");
                  resetGrid();
         });

         connect(randombutton,&PushButton::released,[this]{
                  resetGrid();
                  generateRandGridPattern();
         });
         
         connect(helpbutton,&PushButton::released,&*helpWidget,&QStackedWidget::show);

         connect(exitbutton,&QPushButton::released,this,[this]{
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
         lineInfo->setText("Click on run button on sidebar to display algorithm status");
}

void GraphicsScene::generateRandGridPattern(){
         startCord = getRandomCord();
         while((endCord = getRandomCord()) == startCord){
                  qInfo() << "what";
         }

         sourceNode->setType(Node::Inactive);
         targetNode->setType(Node::Inactive);
         sourceNode = getNodeAt(startCord.first,startCord.second);
         targetNode = getNodeAt(endCord.first,endCord.second);
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

// manager custom properties - currently just statusbutton
void GraphicsScene::configureMachine(QWidget * parentWidget,QPushButton * statusbutton){
         auto machine = new QStateMachine(parentWidget);
         auto statusStart = new QState(machine);
         auto statusEnd = new QState(machine);

         statusStart->assignProperty(this,"runningState",false);
         statusEnd->assignProperty(this,"runningState",true);

         statusStart->assignProperty(statusbutton,"backgroundColor",QColor(Qt::green));
         statusEnd->assignProperty(statusbutton,"backgroundColor",QColor(Qt::red));

         auto startToEnd = new QEventTransition(statusbutton,QEvent::MouseButtonRelease,machine);
         auto endToStart = new QEventTransition(statusbutton,QEvent::MouseButtonRelease,machine);
         auto endedTransition = new QSignalTransition(this,&GraphicsScene::resetButtons,statusEnd); //! memory leak
         auto colorAnimation = new QPropertyAnimation(statusbutton,"backgroundColor",parentWidget);

         colorAnimation->setDuration(1000);
         startToEnd->addAnimation(colorAnimation);
         endToStart->addAnimation(colorAnimation);
         startToEnd->setTargetState(statusEnd);
         endToStart->setTargetState(statusStart);
         endedTransition->setTargetState(statusStart);
         endedTransition->addAnimation(colorAnimation);

         statusStart->addTransition(startToEnd);
         statusEnd->addTransition(endToStart);

         machine->setInitialState(statusStart);
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
         auto infoLine = new QLineEdit("Click on run button on sidebar to display algorithm status",parentWidget);
         infoLine->setAlignment(Qt::AlignCenter); // text align
         infoLine->setReadOnly(true);

         auto slider = new QSlider(Qt::Horizontal,parentWidget);
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

/// utility ///

void GraphicsScene::setRunning(const bool newState){
         runningState = newState;

         if(runningState){
                  const int exception = bar->currentIndex();
                  disableBarTabs(exception);
         }else{
                  enableAllBarTabs();
         }
         emit runningStatusChanged(runningState); // connected with node class
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

//! alternative later
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
                  addShadowEffect(label);

                  auto opacityEffect = new QGraphicsOpacityEffect(icon);
                  opacityEffect->setOpacity(0.5);
                  icon->setGraphicsEffect(opacityEffect);
         }else{
                  addShadowEffect(icon);
         }
         
         QPixmap pixmap(pattern.arg(token));
         icon->setPixmap(pixmap);
         assert(!icon->pixmap().isNull());

         return layout;
}

void GraphicsScene::disableBarTabs(const int exception) const {
         const int tabCount = bar->count();
         for(int index = 0;index < tabCount;index++){
                  if(index != exception){
                           bar->setTabEnabled(index,false);
                  }
         }
}

void GraphicsScene::enableAllBarTabs() const {
         const int tabCount = bar->count();
         for(int index = 0;index < tabCount;index++){
                  bar->setTabEnabled(index,true);
         }
}

// algorithm state
bool GraphicsScene::isRunning() const {
         return runningState;
}

void GraphicsScene::setDelay(const uint32_t newDelay){
         // slide bar has more value when it is set on the fuller side convert before
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

         connect(node,&Node::sourceSet,[this,node,row,col]{
                  startCord = {row,col};
                  sourceNode = node;
         });

         connect(node,&Node::targetSet,[this,node,row,col]{
                  endCord = {row,col};
                  targetNode = node;
         });

         return node;
}

// returns the node ref present inside the grid
Node * GraphicsScene::getNodeAt(const int row,const int col) const {
         return static_cast<Node*>(innerLayout->itemAt(row,col));
}

// returns true of {row,col} is a valid cordinate for grid
bool GraphicsScene::validCordinate(const int row,const int col) const {
         return row >= 0 && row < rowCnt && col >= 0 && col < colCnt;
}

// returns the bottom lineEdit which displays the current algorithm state
QLineEdit * GraphicsScene::getStatusBar(const int tabIndex) const {
         assert(tabIndex < bar->count());
         auto widget = bar->widget(tabIndex);
         auto absLayout = static_cast<QGridLayout*>(widget->layout())->itemAtPosition(1,0);
         return static_cast<QLineEdit*>(static_cast<QHBoxLayout*>(absLayout)->itemAt(0)->widget());
}

// returns true if current node is either sourceNode or targetNode - more status in update
bool GraphicsScene::isSpecial(Node * currentNode) const {
         return currentNode == sourceNode || currentNode == targetNode;
}

// sets the type - used in cleanup && reset button
void GraphicsScene::updateSrcTarNodes() const {
         assert(sourceNode && targetNode);
         sourceNode->setType(Node::Source);
         targetNode->setType(Node::Target);
}

// checks whether the algorithm can proceed through the current node
bool GraphicsScene::isBlock(Node * currentNode) const {
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
         startCord = getRandomCord();
         endCord = getRandomCord();

         sourceNode = getNodeAt(startCord.first,startCord.second);
         targetNode = getNodeAt(endCord.first,endCord.second);
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
/// implementations - gets called when statusbutton is clicked ///

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

// called when an algorithm succeeds
void GraphicsScene::getPath() const {
         pathTimer->start();
}

// called when start is pressed on bfs Tab
void GraphicsScene::bfsStart(const bool newStart) const {
         if(newStart){
                  queue->push({sourceNode,0});
                  auto [startX,startY] = sourceNode->getCord();
                  (*visited)[startX][startY] = true;
         }
         bfsTimer->start();
}

// called when start is pressed on dfs tab
void GraphicsScene::dfsStart(const bool newStart) const {
         if(newStart){
                  stack->push({sourceNode,0});
                  auto [startX,startY] = sourceNode->getCord();
                  (*visited)[startX][startY] = true;
         }
         dfsTimer->start();
}

// called when start is pressed on dijistraTab
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

         connect(bfsTimer,&QTimer::timeout,implementation);
}

// tab index : 1
// connects dfsTimer with the 'implementation' lambda
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

         connect(dfsTimer,&QTimer::timeout,implementation);
}

// tab index : 2
// connects dijistraTimer with 'implementation' lambda
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

         connect(dijkstraTimer,&QTimer::timeout,implementation);
}
