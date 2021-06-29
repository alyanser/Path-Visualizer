#include <QPainter>
#include "node.hpp"


Node::Node(QGraphicsPixmapItem * parent) : QGraphicsPixmapItem(parent), QGraphicsLayoutItem(){
         setGraphicsItem(this);
}

QRectF Node::boundingRect() const{
         return QRectF(0,0,20,20);
}

void Node::paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget){
         Q_UNUSED(option);
         Q_UNUSED(widget);

         painter->setBrush(QBrush(Qt::black));
         painter->drawRect(0,0,20,20);
}

QSizeF Node::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const{
         Q_UNUSED(which);
         
         return constraint;
}

void Node::setGeometry(const QRectF & geometry){
         prepareGeometryChange();
         QGraphicsLayoutItem::setGeometry(geometry);
         setPos(geometry.topLeft());
}
