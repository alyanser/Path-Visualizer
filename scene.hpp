#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>

class QTabWidget;
class QSize;
class QLineEdit;
class QGraphicsGridLayout;
class Node;
class QHBoxLayout;

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
public:
         GraphicsScene(const QSize & size);
         ~GraphicsScene();
private:
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
         void cleanup();
         void updateSrcTarNodes();
         void getPath() const;
         bool validCordinate(const int & row,const int & col) const;
         Node * getNodeAt(const int & row,const int & col) const;
         QLineEdit * getStatusBar(const int & tabIndex) const;
         bool isBlock(Node * currentNode) const;
         bool isSpecial(Node * currentNode) const;
         // implementations
         void bfs() const;
         void dfs() const ;
         void dijkstra() const;
signals:
         void close() const; // connected with qapplication - to quit
         void resetButtons() const; // connected with buttons to reset state after process ends
};

#endif
