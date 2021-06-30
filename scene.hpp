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
         QTabWidget * bar;
         QGraphicsScene * innerScene;
         QGraphicsGridLayout * innerLayout;
         std::pair<int,int> startCord;// default start pos
         std::pair<int,int> endCord; // default end pos
         ///
         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene();
         void reset();
         void getPath() const;
         bool validCordinate(const int & row,const int & col) const;
         Node * getNodeAt(const int & row,const int & col) const;
         QLineEdit * getStatusBar(const int & tabIndex) const;
         
         // implementations
         void bfs() const;
         void dfs() const ;
         void dijkstra() const;
signals:
         void close() const; // quits qapplication
         void reached() const; // emitted after an algorithm ends
};

#endif
