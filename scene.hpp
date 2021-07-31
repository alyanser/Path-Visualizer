#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>
#include <queue>
#include <stack>

class QTabWidget;
class QSize;
class QLineEdit;
class QGraphicsGridLayout;
class Node;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QTimer;
class QPushButton;
class StackedWidget;
class QLabel;

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
         Q_PROPERTY(bool running WRITE setRunning READ isRunning)
         enum { bfsIndex, dfsIndex, dijkstraIndex };
         using pIntNode = std::pair<uint32_t,Node*>;
public:
         explicit GraphicsScene(QSize size);
         ~GraphicsScene();
         GraphicsScene(const GraphicsScene & other) = delete;
         GraphicsScene(GraphicsScene && other) = delete;
         GraphicsScene & operator = (const GraphicsScene & other) = delete;
         GraphicsScene & operator = (GraphicsScene && other) = delete;
private:
         uint32_t timerDelay; 
         const QSize windowSize;
         QGraphicsScene * innerScene;
         std::unique_ptr<StackedWidget> helpDialogWidget;
         std::unique_ptr<QTabWidget> bar;
         QTimer * bfsTimer,* dfsTimer,* dijkstraTimer,* pathTimer; 
         std::pair<size_t,size_t> sourceNodeCord;
         std::pair<size_t,size_t> targetNodeCord;
         QGraphicsGridLayout * innerLayout; 
         std::unique_ptr<std::queue<std::pair<Node*,uint32_t>>> queue; 
         std::unique_ptr<std::stack<std::pair<Node*,uint32_t>>> stack;
         std::unique_ptr<std::vector<std::vector<bool>>> visited; 
         std::unique_ptr<std::vector<std::vector<uint32_t>>> distance;
         std::unique_ptr<std::priority_queue<pIntNode,std::vector<pIntNode>,std::greater<>>> pq;
         bool running = false; 
         Node * sourceNode = nullptr; 
         Node * targetNode = nullptr; 
         constexpr inline static int32_t yOffset = -135; // px : 50 topbar 50 bottombar 35 padding
         constexpr inline static size_t rowCnt = 10;
         constexpr inline static size_t colCnt = 20;
         constexpr inline static uint32_t defaultDelay = 100; // ms
         constexpr inline static std::array<int32_t,4> xCord {-1,1,0,0};
         constexpr inline static std::array<int32_t,4> yCord {0,0,1,-1};

         void populateBar() noexcept;
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText) noexcept;
         void populateGridScene() noexcept;  
         void populateLegend(QWidget * parentWidget,QVBoxLayout * sideLayout) const noexcept;
         void populateBottomLayout(QWidget * parentWidget,QGridLayout * mainLayout) const noexcept;
         void populateSideLayout(QWidget * parent,QVBoxLayout * sideLayout,const QString & algoName,const QString & infoText) noexcept;
         void configureMachine(QWidget * parentWidget,QPushButton * statusButton) noexcept;

         void allocTimers() noexcept;
         void setMainSceneConnections() const noexcept;
         void connectPaths() const noexcept;
         void configureInnerScene() noexcept;
         void generateRandGridPattern() noexcept;
         void allocDataStructures() noexcept;
         void setRunning(bool newState) noexcept;  
         bool isRunning() const noexcept; 
         void cleanup() const noexcept; 
         void resetGrid() const noexcept;
         void updateSourceTargetNodes() const noexcept;
         static bool validCordinate(ptrdiff_t row,ptrdiff_t col) noexcept;
         Node * getNewNode(size_t row,size_t col) noexcept;
         Node * getNodeAt(size_t row,size_t col) const noexcept;
         QLineEdit * getStatusBar(uint32_t tabIndex) const noexcept;
         bool isBlock(Node * currentNode) const noexcept;
         bool isSpecial(Node * currentNode) const noexcept;
         void setTimersIntervals(std::chrono::milliseconds newSpeed) const noexcept;
         void memsetDs() const noexcept;
         void stopTimers() const noexcept;
         void pathConnect() const noexcept;  
         static std::pair<size_t,size_t> getRandomCord() noexcept;
         static void addShadowEffect(QLabel * label) noexcept;
         QHBoxLayout * getLegendLayout(QWidget * parentWidget,QString token) const noexcept;
         void disableBarTabs(int32_t exception) const noexcept;
         void enableAllBarTabs() const noexcept;  
         
         void bfsConnect() const noexcept;
         void dfsConnect() const noexcept;
         void dijkstraConnect() const noexcept;
         
         void bfsStart(bool newStart) const noexcept;
         void dfsStart(bool newStart) const noexcept;
         void dijkstraStart(bool newStart) const noexcept;
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
