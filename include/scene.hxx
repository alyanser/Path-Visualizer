#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>
#include <queue>
#include <stack>
#include <random>
#include <QTimer>
#include <QTabWidget>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsGridLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <node.hxx>
#include <helpDialog.hxx>

class QTabWidget;
class QSize;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QPushButton;

class GraphicsScene : public QGraphicsScene {
         Q_OBJECT
         Q_PROPERTY(bool running WRITE setRunning READ isRunning)

         enum class TabIndex { Bfs, Dfs, Dijkstra };

         using pIntNode = std::pair<uint32_t,Node*>;
public:
         explicit GraphicsScene(QSize size);
         GraphicsScene() = default;
         GraphicsScene(const GraphicsScene & other) = delete;
         GraphicsScene(GraphicsScene && other) = delete;
         GraphicsScene & operator = (const GraphicsScene & other) = delete;
         GraphicsScene & operator = (GraphicsScene && other) = delete;
private:
         void populateBar() noexcept;
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText) noexcept;
         void populateGridScene() noexcept;  
         void populateLegend(QWidget * parentWidget,QVBoxLayout * sideLayout) const noexcept;
         void populateBottomLayout(QWidget * parentWidget,QGridLayout * mainLayout) const noexcept;
         void populateSideLayout(QWidget * parent,QVBoxLayout * sideLayout,const QString & algoName,const QString & infoText) noexcept;
         void configureMachine(QWidget * parentWidget,QPushButton * statusButton) noexcept;
         void setMainSceneConnections() const noexcept;
         void connectPaths() const noexcept;
         void configureInnerScene() noexcept;
         void generateRandGridPattern() noexcept;
         void allocDataStructures() noexcept;
         void setRunning(bool newState) noexcept;
         void dfsImplementation() noexcept;
         void bfsImplementation() noexcept;
         void dijkstraImplementation() noexcept;
         void pathConnect() const noexcept;
         [[nodiscard]] bool isRunning() const noexcept; 
         void cleanup() const noexcept; 
         void resetGrid() const noexcept;
         void updateSourceTargetNodes() const noexcept;
         [[nodiscard]] Node * getNewNode(size_t row,size_t col) noexcept;
         [[nodiscard]] Node * getNodeAt(size_t row,size_t col) const noexcept;
         [[nodiscard]] QLineEdit * getStatusBar(uint32_t tabIndex) const noexcept;
         [[nodiscard]] bool isBlock(Node * currentNode) const noexcept;
         [[nodiscard]] bool isSpecial(Node * currentNode) const noexcept;
         void setTimersIntervals(std::chrono::milliseconds newSpeed) const noexcept;
         void memsetDs() const noexcept;
         void stopTimers() const noexcept;
         [[nodiscard]] QHBoxLayout * getLegendLayout(QWidget * parentWidget,QString token) const noexcept;
         void disableBarTabs(int32_t exception) const noexcept;
         void enableAllBarTabs() const noexcept;  
         void bfsStart(bool newStart) const noexcept;
         void dfsStart(bool newStart) const noexcept;
         void dijkstraStart(bool newStart) const noexcept;
         static void addShadowEffect(QLabel * label) noexcept;
         [[nodiscard]] static bool validCordinate(ptrdiff_t row,ptrdiff_t col) noexcept;
         [[nodiscard]] static std::pair<size_t,size_t> getRandomCord() noexcept;

///
         constexpr static int32_t yOffset = -135;
         constexpr static uint32_t rowCnt = 10;
         constexpr static uint32_t colCnt = 20;
         constexpr static uint32_t defaultDelay = 100;
         constexpr static uint32_t maximumBlocks = 60;
         constexpr static std::array<int32_t,4> xCord {-1,1,0,0};
         constexpr static std::array<int32_t,4> yCord {0,0,1,-1};
         inline static std::mt19937 generator = std::mt19937(std::random_device()());
         inline static std::uniform_int_distribution rowRange = std::uniform_int_distribution<size_t>(0,rowCnt - 1);
         inline static std::uniform_int_distribution colRange = std::uniform_int_distribution<size_t>(0,colCnt - 1);
         inline static std::uniform_int_distribution binaryDist = std::uniform_int_distribution<uint8_t>(0,1);

         bool m_running = false; 
         Node * m_sourceNode = nullptr;
         Node * m_targetNode = nullptr; 
         uint32_t m_timerDelay = defaultDelay; 
         std::unique_ptr<QTimer> bfsTimer = std::make_unique<QTimer>();
         std::unique_ptr<QTimer> dfsTimer = std::make_unique<QTimer>();
         std::unique_ptr<QTimer> dijkstraTimer = std::make_unique<QTimer>();
         std::unique_ptr<QTimer> pathTimer = std::make_unique<QTimer>();
         std::unique_ptr<std::queue<std::pair<Node*,uint32_t>>> m_queue; 
         std::unique_ptr<std::stack<std::pair<Node*,uint32_t>>> m_stack;
         std::unique_ptr<std::vector<std::vector<bool>>> m_visited;
         std::unique_ptr<std::vector<std::vector<uint32_t>>> m_distance;
         std::unique_ptr<std::priority_queue<pIntNode,std::vector<pIntNode>,std::greater<>>> m_priorityQueue;
         QGraphicsScene * innerScene = new QGraphicsScene(this);
         QGraphicsGridLayout * m_innerLayout;
         std::unique_ptr<QTabWidget> m_bar;
         std::pair<size_t,size_t> m_sourceNodeCord;
         std::pair<size_t,size_t> m_targetNodeCord;
         QSize windowSize;
         std::unique_ptr<StackedWidget> helpDialogWidget;

public slots:
         void setDelay(uint32_t newDelay) noexcept;
signals:
         void foundPath() const; 
         void close() const;
         void resetButtons() const; 
         void runningStatusChanged(bool newState) const; 
         void animationDurationChanged(uint32_t newDuration) const; 
};

inline GraphicsScene::GraphicsScene(const QSize size) : windowSize(size), helpDialogWidget(std::make_unique<StackedWidget>(windowSize)){
         populateBar();
         setTimersIntervals(static_cast<std::chrono::milliseconds>(m_timerDelay));
         configureInnerScene();
         connectPaths();
         setMainSceneConnections();
}

inline void GraphicsScene::connectPaths() const noexcept {
         connect(bfsTimer.get(),&QTimer::timeout,this,&GraphicsScene::bfsImplementation);
         connect(dfsTimer.get(),&QTimer::timeout,this,&GraphicsScene::dfsImplementation);
         connect(dijkstraTimer.get(),&QTimer::timeout,this,&GraphicsScene::dijkstraImplementation);
         pathConnect();
}

inline void GraphicsScene::configureInnerScene() noexcept {
         populateGridScene();
         allocDataStructures();
         memsetDs();
}

inline void GraphicsScene::setMainSceneConnections() const noexcept {
         QTimer::singleShot(1009,helpDialogWidget.get(),&StackedWidget::show);
         connect(this,&GraphicsScene::runningStatusChanged,&Node::setRunningState);
         connect(this,SIGNAL(foundPath()),pathTimer.get(),SLOT(start()));
}

inline std::pair<size_t,size_t> GraphicsScene::getRandomCord() noexcept {
         return std::make_pair(rowRange(generator),colRange(generator));
}

inline void GraphicsScene::addShadowEffect(QLabel * label) noexcept {
         auto shadowEffect = new QGraphicsDropShadowEffect(label);
         shadowEffect->setBlurRadius(4);// px
         shadowEffect->setOffset(0.5,0.5); // x,y px
         label->setGraphicsEffect(shadowEffect);
}

inline void GraphicsScene::disableBarTabs(const int32_t exception) const noexcept {
         for(int32_t index = 0;index < m_bar->count();index++){
                  if(index != exception){
                           m_bar->setTabEnabled(index,false);
                  }
         }
}

inline void GraphicsScene::enableAllBarTabs() const noexcept {
         for(int32_t index = 0;index < m_bar->count();index++){
                  m_bar->setTabEnabled(index,true);
         }
}

inline bool GraphicsScene::isRunning() const noexcept {
         return m_running;
}

inline void GraphicsScene::setDelay(const uint32_t newDelay) noexcept {
         m_timerDelay = static_cast<uint32_t>(std::abs(static_cast<int64_t>(1000) - newDelay));
         setTimersIntervals(static_cast<std::chrono::milliseconds>(m_timerDelay));
}

inline void GraphicsScene::setTimersIntervals(const std::chrono::milliseconds newDelay) const noexcept {
         dfsTimer->setInterval(newDelay);
         bfsTimer->setInterval(newDelay);
         dijkstraTimer->setInterval(newDelay);
         pathTimer->setInterval(newDelay);
}

inline void GraphicsScene::stopTimers() const noexcept {
         dfsTimer->stop();
         bfsTimer->stop();
         dijkstraTimer->stop();
         pathTimer->stop();
}

inline Node * GraphicsScene::getNodeAt(const size_t row,const size_t col) const noexcept {
         if(auto * node = dynamic_cast<Node*>(m_innerLayout->itemAt(static_cast<int>(row),static_cast<int>(col)))){
                  return node;
         }

         return nullptr;
}

inline bool GraphicsScene::validCordinate(const ptrdiff_t row,const ptrdiff_t col) noexcept {
         return row >= 0 && static_cast<size_t>(row) < rowCnt && col >= 0 && static_cast<size_t>(col) < colCnt;
}

inline QLineEdit * GraphicsScene::getStatusBar(const uint32_t tabIndex) const noexcept {
         auto widget = m_bar->widget(static_cast<int32_t>(tabIndex));
         auto abstractLayout = static_cast<QGridLayout*>(widget->layout())->itemAtPosition(1,0);
         return static_cast<QLineEdit*>(static_cast<QHBoxLayout*>(abstractLayout)->itemAt(0)->widget());
}

inline bool GraphicsScene::isSpecial(Node * currentNode) const noexcept {
         return currentNode == m_sourceNode || currentNode == m_targetNode;
}

inline void GraphicsScene::updateSourceTargetNodes() const noexcept {
         assert(m_sourceNode && m_targetNode);

         m_sourceNode->setType(Node::State::Source);
         m_targetNode->setType(Node::State::Target);
}

inline bool GraphicsScene::isBlock(Node * currentNode) const noexcept {
         return currentNode->getType() == Node::State::Block;
}

inline void GraphicsScene::bfsStart(const bool newStart) const noexcept {
         if(newStart){
                  m_queue->push({m_sourceNode,0});
                  auto [startX,startY] = m_sourceNode->getCord();
                  (*m_visited)[startX][startY] = true;
         }

         bfsTimer->start();
}

inline void GraphicsScene::dfsStart(const bool newStart) const noexcept {
         if(newStart){
                  m_stack->push({m_sourceNode,0});
                  auto [startX,startY] = m_sourceNode->getCord();
                  (*m_visited)[startX][startY] = true;
         }

         dfsTimer->start();
}

inline void GraphicsScene::dijkstraStart(const bool newStart) const noexcept {
         if(newStart){
                  m_priorityQueue->push({0,m_sourceNode});
                  auto [startX,startY] = m_sourceNode->getCord();
                  (*m_distance)[startX][startY] = 0;
         }

         dijkstraTimer->start();
}

#endif