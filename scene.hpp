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
         QTabWidget * bar;
         QGraphicsScene * innerScene;
         QGraphicsGridLayout * innerLayout;
         std::pair<int,int> startCord,endCord;

         ///
         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene();
         void reset();
         bool validCordinate(const int & row,const int & col) const;
         Node * getNodeAt(const int & row,const int & col) const;
         QLineEdit * getStatusBar(const int & tabIndex) const;

         // implementations
         void bfs();
         void dfs();
signals:
         void close();
};

#endif
