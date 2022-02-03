#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->source->setStr(&source);
    ui->target->setStr(&target);
    ui->source->setAcceptDrops(true);
    ui->target->setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_source_clicked()
{
    QString s=QFileDialog::getExistingDirectory(this,"选择源");
    if(s!="")
    {
        source=s;
        ui->source->setText("已选择-"+s);
    }
}


void MainWindow::on_target_clicked()
{
    QString s=QFileDialog::getExistingDirectory(this,"选择目标");
    if(s!="")
    {
        target=s;
        ui->target->setText("已选择-"+s);
    }
}


void MainWindow::on_start_clicked()
{
    scanT(target);
    qSort(files);
    preScanS(source);
    scanS(source);
}

//计算文件哈希
QString fileMd5(const QString &sourceFilePath) {

    QFile sourceFile(sourceFilePath);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 512000;//一次写入500K

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize];
        int bytesRead;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(QCryptographicHash::Md5);

        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}
//拷贝文件：
bool MainWindow::copyFileToPath(QString sourceDir ,QString toDir)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return false;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile = new QDir;
    int cnt=1;
    int point=toDir.length()-1;
    for(;point>=0&&toDir[point]!='/';point--)//寻找文件名的末尾，不包括扩展名
    {
        if(toDir[point]=='.') break;
    }
    if(point==0||toDir[point]=='/')//没有扩展名
    {
        point=toDir.length()-1;
    }
    int pos,len=0;
    for(int i=toDir.length()-1;i>=0;i--)
    {
        len++;
        if(toDir[i]=='/')
        {
            pos=i;
            break;
        }
    }
    QString folder=toDir;
    folder.remove(pos,len);
    QDir to(folder);
    if(!to.exists())
        to.mkdir(folder);
    QFile::copy(sourceDir, toDir);
    return true;
}

void MainWindow::scanT(QString dir)
{
    ui->statusBar->showMessage("扫描中-目标目录");
    QDir Dir(dir);
    if(!Dir.exists()) return;
    QFileInfoList fileInfoList = Dir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if(fileInfo.isDir())
        {
            scanT(fileInfo.filePath());
        }
        else
        {
            files<<myFile(fileInfo.fileName(),fileInfo.filePath(),fileInfo.lastModified(),"");
        }
    }
}

int MainWindow::find(QString v)
{
    int l=0,r=files.length()-1;
    int mid=(l+r)>>1;
    while(l<r)
    {
        if(files[mid].name<v)
        {
            l=mid+1;
            mid=(l+r)>>1;
        }
        else if(files[mid].name>v)
        {
            r=mid-1;
            mid=(l+r)>>1;
        }
        else
        {
            while(files[mid].name==v) mid--;
            break;
        }
    }
    if(l<=r&&files[mid].name==v)
        return mid;
    else return -1;
}

void MainWindow::scanS(QString dir)
{
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(tot-1);
    for(int i=0;i<tot;i++)
    {
        ui->statusBar->showMessage(copyfrom[i]);
        copyFileToPath(copyfrom[i],copyto[i]);
        ui->progressBar->setValue(i);
    }
}

void MainWindow::preScanS(QString dir)
{
    ui->statusBar->showMessage("扫描中-源");
    QDir Dir(dir);
    if(!Dir.exists()) return;
    QFileInfoList fileInfoList = Dir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if(fileInfo.isDir())
        {
            preScanS(fileInfo.filePath());
        }
        else
        {
            int f=find(fileInfo.fileName());
            QString toDir=target+fileInfo.filePath().mid(source.length());
            if(f==-1) //copyFileToPath(fileInfo.filePath(),toDir);
            {
                copyfrom<<fileInfo.filePath();
                copyto<<toDir;
                tot++;
            }
            else
            {
                int i=f;
                bool copy=true;
                while(i<files.length()&&files[i].name==fileInfo.fileName())
                {
                    if(fileInfo.lastModified()<=files[i].time)
                    {
                        copy=false;
                        break;
                    }
                    i++;
                }
                if(copy)
                {
                    while(f<files.length()&&files[f].name==fileInfo.fileName())
                    {
                        QFile::remove(files[f].path);
                        files[f].time=fileInfo.lastModified();
                        //copyFileToPath(fileInfo.filePath(),toDir);
                        copyfrom<<fileInfo.filePath();
                        copyto<<toDir;
                        tot++;
                        f++;
                    }
                }
            }
        }
    }
}
