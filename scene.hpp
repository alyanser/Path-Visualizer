#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>

class QTabWidget;
class QSize;

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
public:
         GraphicsScene(const QSize & size);
         ~GraphicsScene();
private:
         QTabWidget * bar;
         QGraphicsScene * gridScene;

         ///
         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
signals:
         void close();

};

#endif