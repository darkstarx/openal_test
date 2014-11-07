#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "SoundManager.hpp"
#include "log.hpp"


QString MainWindow::noname = tr("noname");

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->lblFileName->setText(noname);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::select_file()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Ogg files (*.ogg)"));
	if (fileName.count() == 0)
		ui->lblFileName->setText(noname);
	else
		ui->lblFileName->setText(fileName);
}

void MainWindow::on_btnPlay_clicked()
{
	std::string filename(ui->lblFileName->text().toUtf8().constData());
	sound_mgr().preload_sound(filename, filename);
	sound_mgr().play_sound(filename);
}

void MainWindow::on_btnStop_clicked()
{
	std::string filename(ui->lblFileName->text().toUtf8().constData());
	sound_mgr().stop_sound(filename);
}
