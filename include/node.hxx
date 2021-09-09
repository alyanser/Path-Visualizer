#ifndef NODE_HPP
#define NODE_HPP

#include <QGraphicsLayoutItem>
#include <QGraphicsObject>
#include <QTimeLine>
#include <QPainter>

class QTimeLine;

class Node : public QGraphicsObject,public QGraphicsLayoutItem{
         Q_OBJECT
         Q_PROPERTY(State type WRITE setType READ getType)
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
         constexpr static int32_t defaultTimerDuration = 175; // ms
         constexpr static int32_t dimension = 32;
         constexpr static int32_t halfDimension = 16;
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

inline void Node::changeAnimationDuration(const uint32_t newDuration) const noexcept {
         // new duration comes from slider direclty range : 0 - 1000
         double factor = static_cast<double>(newDuration) / 1000.0;
         factor = 1 - factor;
         const auto delta = static_cast<uint32_t>(factor * static_cast<double>(defaultTimerDuration));
         setTimersDuration(delta ? delta * 2 : defaultTimerDuration);
}

inline void Node::setPathParent(Node * newParent) noexcept {
         m_pathParent = newParent;
}

inline std::pair<uint32_t,uint32_t> Node::getCord() const noexcept {
         return m_currentLocation;
}

inline Node * Node::getPathParent() const noexcept {
         return m_pathParent;
}

inline Node::State Node::getType() const noexcept {
         return m_type;
}

inline void Node::setRunningState(const bool newAlgorithmState) noexcept {
         m_algorithmPaused = newAlgorithmState;
}

inline QRectF Node::boundingRect() const noexcept {
         return QRectF(0,0,dimension,dimension);
}

inline void Node::paint(QPainter * painter,const QStyleOptionGraphicsItem * ,QWidget * ) noexcept{
         if(m_type == State::Visited){
                  painter->setOpacity(opacity() / 2);
         }

         painter->setRenderHint(QPainter::Antialiasing);
         painter->drawPixmap(0,0,dimension,dimension,m_icon);
}

inline QSizeF Node::sizeHint(Qt::SizeHint , const QSizeF & ) const noexcept {
         return QSizeF {dimension,dimension};
}

inline void Node::setGeometry(const QRectF & geometry) noexcept{
         prepareGeometryChange();
         QGraphicsLayoutItem::setGeometry(geometry);
         setPos(geometry.topLeft());
}

#endif