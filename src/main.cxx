#include <QApplication>
#include <QGraphicsView>
#include <QScreen>
#include <QIcon>
#include <QFile>
#include <QRect>
#include "scene.hxx"

int main(int argc, char ** argv){
	QApplication app(argc, argv);

         {       
                  QFile file(":/styles/stylesheet.css");
                  file.open(QFile::ReadOnly);
                  app.setStyleSheet(file.readAll());
         }

         auto windowSize = QApplication::primaryScreen()->availableSize();

         GraphicsScene scene(windowSize);
         QGraphicsView view(&scene);

         view.setWindowIcon(QIcon(":/pixmaps/icons/windowIcon.png"));
         view.setWindowTitle("Path Visualizer");
         
         QObject::connect(&scene,&GraphicsScene::close,&app,&QApplication::quit);

         view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
         view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

         view.setFixedSize(windowSize);
         view.show();
         
	return QApplication::exec();
}