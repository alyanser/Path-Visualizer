#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsLayoutItem>
#include <QGraphicsObject>

class Node : public QGraphicsObject,public QGraphicsLayoutItem{
         Q_OBJECT
         Q_PROPERTY(State type WRITE setType READ getType)
public:
         enum State{Source,Target,Active,Inactive,Visited,Block,Inpath};
         ///
         Node(int row,int col,QGraphicsItem * parent = nullptr);
         ~Node() = default;
         // utility
         void setType(const State & newType);
         State getType() const;
         void setPathParent(Node * newParent);
         Node * getPathParent() const;
         std::pair<int,int> getCord() const;
protected:
         // overrides qgraphicslayout methods
         void setGeometry(const QRectF & geometry) override;
         QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const override;
         // overrides qgprahicsitem methods
         QRectF boundingRect() const override;
         void paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget) override;
         //events
         void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
         void dragEnterEvent(QGraphicsSceneDragDropEvent * event) override;
         void dropEvent(QGraphicsSceneDragDropEvent * event) override;
private:
         State type; // type of node
         int gridX,gridY; // cordinate in gridSceneLayout
         Node * pathParent; // parent for paths
signals:
         void sourceSet();
         void targetSet();
};

#endif


//     virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
//     virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
//     virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

//  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
//     virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
//     virtual void focusInEvent(QFocusEvent *event);
//     virtual void focusOutEvent(QFocusEvent *event);
//     virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
//     virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
//     virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
//     virtual void keyPressEvent(QKeyEvent *event);
//     virtual void keyReleaseEvent(QKeyEvent *event);

//     virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//     virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
//     virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
//     virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
//     virtual void inputMethodEvent(QInputMethodEvent *event);
//     virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
