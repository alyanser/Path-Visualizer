#include <QTabWidget>
#include <QEventTransition>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QWidget>
#include <QList>
#include <QStateMachine>
#include <QState>
#include "scene.hpp"
#include "PushButton.hpp"

GraphicsScene::GraphicsScene(const QSize & size) : bar(new QTabWidget()), gridScene(new QGraphicsScene()){
         setSceneRect(0,0,size.width(),size.height());

         addWidget(bar);
         bar->setFixedWidth(size.width());
         bar->setFixedHeight(size.height()-25);  //! fix it
         populateBar();
}

GraphicsScene::~GraphicsScene(){}

void GraphicsScene::populateBar(){

         {
                  auto bfsWidget = new QWidget(bar);
                  const QString algoName = "BFS";
                  const QString info = "This is BFS";
                  bar->addTab(bfsWidget,algoName);
                  populateWidget(bfsWidget,algoName,info);
         }

         {
                  auto dfsWidget = new QWidget(bar);
                  const QString algoName = "DFS";
                  const QString info = "This is DFS";
                  bar->addTab(dfsWidget,algoName);
                  populateWidget(dfsWidget,algoName,info);
         }
}

void GraphicsScene::populateWidget(QWidget * widget,const QString & algoName,const QString & infoText){
         auto mainLayout = new QGridLayout(widget);
         mainLayout->setAlignment(Qt::AlignTop);
         mainLayout->setSpacing(10);

         auto view = new QGraphicsView(gridScene,widget);
         mainLayout->addWidget(view,0,0);

         {        // right side bar
                  auto sideLayout = new QVBoxLayout(); //! memory check
                  mainLayout->addLayout(sideLayout,0,1);
                  sideLayout->setAlignment(Qt::AlignTop);

                  auto infoBut = new PushButton(QString("About %1").arg(algoName),widget);
                  auto statusBut = new PushButton("Run",widget);
                  auto resetBut = new PushButton("Reset",widget);
                  auto exitBut = new PushButton("Exit",widget);

                  sideLayout->addWidget(infoBut);
                  sideLayout->addWidget(statusBut);
                  sideLayout->addWidget(resetBut);
                  sideLayout->addWidget(exitBut);
                  sideLayout->insertSpacing(3,100);

                  {        // state machine for buttons
                           auto machine = new QStateMachine(widget);
                           auto statusStart = new QState(machine);
                           auto statusEnd = new QState(machine);

                           machine->setInitialState(statusStart);
                           statusStart->assignProperty(statusBut,"text","Run");
                           statusStart->assignProperty(statusBut,"bgColor",QColor(Qt::green));
                           statusEnd->assignProperty(statusBut,"text","Stop");
                           statusEnd->assignProperty(statusBut,"bgColor",QColor(Qt::red));

                           auto startToEnd = new QEventTransition(statusBut,QEvent::MouseButtonPress,machine);
                           auto endToStart = new QEventTransition(statusBut,QEvent::MouseButtonPress,machine);
                           auto colorAnimation = new QPropertyAnimation(statusBut,"bgColor",widget);
                           colorAnimation->setDuration(2500);

                           startToEnd->addAnimation(colorAnimation);
                           endToStart->addAnimation(colorAnimation);
                           startToEnd->setTargetState(statusEnd);
                           endToStart->setTargetState(statusStart);

                           statusStart->addTransition(startToEnd);
                           statusEnd->addTransition(endToStart);
                           // whenever staus button (RUN/STOP) is pressed it changes the text respectively
                           machine->start();
                  }

                  connect(infoBut,&QPushButton::clicked,[widget,infoText]{
                           QMessageBox::information(widget,"Information",infoText);
                  });

                  connect(statusBut,&QPushButton::clicked,[]{
                  });

                  connect(resetBut,&QPushButton::clicked,[]{

                  });

                  connect(exitBut,&QPushButton::clicked,[widget,this]{
                           auto choice = QMessageBox::question(widget,"Close","Quit",QMessageBox::No,QMessageBox::Yes);
                           if(choice == QMessageBox::Yes){
                                    emit close();
                           }
                  });
         }

         {        // bottom bar : displays the current algorithm status
                  auto bottomLayout = new QHBoxLayout(); //! memory
                  mainLayout->addLayout(bottomLayout,1,0);
                  bottomLayout->setAlignment(Qt::AlignLeft);

                  auto infoLine = new QLineEdit("Click on run button on sidebar to display algorithm status");
                  infoLine->setReadOnly(true);
                  bottomLayout->addWidget(infoLine);
         }
}

