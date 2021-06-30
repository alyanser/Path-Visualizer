#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsPixmapItem>
#include <QGraphicsLayoutItem>
#include <QObject>

class Node : public QObject,public QGraphicsPixmapItem,public QGraphicsLayoutItem{
         Q_OBJECT
public:
         enum State{Source,Target,Active,Inactive,Visited,Block,Inpath};
         ///
         Node(int row,int col,QGraphicsPixmapItem * parent = nullptr);
         ~Node() = default;

         void setType(const State & newType);
         void setPathParent(Node * newParent);
         Node * getPathParent() const;
         std::pair<int,int> getCord() const;
protected:
         // layoutitem
         void setGeometry(const QRectF & geometry) override;
         QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const override;
         // graphicsitem
         QRectF boundingRect() const override;
         void paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget) override;
         bool sceneEvent(QEvent * event) override;
private:
         State type;
         int gridX,gridY; // cordinate in gridSceneLayout
         Node * pathParent; // for paths
};

#endif