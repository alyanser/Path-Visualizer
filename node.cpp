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

Node::Node(int row,int col,QGraphicsItem * parent) : QGraphicsObject(parent), currentLocation{row,col}{
         backwardTimer = std::make_unique<QTimeLine>();
         forwardTimer = std::make_unique<QTimeLine>();

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
         if(type == Visited){
                  painter->setOpacity(opacity() / 2);
         }
         painter->setRenderHint(QPainter::Antialiasing);
         painter->drawPixmap(0,0,dimension,dimension,icon);
}

QSizeF Node::sizeHint(Qt::SizeHint , const QSizeF & ) const {
         return QSizeF(dimension,dimension);
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

         connect(backwardTimer.get(),&QTimeLine::frameChanged,[this](int delta){
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

         connect(&*forwardTimer,&QTimeLine::frameChanged,[this](int delta){
                  const qreal converted = delta / 100.0;
                  setOpacity(converted);
                  setScale(converted);
                  update();
         });
}

void Node::setType(const State newType,const bool startTimer){
         type = newType;
         bool acceptDrag = true;
         if(type == Source){
                  acceptDrag = false;
                  emit sourceSet(); // update source node in GraphicsScene class
         }else if(type == Target){
                  acceptDrag = false;
                  emit targetSet(); // update target node in GraphicsScene class
         }
         setAcceptDrops(acceptDrag); 
         undoNodeRotation(); 
         
         switch(type){
                  case Source : icon.load(":/pixmaps/icons/source.png"); break;
                  case Target : icon.load(":/pixmaps/icons/target.png"); break;
                  case Active : {
                           icon.load(":/pixmaps/icons/active.png");
                           setNodeRotation();
                           break;
                  }
                  case Inactive : icon.load(":/pixmaps/icons/inactive.png"); break;
                  case Visited : icon.load(":/pixmaps/icons/inactive.png"); break;
                  case Block : icon.load(":/pixmaps/icons/block.png"); break;
                  case Inpath : icon.load(":/pixmaps/icons/inpath.png"); break;
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
         int currentRotation = rotation();
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
         double factor = static_cast<double>(newDuration / 1000.0);
         assert(factor <= 1 && factor >= 0);
         factor = 1 - factor;
         uint32_t delta = static_cast<uint32_t>(factor * defaultTimerDuration);
         setTimersDuration(delta ? delta * 2 : defaultTimerDuration);
}

void Node::setPathParent(Node * newParent){
         pathParent = newParent;
}

std::pair<int,int> Node::getCord() const {
         return currentLocation;
}

Node * Node::getPathParent() const {
         return pathParent;
}

Node::State Node::getType() const {
         return type;
}

void Node::dragEnterEvent(QGraphicsSceneDragDropEvent * event){
         auto mimeData = event->mimeData();
         if(mimeData->hasText()){
                  const QString & text = mimeData->text();
                  if(text == "inverter"){
                           switch(type){
                                    case Source | Target : break;
                                    case Block : setType(Inactive);event->accept(); break;
                                    default : setType(Block);event->accept();
                           }
                  }
         }
         QGraphicsObject::dragEnterEvent(event);
}

void Node::dropEvent(QGraphicsSceneDragDropEvent * event){
         auto mimeData = event->mimeData();
         if(mimeData->hasText()){
                  const QString & indicator = mimeData->text();

                  if(indicator == "fromSource"){
                           setType(Source);
                           event->accept();
                  }else if(indicator == "fromTarget"){
                           setType(Target);
                           event->accept();
                  }
         }
         QGraphicsItem::dropEvent(event);
}

void Node::setTimersDuration(const uint32_t newDuration) const {
         forwardTimer->setDuration(std::max(defaultTimerDuration,newDuration));
         backwardTimer->setDuration(std::max(defaultTimerDuration,newDuration));
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event){
         if(algorithmPaused && type == Source) return; 
         auto dragger = new QDrag(this);
         auto mimeData = new QMimeData();
         dragger->setMimeData(mimeData);
         
         switch(type){
                  case Source : {
                           dragger->setPixmap(icon);
                           mimeData->setText("fromSource");
                           if(dragger->exec()){
                                    setType(Inactive);
                           }
                           break;
                  }
                  case Target : {
                           dragger->setPixmap(icon);
                           mimeData->setText("fromTarget");
                           if(dragger->exec()){
                                    setType(Inactive);
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