#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsLayoutItem>
#include <QGraphicsObject>

class QTimeLine;

class Node : public QGraphicsObject,public QGraphicsLayoutItem{
         Q_OBJECT
         Q_PROPERTY(State type WRITE setType READ getType)
public:
         enum class State{Source,Target,Active,Inactive,Visited,Block,Inpath};
         
         Node(uint32_t row,uint32_t col,QGraphicsItem * parent = nullptr);
         ~Node() override;
         Node(const Node & other) = delete;
         Node(Node && other) = delete;
         Node & operator = (const Node & other) = delete;
         Node & operator = (Node && other) = delete;

         State getType() const;
         void setType(State newType,bool newStart = true);
         Node * getPathParent() const;
         void setPathParent(Node * newParent); 
         std::pair<uint32_t,uint32_t> getCord() const;
protected:
         // overrides qgraphicslayout methods
         void setGeometry(const QRectF & geometry) override;
         QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint) const override;
         // overrides qgprahicsitem methods
         QRectF boundingRect() const override;
         void paint(QPainter * painter,const QStyleOptionGraphicsItem * option, QWidget * widget) override;
         
         void mousePressEvent(QGraphicsSceneMouseEvent * event) override;
         void dragEnterEvent(QGraphicsSceneDragDropEvent * event) override;
         void dropEvent(QGraphicsSceneDragDropEvent * event) override;
private:
         std::pair<uint32_t,uint32_t> currentLocation;
         std::unique_ptr<QTimeLine> backwardTimer;
         std::unique_ptr<QTimeLine> forwardTimer;
         Node * pathParent = nullptr; 
         State type = Node::State::Inactive;
         QPixmap icon {};
         inline static bool algorithmPaused = false;
         inline constexpr static int32_t dimension = 32; // px
         inline constexpr static int32_t halfDimension = dimension / 2; // px
         inline constexpr static uint32_t defaultTimerDuration = 175;  // ms

         void setTimersDuration(uint32_t newDuration) const;
         void configureBackwardTimer();
         void configureForwardTimer();
         void setNodeRotation();
         void undoNodeRotation();
public slots:
         static void setRunningState(bool newAlgorithmState);
         void changeAnimationDuration(uint32_t newDuration) const;
signals:
         void sourceSet();
         void targetSet();
};

#endif