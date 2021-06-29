#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>

class QTabWidget;
class QSize;
class QLineEdit;

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
public:
         GraphicsScene(const QSize & size);
         ~GraphicsScene();
private:
         QTabWidget * bar;
         QGraphicsScene * gridScene;
         QLineEdit * infoLine;

         ///
         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene();
signals:
         void close();
};

#endif