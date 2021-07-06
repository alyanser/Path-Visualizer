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
class StackedWidget;

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
         Q_PROPERTY(bool runningState WRITE setRunning READ isRunning);
         typedef std::pair<int,Node*> pIntNode;
public:
         GraphicsScene(const QSize & size);
         ~GraphicsScene();
private:
         uint32_t timerDelay; 
         QGraphicsScene * innerScene; // grid
         StackedWidget * helpWidget;
         QTabWidget * bar;
         QTimer * bfsTimer,* dfsTimer,* dijkstraTimer,* pathTimer; 
         std::pair<int,int> startCord;// sourceNode position
         std::pair<int,int> endCord; // targetNode position
         QGraphicsGridLayout * innerLayout; // layout of grid
         std::unique_ptr<std::queue<std::pair<Node*,int>>> queue; 
         std::unique_ptr<std::stack<std::pair<Node*,int>>> stack;
         std::unique_ptr<std::vector<std::vector<bool>>> visited; 
         std::unique_ptr<std::vector<std::vector<int>>> distance;
         std::unique_ptr<std::priority_queue<pIntNode,std::vector<pIntNode>,std::greater<>>> pq;
         bool runningState = false; // 0 : no - 1 : yes
         Node * sourceNode = nullptr; 
         Node * targetNode = nullptr; 

         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene(); 
         void populateLegend(QWidget * parentWidget,QVBoxLayout * sideLayout) const;
         void populateBottomLayout(QWidget * parentWidget,QGridLayout * mainLayout) const;
         void populateSideLayout(QVBoxLayout * sideLayout,const QString & algoName,const QString & infoText);

         void configureMachine(QWidget * parentWidget,QPushButton * statusButton);

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
         void getPath() const; 
         std::pair<int,int> getRandomCord() const;
         
         void bfsConnect() const;
         void dfsConnect() const;
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
