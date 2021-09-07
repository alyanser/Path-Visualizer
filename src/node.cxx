#include <QGraphicsScene>
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QTimeLine>
#include <QMouseEvent>
#include "node.hpp"

Node::Node(const uint32_t row,const uint32_t col,QGraphicsItem * parent) : QGraphicsObject(parent), m_currentLocation(std::make_pair(row,col)){
         setGraphicsItem(this);
         setAcceptDrops(true);
         setAcceptHoverEvents(true);

         configureBackwardTimer();
         configureForwardTimer();
         setTimersDuration(defaultTimerDuration);

         setType(m_type,true);
}

void Node::setRunningState(const bool newAlgorithmState) noexcept {
         m_algorithmPaused = newAlgorithmState;
}

QRectF Node::boundingRect() const noexcept {
         return QRectF(0,0,dimension,dimension);
}

void Node::paint(QPainter * painter,const QStyleOptionGraphicsItem * ,QWidget * ) noexcept{
         if(m_type == State::Visited){
                  painter->setOpacity(opacity() / 2);
         }

         painter->setRenderHint(QPainter::Antialiasing);
         painter->drawPixmap(0,0,dimension,dimension,m_icon);
}

QSizeF Node::sizeHint(Qt::SizeHint , const QSizeF & ) const noexcept {
         return QSizeF {dimension,dimension};
}

void Node::setGeometry(const QRectF & geometry) noexcept{
         prepareGeometryChange();
         QGraphicsLayoutItem::setGeometry(geometry);
         setPos(geometry.topLeft());
}

void Node::configureBackwardTimer() noexcept {
         m_backwardTimer->setDirection(QTimeLine::Backward);
         m_backwardTimer->setFrameRange(70,100);
         m_backwardTimer->setEasingCurve(QEasingCurve(QEasingCurve::InQuad));

         connect(m_backwardTimer.get(),&QTimeLine::frameChanged,[this](int32_t delta){
                  const qreal converted = delta / 100.0;
                  setOpacity(converted);
                  setScale(converted);
                  update();
         });

         connect(m_backwardTimer.get(),&QTimeLine::finished,[&m_forwardTimer = m_forwardTimer]{
                  if(m_forwardTimer->state() == QTimeLine::NotRunning){
                           m_forwardTimer->start();
                  }
         });
}

void Node::configureForwardTimer() noexcept{
         m_forwardTimer->setDirection(QTimeLine::Forward);
         m_forwardTimer->setFrameRange(70,100);
         m_forwardTimer->setEasingCurve(QEasingCurve(QEasingCurve::InQuad));

         connect(m_forwardTimer.get(),&QTimeLine::frameChanged,[this](int32_t delta){
                  const qreal converted = delta / 100.0;
                  setOpacity(converted);
                  setScale(converted);
                  update();
         });
}

void Node::setType(const State newType,const bool startTimer) noexcept{
         m_type = newType;
         bool acceptDrag = true;

         if(m_type == State::Source){
                  acceptDrag = false;
                  emit sourceSet(); // update source node in GraphicsScene class
         }else if(m_type == State::Target){
                  acceptDrag = false;
                  emit targetSet(); // update target node in GraphicsScene class
         }

         setAcceptDrops(acceptDrag); 
         undoNodeRotation(); 
         
         switch(m_type){
                  case State::Source : m_icon.load(":/pixmaps/icons/source.png"); break;
                  case State::Target : m_icon.load(":/pixmaps/icons/target.png"); break;
                  case State::Active : {
                           m_icon.load(":/pixmaps/icons/active.png");
                           setNodeRotation();
                           break;
                  }
                  case State::Inactive : m_icon.load(":/pixmaps/icons/inactive.png"); break;
                  case State::Visited : m_icon.load(":/pixmaps/icons/inactive.png"); break;
                  case State::Block : m_icon.load(":/pixmaps/icons/block.png"); break;
                  case State::Inpath : m_icon.load(":/pixmaps/icons/inpath.png"); break;
                  default : __builtin_unreachable();
         }
         
         if(startTimer && m_backwardTimer->state() == QTimeLine::NotRunning){
                  m_backwardTimer->start();
         }else{
                  update();
         }
}

void Node::setNodeRotation() noexcept {
         if(m_pathParent){
                  const auto [selfX,selfY] = getCord();
                  const auto [parentX,parentY] = m_pathParent->getCord();

                  if(selfX != parentX){ // X axis changed
                           if(selfX < parentX){ // top of parent
                                    moveBy(halfDimension,halfDimension);
                                    setRotation(180);
                                    moveBy(halfDimension,halfDimension);
                           }
                  }else{ // Y axis changed
                           moveBy(halfDimension,halfDimension);
                           if(selfY < parentY){ // left of parent
                                    setRotation(90);
                                    moveBy(halfDimension ,-halfDimension);
                           }else{ // opposite
                                    setRotation(270);
                                    moveBy(-halfDimension,halfDimension);
                           }
                  }
         }
}

void Node::undoNodeRotation() noexcept {
         const auto currentRotation = static_cast<int32_t>(rotation());

         switch(currentRotation){
                  case 180 : {
                           moveBy(-halfDimension,-halfDimension);
                           setRotation(0);
                           moveBy(-halfDimension,-halfDimension);
                           break;
                  }

                  case 270 : {
                           moveBy(halfDimension,-halfDimension);
                           setRotation(0);
                           moveBy(-halfDimension,-halfDimension);
                           break;
                  }

                  case 90 : {
                           moveBy(-halfDimension,halfDimension);
                           setRotation(0);
                           moveBy(-halfDimension,-halfDimension);
                           break;
                  }
         }
}

void Node::changeAnimationDuration(const uint32_t newDuration) const noexcept {
         // new duration comes from slider direclty range : 0 - 1000
         double factor = static_cast<double>(newDuration) / 1000.0;
         assert(factor <= 1.0 && factor >= 0.0);
         factor = 1 - factor;
         const auto delta = static_cast<uint32_t>(factor * static_cast<double>(defaultTimerDuration));
         setTimersDuration(delta ? delta * 2 : defaultTimerDuration);
}

void Node::setPathParent(Node * newParent) noexcept {
         m_pathParent = newParent;
}

std::pair<uint32_t,uint32_t> Node::getCord() const noexcept {
         return m_currentLocation;
}

Node * Node::getPathParent() const noexcept {
         return m_pathParent;
}

Node::State Node::getType() const noexcept {
         return m_type;
}

void Node::dragEnterEvent(QGraphicsSceneDragDropEvent * event) noexcept {
         const auto * mimeData = event->mimeData();

         if(mimeData->hasText()){
                  const QString & text = mimeData->text();

                  if(text == "inverter"){
                           switch(m_type){
                                    case State::Source : [[fallthrough]];
                                    case State::Target : break;
                                    case State::Block : setType(State::Inactive); event->accept(); break;
                                    default : setType(State::Block); event->accept();
                           }
                  }
         }

         QGraphicsObject::dragEnterEvent(event);
}

void Node::dropEvent(QGraphicsSceneDragDropEvent * event) noexcept {
         const auto * mimeData = event->mimeData();

         if(mimeData->hasText()){
                  const QString & indicator = mimeData->text();

                  if(indicator == "fromSource"){
                           setType(State::Source);
                           event->accept();
                  }else if(indicator == "fromTarget"){
                           setType(State::Target);
                           event->accept();
                  }
         }

         QGraphicsItem::dropEvent(event);
}

void Node::setTimersDuration(const uint32_t newDuration) const noexcept {
         m_forwardTimer->setDuration(static_cast<int32_t>(std::max<uint32_t>(defaultTimerDuration,newDuration)));
         m_backwardTimer->setDuration(static_cast<int32_t>(std::max<uint32_t>(defaultTimerDuration,newDuration)));
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event) noexcept {
         if(m_algorithmPaused && m_type == State::Source) return; 

         auto * dragger = new QDrag(this);
         auto * mimeData = new QMimeData();

         dragger->setMimeData(mimeData);
         
         switch(m_type){
                  case State::Source : {
                           dragger->setPixmap(m_icon);
                           mimeData->setText("fromSource");
                           if(dragger->exec()){
                                    setType(State::Inactive);
                           }
                           break;
                  }

                  case State::Target : {
                           dragger->setPixmap(m_icon);
                           mimeData->setText("fromTarget");
                           if(dragger->exec()){
                                    setType(State::Inactive);
                           }
                           break;
                  }
                  
                  default  : {
                           mimeData->setText("inverter");
                           dragger->exec();
                  }
         }

         QGraphicsItem::mousePressEvent(event);
}