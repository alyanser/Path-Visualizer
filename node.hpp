#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsPixmapItem>
#include <QGraphicsLayoutItem>

class Node : public QGraphicsPixmapItem,public QGraphicsLayoutItem{

public:
         Node(QGraphicsPixmapItem * parent = nullptr);
protected:
         void setGeometry(const QRectF & geometry) override;
         QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const override;

         QRectF boundingRect() const override;
         void paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget) override;
private:

};

// Q_DECLARE_INTERFACE(Node,"NodefGrid")

#endif