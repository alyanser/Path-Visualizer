#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>
#include <stack>
#include <queue>

QT_BEGIN_NAMESPACE
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
QT_END_NAMESPACE

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
         Q_PROPERTY(bool running WRITE setRunning READ isRunning);
         typedef std::pair<int,Node*> pIntNode;
public:
         GraphicsScene(const QSize size);
         ~GraphicsScene();
private:
         uint32_t timerDelay; 
         const QSize windowSize;
         QGraphicsScene * innerScene;
         std::unique_ptr<StackedWidget> helpDialogWidget;
         std::unique_ptr<QTabWidget> bar;
         QTimer * bfsTimer,* dfsTimer,* dijkstraTimer,* pathTimer; 
         std::pair<int,int> sourceNodeCord;
         std::pair<int,int> targetNodeCord;
         QGraphicsGridLayout * innerLayout; 
         std::unique_ptr<std::queue<std::pair<Node*,int>>> queue; 
         std::unique_ptr<std::stack<std::pair<Node*,int>>> stack;
         std::unique_ptr<std::vector<std::vector<bool>>> visited; 
         std::unique_ptr<std::vector<std::vector<int>>> distance;
         std::unique_ptr<std::priority_queue<pIntNode,std::vector<pIntNode>,std::greater<>>> pq;
         bool running = false; 
         Node * sourceNode = nullptr; 
         Node * targetNode = nullptr; 
         constexpr inline static int yOffset = -32;
         constexpr inline static int rowCnt = 10;
         constexpr inline static int colCnt = 19;
         constexpr inline static uint32_t defaultDelay = 100; // ms
         constexpr inline static int xCord[] {-1,1,0,0}; 
         constexpr inline static int yCord[] {0,0,1,-1};

         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene(); 
         void populateLegend(QWidget * parentWidget,QVBoxLayout * sideLayout) const;
         void populateBottomLayout(QWidget * parentWidget,QGridLayout * mainLayout) const;
         void populateSideLayout(QVBoxLayout * sideLayout,const QString & algoName,const QString & infoText);

         void configureMachine(QWidget * parentWidget,QPushButton * statusButton);

         void allocTimers();
         void setMainSceneConnections() const;
         void connectPaths() const;
         void configureInnerScene();
         void generateRandGridPattern();
         void allocDataStructures();
         void setRunning(const bool newState); 
         bool isRunning() const; 
         void cleanup() const; 
         void resetGrid() const;
         void updateSrcTarNodes() const; // srcTar := source Target
         bool validCordinate(const int row,const int col) const;
         Node * getNewNode(const int row,const int col);
         Node * getNodeAt(const int row,const int col) const;
         QLineEdit * getStatusBar(const int tabIndex) const;
         bool isBlock(Node * currentNode) const;
         bool isSpecial(Node * currentNode) const;
         void setTimersIntervals(const uint32_t newSpeed) const;
         void memsetDs() const;
         void stopTimers() const;
         void pathConnect() const; 
         std::pair<int,int> getRandomCord() const;
         void addShadowEffect(QLabel * label) const;
         QHBoxLayout * getLegendLayout(QWidget * parentWidget,QString token) const;
         void disableBarTabs(const int exception) const;
         void enableAllBarTabs() const;
         
         void bfsConnect() const;
         void dfsConnect() const;
         void dijkstraConnect() const;
         
         void bfsStart(const bool newStart) const;
         void dfsStart(const bool newStart) const;
         void dijkstraStart(const bool newStart) const;
public slots:
         void setDelay(const uint32_t newDelay);
signals:
         void foundPath() const; 
         void close() const; // connected with qapplication
         void resetButtons() const; 
         void runningStatusChanged(const bool newState) const; 
         void animationDurationChanged(const uint32_t newDuration) const; 
};

#endif
