#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	static QString noname;

public slots:
	void select_file();

private slots:
	void on_btnPlay_clicked();
	void on_btnStop_clicked();
};

#endif // MAINWINDOW_HPP
