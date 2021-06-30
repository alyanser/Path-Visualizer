#include <QApplication>
#include <QGraphicsView>
#include <QScreen>
#include <QFile>
#include "scene.hpp"

#include "node.hpp"

int main(int argc, char ** argv){
	QApplication app(argc, argv);

         {        // set overall stylesheet     
                  QFile file(":/styles/stylesheet.css");
                  file.open(QFile::ReadOnly);
                  app.setStyleSheet(file.readAll());
                  file.close();
         }
         // window size
         auto mainRect = QApplication::primaryScreen()->availableSize();

         GraphicsScene scene(mainRect);
         QGraphicsView view(&scene);

         QObject::connect(&scene,&GraphicsScene::close,&app,&QApplication::quit);

         view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
         view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
         view.setFixedSize(mainRect);
         view.show();
         
	return app.exec();
}
