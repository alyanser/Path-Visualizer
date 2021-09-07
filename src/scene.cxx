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
#include <QStateMachine>
#include <QState>
#include <stack>
#include <queue>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QTabBar>
#include <QIcon>
#include "scene.hpp"
#include "pushButton.hpp"
#include "node.hpp"
#include "helpDialog.hpp"
#include "defines.hpp"

GraphicsScene::GraphicsScene(const QSize size) : windowSize(size), helpDialogWidget(std::make_unique<StackedWidget>(windowSize)){
         populateBar();
         setTimersIntervals(static_cast<std::chrono::milliseconds>(m_timerDelay));
         configureInnerScene();
         connectPaths();
         setMainSceneConnections();
}

void GraphicsScene::setMainSceneConnections() const noexcept {
         QTimer::singleShot(1009,helpDialogWidget.get(),&StackedWidget::show);
         connect(this,&GraphicsScene::runningStatusChanged,&Node::setRunningState);
         connect(this,SIGNAL(foundPath()),pathTimer.get(),SLOT(start()));
}

void GraphicsScene::connectPaths() const noexcept {
         bfsConnect();
         dfsConnect();
         dijkstraConnect();
         pathConnect();
}

void GraphicsScene::configureInnerScene() noexcept {
         populateGridScene();
         allocDataStructures();
         memsetDs();
}

void GraphicsScene::populateBar() noexcept {
         m_bar = std::make_unique<QTabWidget>();
         addWidget(m_bar.get());
         m_bar->setFixedSize(windowSize);

         {        
                  auto * bfsWidget = new QWidget(m_bar.get());
                  const QString algorithmName = "BFS";
                  m_bar->addTab(bfsWidget,algorithmName);
                  populateWidget(bfsWidget,algorithmName,::bfsInfo);
         }
         {        
                  auto * dfsWidget = new QWidget(m_bar.get());
                  const QString algorithmName = "DFS";
                  m_bar->addTab(dfsWidget,algorithmName);
                  populateWidget(dfsWidget,algorithmName,::dfsInfo);
         }
         {        
                  auto * dijkstraWidget = new QWidget(m_bar.get());
                  const QString algorithmName = "Dijkstra";
                  m_bar->addTab(dijkstraWidget,algorithmName);
                  populateWidget(dijkstraWidget,algorithmName,::dijkstraInfo);
         }
}

void GraphicsScene::allocDataStructures() noexcept {
         m_queue = std::make_unique<std::queue<std::pair<Node *,uint32_t>>>(); // {:m_distance}
         m_stack = std::make_unique<std::stack<std::pair<Node *,uint32_t>>>(); // {:m_distance}
         m_visited = std::make_unique<std::vector<std::vector<bool>>>();
         m_distance = std::make_unique<std::vector<std::vector<uint32_t>>>();
         m_priorityQueue = std::make_unique<std::priority_queue<pIntNode,std::vector<pIntNode>,std::greater<>>>(); // {m_distance:}
}

void GraphicsScene::memsetDs() const noexcept {
         while(!m_queue->empty()) m_queue->pop();
         while(!m_stack->empty()) m_stack->pop();
         while(!m_priorityQueue->empty()) m_priorityQueue->pop();

         m_distance->assign(RowCnt,std::vector<uint32_t>(ColCnt,std::numeric_limits<uint32_t>::max()));
         m_visited->assign(RowCnt,std::vector<bool>(ColCnt,false));
}

void GraphicsScene::populateWidget(QWidget * holder,const QString & algorithmName,const QString & infoText) noexcept {
         auto * mainLayout = new QGridLayout(holder);
         mainLayout->setSpacing(10);

         auto * view = new QGraphicsView(innerScene,holder);
         view->setMaximumHeight(windowSize.height() + Yoffset);
         mainLayout->setAlignment(Qt::AlignTop);
         mainLayout->addWidget(view,0,0);

         auto * sideLayout = new QVBoxLayout(); 
         sideLayout->setSpacing(5);
         mainLayout->addLayout(sideLayout,0,1);
         sideLayout->setAlignment(Qt::AlignTop);

         populateSideLayout(holder,sideLayout,algorithmName,infoText);

         populateLegend(holder,sideLayout);
         populateBottomLayout(holder,mainLayout);
}

void GraphicsScene::populateSideLayout(QWidget * holder,QVBoxLayout * sideLayout,const QString & algorithmName,const QString & info) noexcept {
         auto * infoButton = new PushButton("Information",holder);
         auto * statusButton = new PushButton("Run",holder);
         auto * resetButton = new PushButton("Reset",holder);
         auto * randomButton = new PushButton("Random",holder);
         auto * helpButton = new PushButton("Help",holder);
         auto * exitButton = new PushButton("Exit",holder);

         sideLayout->addWidget(infoButton);
         sideLayout->addWidget(statusButton);
         sideLayout->addWidget(resetButton);
         sideLayout->addWidget(randomButton);
         sideLayout->addWidget(helpButton);
         sideLayout->addWidget(exitButton);
         sideLayout->insertSpacing(4,25);

         configureMachine(holder,statusButton);

         connect(infoButton,&QPushButton::released,[algorithmName,info]{
                  QMessageBox::information(nullptr,algorithmName,info);
         });

         connect(this,&GraphicsScene::resetButtons,statusButton,[this,statusButton]{
                  statusButton->setText("Run");
                  memsetDs();
         });

         connect(statusButton,&QPushButton::released,statusButton,[this,statusButton]{
                  // statemachine would have already changed the state - @configureMachine
                  if(m_running){
                           bool toStartNew = statusButton->text() == "Run";  // else continue
                           statusButton->setText("Stop");

                           if(toStartNew){
                                    cleanup();
                                    memsetDs();
                           }
                           switch(m_bar->currentIndex()){
                                    case BfsIndex : bfsStart(toStartNew); break;
                                    case DfsIndex : dfsStart(toStartNew); break;
                                    case DijkstraIndex : dijkstraStart(toStartNew); break;
                                    default : __builtin_unreachable();
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
         
         connect(helpButton,&PushButton::released,helpDialogWidget.get(),&QStackedWidget::show);

         connect(exitButton,&QPushButton::released,this,[this]{
                  //? weird bug when set some widget as parent 
                  auto choice = QMessageBox::critical(nullptr,"Close","Quit",QMessageBox::No,QMessageBox::Yes);
                  if(choice == QMessageBox::Yes){
                           emit close();
                  }
         });
}

void GraphicsScene::resetGrid() const noexcept {
         stopTimers();
         emit resetButtons();
         memsetDs();

         for(size_t row = 0;row < RowCnt;row++){
                  for(size_t col = 0;col < ColCnt;col++){
                           auto * node = getNodeAt(row,col);
                           if(isSpecial(node)) continue;
                           node->setPathParent(nullptr);
                           constexpr bool runAnimations = false;
                           node->setType(Node::State::Inactive,runAnimations);
                  }
         }
         
         const auto curTabIndex = static_cast<uint32_t>(m_bar->currentIndex());
         auto * lineInfo = getStatusBar(curTabIndex);
         lineInfo->setText("Click on run Button on sidem_bar to display algorithm status");
}

void GraphicsScene::generateRandGridPattern() noexcept {
         m_sourceNodeCord = getRandomCord();
         while((m_targetNodeCord = getRandomCord()) == m_sourceNodeCord);

         m_sourceNode->setType(Node::State::Inactive);
         m_targetNode->setType(Node::State::Inactive);

         const auto [sourceX,sourceY] = m_sourceNodeCord;
         const auto [targetX,targetY] = m_targetNodeCord;

         m_sourceNode = getNodeAt(sourceX,sourceY);
         m_targetNode = getNodeAt(targetX,targetY);

         updateSourceTargetNodes();

         for(int32_t placed = 0;placed < MaximumBlocks;){
                  auto [randX,randY] = getRandomCord();
                  auto node = getNodeAt(randX,randY);

                  if(!isSpecial(node) && m_binaryDist(m_generator)){
                           node->setType(Node::State::Block);
                           placed++;
                  }
         }
}

void GraphicsScene::configureMachine(QWidget * holder,QPushButton * statusButton) noexcept {
         auto * machine = new QStateMachine(holder);
         auto * stoppedState = new QState(machine);
         auto * runningState = new QState(machine);

         stoppedState->assignProperty(this,"running",false);
         runningState->assignProperty(this,"running",true);

         stoppedState->assignProperty(statusButton,"backgroundColor",QColor(Qt::green));
         runningState->assignProperty(statusButton,"backgroundColor",QColor(Qt::red));

         auto * stopToRun = new QEventTransition(statusButton,QEvent::MouseButtonRelease,stoppedState);
         auto * runToStop = new QEventTransition(statusButton,QEvent::MouseButtonRelease,runningState);
         auto * resetTransition = new QSignalTransition(this,&GraphicsScene::resetButtons,runningState);
         auto * colorAnimation = new QPropertyAnimation(statusButton,"backgroundColor",stoppedState);

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

void GraphicsScene::populateLegend(QWidget * holder,QVBoxLayout * sideLayout) const noexcept {
         sideLayout->addSpacing(25);

         auto * legendLabel = new QLabel("Legend:",holder);
         sideLayout->addWidget(legendLabel);

         legendLabel->setObjectName("legendTitle");  // for css
         addShadowEffect(legendLabel);

         sideLayout->addLayout(getLegendLayout(holder,"source"));
         sideLayout->addLayout(getLegendLayout(holder,"target"));
         sideLayout->addLayout(getLegendLayout(holder,"active"));
         sideLayout->addLayout(getLegendLayout(holder,"inactive"));
         sideLayout->addLayout(getLegendLayout(holder,"visited"));
         sideLayout->addLayout(getLegendLayout(holder,"block"));
         sideLayout->addLayout(getLegendLayout(holder,"inpath"));
}

void GraphicsScene::populateBottomLayout(QWidget * holder,QGridLayout * mainLayout) const noexcept {
         auto * infoLine = new QLineEdit("Click on run Button on sidem_bar to display algorithm status",holder);
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
                  auto * shadowEffect = new QGraphicsDropShadowEffect(infoLine);
                  shadowEffect->setBlurRadius(10);
                  shadowEffect->setOffset(2,2);
                  infoLine->setGraphicsEffect(shadowEffect);
         }

         auto * slider = new QSlider(Qt::Horizontal,holder);
         slider->setRange(0,1000);
         slider->setValue(925);
         slider->setTracking(true);

         auto * bottomLayout = new QHBoxLayout();
         bottomLayout->setAlignment(Qt::AlignCenter);
         bottomLayout->addWidget(infoLine);
         bottomLayout->addSpacing(40);
         bottomLayout->addWidget(new QLabel("Speed: "));
         bottomLayout->addWidget(slider);
         mainLayout->addLayout(bottomLayout,1,0);

         connect(slider,&QSlider::valueChanged,this,&GraphicsScene::setDelay);
         connect(slider,&QSlider::valueChanged,this,&GraphicsScene::animationDurationChanged);
}

void GraphicsScene::setRunning(const bool newState) noexcept {
         m_running = newState;

         if(m_running){ 
                  const int32_t exception = m_bar->currentIndex();
                  disableBarTabs(exception);
         }else{
                  enableAllBarTabs();
         }

         emit runningStatusChanged(m_running); // connected with node class
}

std::pair<size_t,size_t> GraphicsScene::getRandomCord() noexcept {
         return std::make_pair(m_rowRange(m_generator),m_colRange(m_generator));
}

void GraphicsScene::addShadowEffect(QLabel * label) noexcept {
         auto shadowEffect = new QGraphicsDropShadowEffect(label);
         shadowEffect->setBlurRadius(4);// px
         shadowEffect->setOffset(0.5,0.5); // x,y px
         label->setGraphicsEffect(shadowEffect);
}

QHBoxLayout * GraphicsScene::getLegendLayout(QWidget * holder,QString token) const noexcept {
         const QString pattern = R"(:/pixmaps/icons/%1.png)";
         QString labelText = token;
         labelText[0] = labelText[0].toUpper();

         auto * layout = new QHBoxLayout();
         auto * label = new QLabel(labelText,holder);
         auto * icon = new QLabel(holder);

         layout->addWidget(icon);
         layout->addWidget(label);

         if(token == "m_visited"){
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
         return layout;
}

void GraphicsScene::disableBarTabs(const int32_t exception) const noexcept {
         for(int32_t index = 0;index < m_bar->count();index++){
                  if(index != exception){
                           m_bar->setTabEnabled(index,false);
                  }
         }
}

void GraphicsScene::enableAllBarTabs() const noexcept {
         for(int32_t index = 0;index < m_bar->count();index++){
                  m_bar->setTabEnabled(index,true);
         }
}

bool GraphicsScene::isRunning() const noexcept {
         return m_running;
}

void GraphicsScene::setDelay(const uint32_t newDelay) noexcept {
         m_timerDelay = static_cast<uint32_t>(std::abs(static_cast<int64_t>(1000) - newDelay));
         setTimersIntervals(static_cast<std::chrono::milliseconds>(m_timerDelay));
}

void GraphicsScene::setTimersIntervals(const std::chrono::milliseconds newDelay) const noexcept {
         dfsTimer->setInterval(newDelay);
         bfsTimer->setInterval(newDelay);
         dijkstraTimer->setInterval(newDelay);
         pathTimer->setInterval(newDelay);
}

void GraphicsScene::stopTimers() const noexcept {
         dfsTimer->stop();
         bfsTimer->stop();
         dijkstraTimer->stop();
         pathTimer->stop();
}

Node * GraphicsScene::getNewNode(const size_t row,const size_t col) noexcept {
         auto * node = new Node(static_cast<uint32_t>(row),static_cast<uint32_t>(col));

         connect(this,&GraphicsScene::animationDurationChanged,node,&Node::changeAnimationDuration);

         connect(node,&Node::sourceSet,[this,node,row,col]{
                  m_sourceNodeCord = {row,col};
                  m_sourceNode = node;
         });

         connect(node,&Node::targetSet,[this,node,row,col]{
                  m_targetNodeCord = {row,col};
                  m_targetNode = node;
         });

         return node;
}

Node * GraphicsScene::getNodeAt(const size_t row,const size_t col) const noexcept {
         if(auto * node = dynamic_cast<Node*>(m_innerLayout->itemAt(static_cast<int>(row),static_cast<int>(col)))){
                  return node;
         }

         assert(!static_cast<bool>("Downcast failed"));
         return nullptr;
}

bool GraphicsScene::validCordinate(const ptrdiff_t row,const ptrdiff_t col) noexcept {
         return row >= 0 && static_cast<size_t>(row) < RowCnt && col >= 0 && static_cast<size_t>(col) < ColCnt;
}

QLineEdit * GraphicsScene::getStatusBar(const uint32_t tabIndex) const noexcept {
         assert(tabIndex < static_cast<size_t>(m_bar->count())); 

         auto widget = m_bar->widget(static_cast<int32_t>(tabIndex));
         auto abstractLayout = static_cast<QGridLayout*>(widget->layout())->itemAtPosition(1,0);
         return static_cast<QLineEdit*>(static_cast<QHBoxLayout*>(abstractLayout)->itemAt(0)->widget());
}

bool GraphicsScene::isSpecial(Node * currentNode) const noexcept {
         return currentNode == m_sourceNode || currentNode == m_targetNode;
}

void GraphicsScene::updateSourceTargetNodes() const noexcept {
         assert(m_sourceNode && m_targetNode);

         m_sourceNode->setType(Node::State::Source);
         m_targetNode->setType(Node::State::Target);
}

bool GraphicsScene::isBlock(Node * currentNode) const noexcept {
         return currentNode->getType() == Node::State::Block;
}

void GraphicsScene::populateGridScene() noexcept {
         auto * holder = new QGraphicsWidget(); 
         m_innerLayout = new QGraphicsGridLayout(holder);

         holder->setLayout(m_innerLayout);
         innerScene->addItem(holder);
         m_innerLayout->setSpacing(25);

         for(size_t row = 0;row < RowCnt;row++){
                  for(size_t col = 0;col < ColCnt;col++){
                           auto * node = getNewNode(row,col);
                           m_innerLayout->addItem(node,static_cast<int32_t>(row),static_cast<int32_t>(col));
                  }
         }

         m_sourceNodeCord = getRandomCord();
         m_targetNodeCord = getRandomCord();

         const auto [sourceX,sourceY] = m_sourceNodeCord;
         const auto [targetX,targetY] = m_targetNodeCord;

         m_sourceNode = getNodeAt(sourceX,sourceY);
         m_targetNode = getNodeAt(targetX,targetY);

         updateSourceTargetNodes();
}

void GraphicsScene::cleanup() const noexcept {
         for(size_t row = 0;row < RowCnt;row++){
                  for(size_t col = 0;col < ColCnt;col++){
                           auto * node = getNodeAt(row,col);
                           if(isBlock(node)) continue;
                           bool runAnimations = false;
                           node->setType(Node::State::Inactive,runAnimations);
                  }
         }

         updateSourceTargetNodes();
}

void GraphicsScene::pathConnect() const noexcept {

         auto moveUp = [this,currentNode = m_targetNode,newStart = true]() mutable {
                  if(newStart){
                           currentNode = m_targetNode;
                  }

                  newStart = false;

                  if(!currentNode){
                           newStart = true;
                           return void(pathTimer->stop());
                  }else if(!isSpecial(currentNode)){
                           currentNode->setType(Node::State::Inpath);
                  }

                  currentNode = currentNode->getPathParent();
         };

         connect(pathTimer.get(),&QTimer::timeout,m_targetNode,moveUp,Qt::UniqueConnection);
}

void GraphicsScene::bfsStart(const bool newStart) const noexcept {
         if(newStart){
                  m_queue->push({m_sourceNode,0});
                  auto [startX,startY] = m_sourceNode->getCord();
                  (*m_visited)[startX][startY] = true;
         }

         bfsTimer->start();
}

void GraphicsScene::dfsStart(const bool newStart) const noexcept {
         if(newStart){
                  m_stack->push({m_sourceNode,0});
                  auto [startX,startY] = m_sourceNode->getCord();
                  (*m_visited)[startX][startY] = true;
         }

         dfsTimer->start();
}

void GraphicsScene::dijkstraStart(const bool newStart) const noexcept {
         if(newStart){
                  m_priorityQueue->push({0,m_sourceNode});
                  auto [startX,startY] = m_sourceNode->getCord();
                  (*m_distance)[startX][startY] = 0;
         }

         dijkstraTimer->start();
}

void GraphicsScene::bfsConnect() const noexcept {

         auto implementation = [this,infoLine = getStatusBar(BfsIndex)]{
                  if(m_queue->empty()){
                           bfsTimer->stop();
                           infoLine->setText("Could not reach destination.");
                           emit resetButtons();
                           return;
                  }

                  auto [currentNode,currentDistance] = m_queue->front();
                  m_queue->pop();
                  auto [curX,curY] = currentNode->getCord();

                  if(!isSpecial(currentNode)){
                           currentNode->setType(Node::State::Active);
                  }

                  auto nodeParent = currentNode->getPathParent();

                  if(nodeParent && !isSpecial(nodeParent)){
                           nodeParent->setType(Node::State::Visited);
                  }

                  infoLine->setText(QString("Current Distance : %1").arg(currentDistance));

                  if(currentNode == m_targetNode){
                           bfsTimer->stop();
                           emit foundPath();
                           emit resetButtons();
                           return;
                  }

                  for(uint32_t direction = 0;direction < 4;direction++){
                           const auto toRow = static_cast<ptrdiff_t>(curX) + m_xCord[direction];
                           const auto toCol = static_cast<ptrdiff_t>(curY) + m_yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    const auto validRow = static_cast<size_t>(toRow);
                                    const auto validCol = static_cast<size_t>(toCol); 

                                    auto * togoNode = getNodeAt(validRow,validCol);

                                    if(isBlock(togoNode) || (*m_visited)[validRow][validCol]) continue;

                                    (*m_visited)[validRow][validCol] = true;
                                    togoNode->setPathParent(currentNode);
                                    m_queue->push({togoNode,currentDistance+1});
                           }
                  }
         };

         connect(bfsTimer.get(),&QTimer::timeout,implementation);
}

void GraphicsScene::dfsConnect() const noexcept {

         auto implementation = [this,infoLine = getStatusBar(DfsIndex)]{
                  if(m_stack->empty()){
                           dfsTimer->stop();
                           infoLine->setText("Could not reach destination.");
                           emit resetButtons();
                           return;
                  }

                  auto [currentNode,currentDistance] = m_stack->top();
                  m_stack->pop();
                  infoLine->setText(QString("Current Distance : %1").arg(currentDistance));

                  if(!isSpecial(currentNode)){
                           currentNode->setType(Node::State::Active);
                  }

                  if(currentNode == m_targetNode){
                           dfsTimer->stop();
                           emit foundPath();
                           emit resetButtons();
                           return;
                  }

                  auto [curX,curY] = currentNode->getCord();
                  auto * nodeParent = currentNode->getPathParent();

                  if(nodeParent && !isSpecial(nodeParent)){
                           nodeParent->setType(Node::State::Visited);
                  }

                  for(uint32_t direction = 0;direction < 4;direction++){
                           const auto toRow = static_cast<ptrdiff_t>(curX) + m_xCord[direction];
                           const auto toCol = static_cast<ptrdiff_t>(curY) + m_yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    const auto validRow = static_cast<size_t>(toRow);
                                    const auto validCol = static_cast<size_t>(toCol); 
                                    auto * togoNode = getNodeAt(validRow,validCol);

                                    if(isBlock(togoNode) || (*m_visited)[validRow][validCol]) continue;

                                    (*m_visited)[validRow][validCol] = true;
                                    togoNode->setPathParent(currentNode);
                                    m_stack->push({togoNode,currentDistance+1});
                           }
                  }
         };

         connect(dfsTimer.get(),&QTimer::timeout,implementation);
}

void GraphicsScene::dijkstraConnect() const noexcept {

         auto implementation = [this,infoLine = getStatusBar(DijkstraIndex)]{
                  if(m_priorityQueue->empty()){
                           dijkstraTimer->stop();
                           infoLine->setText("Could not reach destination.");
                           emit resetButtons();
                           return;
                  }

                  auto [currentDistance,currentNode] = m_priorityQueue->top();
                  m_priorityQueue->pop();
                  auto [curX,curY] = currentNode->getCord();

                  if((*m_distance)[curX][curY] != currentDistance) return;

                  if(!isSpecial(currentNode)){
                           currentNode->setType(Node::State::Active);
                  }
                  auto nodeParent = currentNode->getPathParent();

                  if(nodeParent && !isSpecial(nodeParent)){
                           nodeParent->setType(Node::State::Visited);
                  }

                  infoLine->setText(QString("Current Distance: %1").arg(currentDistance));

                  if(currentNode == m_targetNode){
                           dijkstraTimer->stop();
                           emit foundPath();
                           emit resetButtons();
                           return;
                  }

                  for(uint32_t direction = 0;direction < 4;direction++){
                           const auto toRow = static_cast<ptrdiff_t>(curX) + m_xCord[direction];
                           const auto toCol = static_cast<ptrdiff_t>(curY) + m_yCord[direction];

                           if(validCordinate(toRow,toCol)){
                                    const auto validRow = static_cast<size_t>(toRow);
                                    const auto validCol = static_cast<size_t>(toCol); 
                                    auto * togoNode = getNodeAt(validRow,validCol);

                                    if(isBlock(togoNode)) continue;

                                    uint32_t & destDistance = (*m_distance)[validRow][validCol];
                                    const auto newDistance = currentDistance + 1;

                                    if(newDistance < destDistance){
                                             destDistance = newDistance;
                                             togoNode->setPathParent(currentNode);
                                             m_priorityQueue->push({newDistance,togoNode});
                                    }
                           }
                  }
         };

         connect(dijkstraTimer.get(),&QTimer::timeout,implementation);
}