#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>
#include <queue>
#include <stack>
#include <random>
#include <QTimer>
#include <QTabWidget>
#include <QGraphicsScene>
#include "helpDialog.hpp"

class QTabWidget;
class QSize;
class QLineEdit;
class QGraphicsGridLayout;
class Node;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QPushButton;
class QLabel;

class GraphicsScene : public QGraphicsScene {
         Q_OBJECT
         Q_PROPERTY(bool running WRITE setRunning READ isRunning)

         enum indexes { BfsIndex, DfsIndex, DijkstraIndex };
         enum { Yoffset = -135, RowCnt = 10, ColCnt = 20, DefaultDelay = 100, MaximumBlocks = 60 };

         using pIntNode = std::pair<uint32_t,Node*>;
public:
         explicit GraphicsScene(QSize size);
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
         void pathConnect() const noexcept;  
         [[nodiscard]] QHBoxLayout * getLegendLayout(QWidget * parentWidget,QString token) const noexcept;
         void disableBarTabs(int32_t exception) const noexcept;
         void enableAllBarTabs() const noexcept;  
         void bfsConnect() const noexcept;
         void dfsConnect() const noexcept;
         void dijkstraConnect() const noexcept;
         void bfsStart(bool newStart) const noexcept;
         void dfsStart(bool newStart) const noexcept;
         void dijkstraStart(bool newStart) const noexcept;
         static void addShadowEffect(QLabel * label) noexcept;
         [[nodiscard]] static bool validCordinate(ptrdiff_t row,ptrdiff_t col) noexcept;
         [[nodiscard]] static std::pair<size_t,size_t> getRandomCord() noexcept;

///
         constexpr static std::array<int32_t,4> m_xCord {-1,1,0,0};
         constexpr static std::array<int32_t,4> m_yCord {0,0,1,-1};
         inline static std::mt19937 m_generator = std::mt19937(std::random_device()());
         inline static std::uniform_int_distribution m_rowRange = std::uniform_int_distribution<size_t>(0,RowCnt - 1);
         inline static std::uniform_int_distribution m_colRange = std::uniform_int_distribution<size_t>(0,ColCnt - 1);
         inline static std::uniform_int_distribution m_binaryDist = std::uniform_int_distribution<uint8_t>(0,1);

         bool m_running = false; 
         Node * m_sourceNode = nullptr;
         Node * m_targetNode = nullptr; 
         uint32_t m_timerDelay = DefaultDelay; 
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
         // ctor init
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

#endif
