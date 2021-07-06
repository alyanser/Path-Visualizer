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
         
         Node(int row,int col,QGraphicsItem * parent = nullptr);
         ~Node();

         State getType() const;
         void setType(const State newType,const bool newStart = true);
         Node * getPathParent() const;
         void setPathParent(Node * newParent);
         std::pair<int,int> getCord() const;
protected:
         // overrides qgraphicslayout methods
         void setGeometry(const QRectF & geometry) override;
         QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const override;
         // overrides qgprahicsitem methods
         QRectF boundingRect() const override;
         void paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget) override;
         
         void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
         void dragEnterEvent(QGraphicsSceneDragDropEvent * event) override;
         void dropEvent(QGraphicsSceneDragDropEvent * event) override;
private:
         std::pair<int,int> currentLocation;
         QTimeLine * backwardTimer,* forwardTimer;
         Node * pathParent = nullptr; 
         State type = Node::Inactive;
         QPixmap icon;
         inline static bool algorithmPaused = false; 
         inline constexpr static int dimension = 32; // px
         inline constexpr static int halfDimension = 16; // px
         inline constexpr static uint32_t backwardDuration = 175;  // ms
         inline constexpr static uint32_t forwardDuration = 175;  // ms

         void configureBackwardTimer();
         void configureForwardTimer();
         void setNodeRotation();
         void undoNodeRotation();
signals:
         void sourceSet();
         void targetSet();
public slots:
         static void setRunningState(const bool newAlgorithmState);
};

#endif