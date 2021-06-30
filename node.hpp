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
         //events
private:
         State type;
         int gridX,gridY; // cordinate in gridSceneLayout
         Node * pathParent; // for paths
};

#endif


//  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
//     virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
//     virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
//     virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
//     virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
//     virtual void focusInEvent(QFocusEvent *event);
//     virtual void focusOutEvent(QFocusEvent *event);
//     virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
//     virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
//     virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
//     virtual void keyPressEvent(QKeyEvent *event);
//     virtual void keyReleaseEvent(QKeyEvent *event);
//     virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
//     virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//     virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
//     virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
//     virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
//     virtual void inputMethodEvent(QInputMethodEvent *event);
//     virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
