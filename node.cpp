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

Node::Node(const uint32_t row,const uint32_t col,QGraphicsItem * parent) : QGraphicsObject(parent), currentLocation(std::make_pair(row,col)), 
         backwardTimer(std::make_unique<QTimeLine>()), forwardTimer(std::make_unique<QTimeLine>())
{
         setGraphicsItem(this);
         setAcceptDrops(true);
         setAcceptHoverEvents(true);

         configureBackwardTimer();
         configureForwardTimer();
         setTimersDuration(defaultTimerDuration);

         constexpr bool startTimer = false;
         setType(type,startTimer);
}

Node::~Node(){}

void Node::setRunningState(const bool newAlgorithmState){
         algorithmPaused = newAlgorithmState;
}

QRectF Node::boundingRect() const {
         return QRectF(0,0,dimension,dimension);
}

void Node::paint(QPainter * painter,const QStyleOptionGraphicsItem * ,QWidget * ){
         if(type == State::Visited){
                  painter->setOpacity(opacity() / 2);
         }
         painter->setRenderHint(QPainter::Antialiasing);
         painter->drawPixmap(0,0,dimension,dimension,icon);
}

QSizeF Node::sizeHint(Qt::SizeHint , const QSizeF & ) const {
         return QSizeF {dimension,dimension};
}

void Node::setGeometry(const QRectF & geometry){
         prepareGeometryChange();
         QGraphicsLayoutItem::setGeometry(geometry);
         setPos(geometry.topLeft());
}

void Node::configureBackwardTimer(){
         backwardTimer->setDirection(QTimeLine::Backward);
         backwardTimer->setFrameRange(70,100);
         backwardTimer->setEasingCurve(QEasingCurve(QEasingCurve::InQuad));

         connect(backwardTimer.get(),&QTimeLine::frameChanged,[this](int32_t delta){
                  const qreal converted = delta / 100.0;
                  setOpacity(converted);
                  setScale(converted);
                  update();
         });

         connect(backwardTimer.get(),&QTimeLine::finished,[&forwardTimer = forwardTimer]{
                  if(forwardTimer->state() == QTimeLine::NotRunning){
                           forwardTimer->start();
                  }
         });
}

void Node::configureForwardTimer(){
         forwardTimer->setDirection(QTimeLine::Forward);
         forwardTimer->setFrameRange(70,100);
         forwardTimer->setEasingCurve(QEasingCurve(QEasingCurve::InQuad));

         connect(&*forwardTimer,&QTimeLine::frameChanged,[this](int32_t delta){
                  const qreal converted = delta / 100.0;
                  setOpacity(converted);
                  setScale(converted);
                  update();
         });
}

void Node::setType(const State newType,const bool startTimer){
         type = newType;
         bool acceptDrag = true;
         if(type == State::Source){
                  acceptDrag = false;
                  emit sourceSet(); // update source node in GraphicsScene class
         }else if(type == State::Target){
                  acceptDrag = false;
                  emit targetSet(); // update target node in GraphicsScene class
         }
         setAcceptDrops(acceptDrag); 
         undoNodeRotation(); 
         
         switch(type){
                  case State::Source : icon.load(":/pixmaps/icons/source.png"); break;
                  case State::Target : icon.load(":/pixmaps/icons/target.png"); break;
                  case State::Active : {
                           icon.load(":/pixmaps/icons/active.png");
                           setNodeRotation();
                           break;
                  }
                  case State::Inactive : icon.load(":/pixmaps/icons/inactive.png"); break;
                  case State::Visited : icon.load(":/pixmaps/icons/inactive.png"); break;
                  case State::Block : icon.load(":/pixmaps/icons/block.png"); break;
                  case State::Inpath : icon.load(":/pixmaps/icons/inpath.png"); break;
                  default : assert(false);
         }
         
         if(startTimer && backwardTimer->state() == QTimeLine::NotRunning){
                  backwardTimer->start();
         }else{
                  update();
         }
}

void Node::setNodeRotation(){
         if(pathParent){
                  const auto [selfX,selfY] = getCord();
                  const auto [parentX,parentY] = pathParent->getCord();
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

void Node::undoNodeRotation(){
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
                  }
         }
}

void Node::changeAnimationDuration(const uint32_t newDuration) const {
         // new duration comes from slider direclty range : 0 - 1000
         double factor = static_cast<double>(newDuration) / 1000.0;
         assert(factor <= 1.0 && factor >= 0.0);
         factor = 1 - factor;
         auto delta = static_cast<uint32_t>(factor * static_cast<double>(defaultTimerDuration));
         setTimersDuration(delta ? delta * 2 : defaultTimerDuration);
}

void Node::setPathParent(Node * newParent){
         pathParent = newParent;
}

std::pair<uint32_t,uint32_t> Node::getCord() const {
         return currentLocation;
}

Node * Node::getPathParent() const {
         return pathParent;
}

Node::State Node::getType() const {
         return type;
}

void Node::dragEnterEvent(QGraphicsSceneDragDropEvent * event){
         const auto * mimeData = event->mimeData();
         if(mimeData->hasText()){
                  const QString & text = mimeData->text();
                  if(text == "inverter"){
                           switch(type){
                                    case State::Source : [[fallthrough]];
                                    case State::Target : break;
                                    case State::Block : setType(State::Inactive); event->accept(); break;
                                    default : setType(State::Block); event->accept();
                           }
                  }
         }
         QGraphicsObject::dragEnterEvent(event);
}

void Node::dropEvent(QGraphicsSceneDragDropEvent * event){
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

void Node::setTimersDuration(const uint32_t newDuration) const {
         forwardTimer->setDuration(std::max<int32_t>(defaultTimerDuration,newDuration));
         backwardTimer->setDuration(std::max<int32_t>(defaultTimerDuration,newDuration));
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event){
         if(algorithmPaused && type == State::Source) return; 
         auto * dragger = new QDrag(this);
         auto * mimeData = new QMimeData(); // event deletes
         dragger->setMimeData(mimeData);
         
         switch(type){
                  case State::Source : {
                           dragger->setPixmap(icon);
                           mimeData->setText("fromSource");
                           if(dragger->exec()){
                                    setType(State::Inactive);
                           }
                           break;
                  }
                  case State::Target : {
                           dragger->setPixmap(icon);
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