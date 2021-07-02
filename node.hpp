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
         State type = Node::Inactive; // type of node
         int gridX,gridY; // cordinate in gridSceneLayout
         Node * pathParent = nullptr; // parent for paths
         inline static bool on = false; // determines if any algorthm is running - connected with Graphicscene
signals:
         void sourceSet();
         void targetSet();
public slots:
         static void setRunningState(const bool & newState);

};

#endif