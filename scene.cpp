#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGridLayout>
#include <QPushButton>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QWidget>
#include <QList>
#include <QStateMachine>
#include <QState>
#include "scene.hpp"

GraphicsScene::GraphicsScene(const QSize & size) : bar(new QTabWidget()), gridScene(new QGraphicsScene()){
         setSceneRect(0,0,size.width(),size.height());
         
         auto barProxy = addWidget(bar);
         bar->setFixedSize(size);
         populateBar();
}

GraphicsScene::~GraphicsScene(){}

void GraphicsScene::populateBar(){

         {
                  auto bfsWidget = new QWidget(bar);
                  const QString algoName = "BFS";
                  bar->addTab(bfsWidget,algoName);
                  populateWidget(bfsWidget,algoName);
         }

         {
                  auto dfsWidget = new QWidget(bar);
                  const QString algoName = "DFS";
                  bar->addTab(dfsWidget,algoName);
                  populateWidget(dfsWidget,algoName);
         }
}

void GraphicsScene::populateWidget(QWidget * widget,const QString & algoName){
         auto mainLayout = new QGridLayout(widget);
         auto view = new QGraphicsView(gridScene,widget);
         mainLayout->addWidget(view,0,0);

         {
                  auto sideLayout = new QVBoxLayout(); //!
                  mainLayout->addLayout(sideLayout,0,1);
                  sideLayout->setAlignment(Qt::AlignTop);

                  auto infoBut = new QPushButton(QString("About %1").arg(algoName),widget);
                  auto startStopBut = new QPushButton("Run",widget);
                  auto resetBut = new QPushButton("Reset",widget);

                  sideLayout->addWidget(infoBut);
                  sideLayout->addWidget(startStopBut);
                  sideLayout->addWidget(resetBut);
         }

}

