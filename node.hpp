#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsLayoutItem>
#include <QGraphicsObject>

class QTimeLine;

class Node : public QGraphicsObject,public QGraphicsLayoutItem{
         Q_OBJECT
         Q_PROPERTY(State type WRITE setType READ getType)
public:
         enum State{Source,Target,Active,Inactive,Visited,Block,Inpath};
         ///
         Node(int row,int col,QGraphicsItem * parent = nullptr);
         ~Node();
         // utility
         void setType(const State newType,const bool newStart = true);
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
         // events
         void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
         void dragEnterEvent(QGraphicsSceneDragDropEvent * event) override;
         void dropEvent(QGraphicsSceneDragDropEvent * event) override;
private:
         State type = Node::Inactive;
         Node * pathParent = nullptr; 
         std::pair<int,int> currentLocation;
         inline static bool algoRunning = false; 
         inline const static uint32_t dimension = 32; // px
         inline const static uint32_t backwardDuration = 200;  // ms
         inline const static uint32_t forwardDuration = 200;  // ms
         QPixmap icon;
         QTimeLine * backwardTimer,* forwardTimer;
         ///
         void configureBackwardTimer();
         void configureForwardTimer();
signals:
         void sourceSet();
         void targetSet();
public slots:
         static void setRunningState(const bool newState);

};

#endif