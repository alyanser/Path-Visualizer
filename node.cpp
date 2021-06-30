#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include "node.hpp"

Node::Node(QGraphicsPixmapItem * parent) : QGraphicsPixmapItem(parent), QGraphicsLayoutItem(), type(Inactive){
         setGraphicsItem(this);
}

QRectF Node::boundingRect() const{
         return QRectF(0,0,24,23);
}

void Node::paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget){
         Q_UNUSED(option);
         Q_UNUSED(widget);

         QBrush brush;
         brush.setStyle(Qt::SolidPattern);

         switch(type){
                  case Starter : brush.setColor(Qt::green);break;
                  case Ender : brush.setColor(Qt::red);break;
                  case Active : brush.setColor(Qt::white);break;
                  case Inactive : brush.setColor(Qt::blue);break;
                  default : assert(false);
         }
         painter->setBrush(brush);
         painter->setRenderHint(QPainter::Antialiasing);
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

void Node::setType(const State & newType){
         type = newType;
}