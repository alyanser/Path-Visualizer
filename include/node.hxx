#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsLayoutItem>
#include <QGraphicsObject>
#include <QTimeLine>

class QTimeLine;

class Node : public QGraphicsObject,public QGraphicsLayoutItem{
         Q_OBJECT
         Q_PROPERTY(State type WRITE setType READ getType)

         enum dimensions { dimension = 32, halfDimension = 16 }; // px
         enum { defaultTimerDuration = 175 /* ms */ };
public:
         enum class State{Source,Target,Active,Inactive,Visited,Block,Inpath};
         
         Node(uint32_t row,uint32_t col,QGraphicsItem * parent = nullptr);

         void setType(State newType,bool newStart = true) noexcept;
         [[nodiscard]] State getType() const noexcept;
         void setPathParent(Node * newParent) noexcept;  
         [[nodiscard]] Node * getPathParent() const noexcept;
         [[nodiscard]] std::pair<uint32_t,uint32_t> getCord() const noexcept;
protected:
         void setGeometry(const QRectF & geometry) noexcept override;
         [[nodiscard]] QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint) const noexcept override;
         [[nodiscard]] QRectF boundingRect() const noexcept override;
         void paint(QPainter * painter,const QStyleOptionGraphicsItem * option, QWidget * widget) noexcept override;
         
         void mousePressEvent(QGraphicsSceneMouseEvent * event) noexcept override;
         void dragEnterEvent(QGraphicsSceneDragDropEvent * event) noexcept override;
         void dropEvent(QGraphicsSceneDragDropEvent * event) noexcept override;
private:
         void setTimersDuration(uint32_t newDuration) const noexcept;
         void configureBackwardTimer() noexcept ;
         void configureForwardTimer() noexcept;
         void setNodeRotation() noexcept;
         void undoNodeRotation() noexcept;

///
         std::unique_ptr<QTimeLine> m_backwardTimer = std::make_unique<QTimeLine>();
         std::unique_ptr<QTimeLine> m_forwardTimer = std::make_unique<QTimeLine>();
         QPixmap m_icon;
         Node * m_pathParent = nullptr; 
         State m_type = Node::State::Inactive;
         inline static bool m_algorithmPaused = false;

         std::pair<uint32_t,uint32_t> m_currentLocation;
public slots:
         static void setRunningState(bool newAlgorithmState) noexcept;
         void changeAnimationDuration(uint32_t newDuration) const noexcept;
signals:
         void sourceSet();
         void targetSet();
};

#endif