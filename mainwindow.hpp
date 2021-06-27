#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class mainwindow : public QMainWindow{
Q_OBJECT

public:
	mainwindow(QWidget * parent = nullptr);
	~mainwindow();
};

#endif // MAINWINDOW_HPP
