#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include "node.hpp"

// 24x23

Node::Node(QGraphicsPixmapItem * parent) : QGraphicsPixmapItem(parent), QGraphicsLayoutItem(){
         setGraphicsItem(this);
}

QRectF Node::boundingRect() const{
         return QRectF(0,0,24,23);
}

void Node::paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget){
         Q_UNUSED(option);
         Q_UNUSED(widget);
         
         painter->setRenderHint(QPainter::Antialiasing);
         painter->setBrush(QBrush(Qt::blue));
         painter->drawEllipse(0,0,24,23);
}

QSizeF Node::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const{
         Q_UNUSED(which); // size will be constant so whichis not required
         return QSizeF(24,23);
}

void Node::setGeometry(const QRectF & geometry){
         prepareGeometryChange();
         QGraphicsLayoutItem::setGeometry(geometry);
         setPos(geometry.topLeft());
}

bool Node::sceneEvent(QEvent * event){
         return false;
}