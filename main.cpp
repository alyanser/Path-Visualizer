#include <QApplication>
#include <QScreen>
#include <QGraphicsView>
#include "scene.hpp"

int main(int argc, char ** argv){
	QApplication app(argc, argv);

         auto mainRect = QApplication::primaryScreen()->availableSize();

         GraphicsScene scene(mainRect);
         QGraphicsView view(&scene);

         view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
         view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
         view.setGeometry(0,0,mainRect.width(),mainRect.height());
         view.setMinimumSize(640,480);
         scene.setSceneRect(0,0,mainRect.width(),mainRect.height());

         view.show();
	return app.exec();
}
