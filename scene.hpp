#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>
#include <stack>
#include <queue>

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

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
         Q_PROPERTY(bool runningState WRITE setRunning READ isRunning);
public:
         GraphicsScene(const QSize & size);
         ~GraphicsScene();
private:
         bool runningState = false; // 0 : off - 1 : on
         Node * sourceNode = nullptr; 
         Node * targetNode = nullptr; 
         uint32_t timerDelay; // determines the timer after which timer will shoot
         QTabWidget * bar; // represents all algorithm tab by tab
         QGraphicsScene * innerScene; // grid
         QTimer * bfsTimer,* dfsTimer,* dijkstraTimer,* pathTimer;// to control the delay between each step
         std::pair<int,int> startCord;// default sourceNode position
         std::pair<int,int> endCord; // default targetNode position
         QGraphicsGridLayout * innerLayout; // layout of grid
         std::unique_ptr<std::queue<std::pair<Node*,int>>> queue; 
         std::unique_ptr<std::stack<std::pair<Node*,int>>> stack;
         std::unique_ptr<std::vector<std::vector<bool>>> visited; 
         std::unique_ptr<std::vector<std::vector<int>>> distance;
         std::unique_ptr<std::priority_queue<std::pair<int,Node*>,std::vector<std::pair<int,Node*>>,std::greater<>>> pq;
         /// visual designs
         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene(); 
         void populateLegend(QWidget * parentWidget,QVBoxLayout * sideLayout) const;
         void populateBottomLayout(QWidget * parentWidget,QGridLayout * mainLayout) const;
         void populateSideLayout(QVBoxLayout * sideLayout,const QString & algoName,const QString & infoText);
         // setups
         void configureMachine(QWidget * parentWidget,QPushButton * statusBut);
         // utility
         void allocDataStructures();
         void setRunning(const bool newState); 
         bool isRunning() const; 
         void cleanup() const; 
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
         void getPath() const; 
         // implementations of algorithms
         void bfsConnect() const;
         void dfsConnect() const ;
         void dijkstraConnect() const;
         void bfsStart(const bool newStart) const;
         void dfsStart(const bool newStart) const;
         void dijkstraStart(const bool newStart) const;
signals:
         void close() const; // connected with qapplication - to quit
         void resetButtons() const; // connected with buttons to reset state after process ends
         void runningStatusChanged(const bool newState); // connected with Node class 
public slots:
         void setDelay(const uint32_t newDelay);
};

#endif
