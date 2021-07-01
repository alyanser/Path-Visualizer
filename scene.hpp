#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>

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
         const uint32_t speed; // determines the speed at which algorithm will execute
         bool on; // determines if any algorithm is running
         QTimer * timer;
         Node * sourceNode;
         Node * targetNode;
         std::pair<int,int> startCord;// default sourceNode position
         std::pair<int,int> endCord; // default targetNode position
         QTabWidget * bar;
         QGraphicsScene * innerScene; // grid
         QGraphicsGridLayout * innerLayout; // layout of grid
         /// designs
         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene();
         // utility
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
         // algorithm implementations
         void bfs() const;
         void dfs() const ;
         void dijkstra() const;
signals:
         void close() const; // connected with qapplication - to quit
         void resetButtons() const; // connected with buttons to reset state after process ends
         void runningStatusChanged(const bool & newState);
};

#endif
