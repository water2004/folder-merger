#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSet>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QVector>
#include <myfile.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString source="",target="";
    QString fileMd5(const QString &sourceFilePath);
    bool copyFileToPath(QString sourceDir ,QString toDir);
    void scanS(QString dir);
    QVector<myFile> files;
    int find(QString v);
    void scanT(QString dir);
    void preScanS(QString dir);
    QVector<QString> copyfrom;
    QVector<QString> copyto;
    int tot=0;

private slots:
    void on_source_clicked();

    void on_target_clicked();

    void on_start_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
