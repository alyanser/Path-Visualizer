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
class QTimer;

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
         Q_PROPERTY(bool on WRITE setRunning READ isRunning);
public:
         GraphicsScene(const QSize & size);
         ~GraphicsScene();
private:
         uint32_t timerDelay; // determines the timer after which timer will shoot
         bool on; // determines if any algorithm is running
         QTimer * bfsTimer,* dfsTimer,* dijTimer;
         Node * sourceNode;
         Node * targetNode;
         std::pair<int,int> startCord;// default sourceNode position
         std::pair<int,int> endCord; // default targetNode position
         QTabWidget * bar;
         QGraphicsScene * innerScene; // grid
         QGraphicsGridLayout * innerLayout; // layout of grid
         std::unique_ptr<std::queue<std::pair<Node*,int>>> queue;
         std::unique_ptr<std::stack<std::pair<Node*,int>>> stack;
         std::unique_ptr<std::vector<std::vector<bool>>> visited;
         std::unique_ptr<std::vector<std::vector<int>>> distance;
         std::unique_ptr<std::priority_queue<std::pair<int,Node*>,std::vector<std::pair<int,Node*>>,std::greater<>>> pq;
         /// designs
         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene();
         // utility
         void setDataStructures();
         void setRunning(const bool & newState);
         bool isRunning() const;
         void cleanup();
         void updateSrcTarNodes();
         void getPath() const;
         bool validCordinate(const int & row,const int & col) const;
         Node * getNewNode(const int & row,const int & col);
         Node * getNodeAt(const int & row,const int & col) const;
         QLineEdit * getStatusBar(const int & tabIndex) const;
         bool isBlock(Node * currentNode) const;
         bool isSpecial(Node * currentNode) const;
         void setTimersIntervals(const uint32_t & newSpeed) const;
         void memsetDs();
         void stopTimers() const;
         // algorithm implementations
         void bfs(const bool & newStart) const;
         void dfs(const bool & newStart) const ;
         void dijkstra(const bool & newStart) const;
signals:
         void close() const; // connected with qapplication - to quit
         void resetButtons() const; // connected with buttons to reset state after process ends
         void runningStatusChanged(const bool & newState);
public slots:
         void setSpeed(const uint32_t & newSpeed);
};

#endif
