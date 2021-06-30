#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsPixmapItem>
#include <QGraphicsLayoutItem>

class Node : public QGraphicsPixmapItem,public QGraphicsLayoutItem{

public:
         Node(QGraphicsPixmapItem * parent = nullptr);
         ~Node() = default;
protected:
         // layoutitem
         void setGeometry(const QRectF & geometry) override;
         QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const override;
         // graphicsitem
         QRectF boundingRect() const override;
         void paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget) override;

         bool sceneEvent(QEvent * event) override;
private:

};

// Q_DECLARE_INTERFACE(Node,"NodefGrid")

#endif