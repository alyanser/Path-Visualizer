#include <QApplication>
#include <QGraphicsView>
#include <QScreen>
#include <QFile>
#include <QLabel>
#include <QMovie>
#include <QIcon>
#include <QBrush>
#include "scene.hpp"
#include "node.hpp"

int main(int argc, char ** argv){
	QApplication app(argc, argv);
         {       
                  QFile file(":/styles/stylesheet.css");
                  file.open(QFile::ReadOnly);
                  app.setStyleSheet(file.readAll());
                  file.close();
         }
         auto mainRect = QApplication::primaryScreen()->availableSize();

         GraphicsScene scene(mainRect);
         QGraphicsView view(&scene);

         view.setWindowIcon(QIcon(":/pixmaps/icons/windowIcon.png"));
         view.setWindowTitle("Graph Algorithm Visualizer");
         
         QObject::connect(&scene,&GraphicsScene::close,&app,&QApplication::quit);

         // view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
         // view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

         QObject::connect(&scene,&GraphicsScene::changed,[&scene]{
                  auto rec = scene.itemsBoundingRect();
                  scene.setSceneRect(rec);
         });

         view.show();
	return app.exec();
}
