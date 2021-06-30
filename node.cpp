#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>
#include "node.hpp"

Node::Node(int row,int col,QGraphicsPixmapItem * parent) : QGraphicsPixmapItem(parent), type(Inactive), gridX(row), gridY(col){
         pathParent = nullptr;
         // setFlags(ItemIsSelectable | ItemIsFocusable);
         setAcceptHoverEvents(true);
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
                  case Source : brush.setColor(Qt::green);break;
                  case Target : brush.setColor(Qt::red);break;
                  case Active : brush.setColor(Qt::white);break;
                  case Inactive : brush.setColor(Qt::blue);break;
                  case Visited : brush.setColor(Qt::yellow);break;
                  case Block : brush.setColor(Qt::gray);break;
                  case Inpath : brush.setColor(Qt::red);break;
                  default : assert(false);
         }
         painter->setBrush(brush);
         painter->setRenderHint(QPainter::Antialiasing);
         painter->drawEllipse(0,0,24,23);
}

QSizeF Node::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const{
         Q_UNUSED(which);
         Q_UNUSED(constraint);

         return QSizeF(24,23);
}

void Node::setGeometry(const QRectF & geometry){
         prepareGeometryChange();
         QGraphicsLayoutItem::setGeometry(geometry);
         setPos(geometry.topLeft());
}

void Node::setType(const State & newType){
         type = newType;
         update();
}

void Node::setPathParent(Node * newParent){
         pathParent = newParent;
}

std::pair<int,int> Node::getCord() const{
         return {gridX,gridY};
}

Node * Node::getPathParent() const{
         return pathParent;
}
