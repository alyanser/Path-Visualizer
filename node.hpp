#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsPixmapItem>
#include <QGraphicsLayoutItem>
#include <QObject>

class Node : public QObject,public QGraphicsPixmapItem,public QGraphicsLayoutItem{
         Q_OBJECT
public:
         Node(QGraphicsPixmapItem * parent = nullptr);
         ~Node() = default;

         enum State{Starter,Ender,Active,Inactive};
         void setType(const State & newType);
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
};

#endif