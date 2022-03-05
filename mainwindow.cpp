#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QVector>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->source->setStr(&source);
    ui->target->setStr(&target);
    ui->target->type=2;
    ui->source->type=1;
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
        if(s.length()>20)
        {
            s="..."+s.mid(s.length()-20);
        }
        ui->source->setText("源\n"+s);
    }
}


void MainWindow::on_target_clicked()
{
    QString s=QFileDialog::getExistingDirectory(this,"选择目标");
    if(s!="")
    {
        target=s;
        if(s.length()>20)
        {
            s="..."+s.mid(s.length()-20);
        }
        ui->target->setText("目标\n"+s);
    }
}


void MainWindow::on_start_clicked()
{
    if(source==""||target=="")
    {
        QMessageBox::warning(this,"提示","请选择源和目标目录！");
        return;
    }
    scanT(target);
    qSort(files);
    if(useHash)
        for(int i=0;i<files.length();i++)
        {
            hashs<<files[i].hash;
        }
    preScanS(source);
    scanS(source);
    ui->statusBar->showMessage("完成");
    tot=0;
    copyfrom.clear();
    copyto.clear();
    hashs.clear();
    files.clear();
}

//计算文件哈希
QString MainWindow::fileMd5(QString sourceFilePath) {

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
    folder=folder.mid(target.length());
    QDir to(folder);
    if(!to.exists())
    {
        to=QDir(target);
        QStringList list=folder.split("/");
        for(int i=0;i<list.length();i++)
        {
            to.mkdir(list[i]);
            to.cd(list[i]);
        }
    }
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
        ui->statusBar->showMessage("扫描中"+fileInfo.filePath());
        if(fileInfo.isDir())
        {
            scanT(fileInfo.filePath());
        }
        else
        {
            if(!useHash)
                files<<myFile(fileInfo.fileName(),fileInfo.filePath(),fileInfo.lastModified(),"");
            else
            {
                files<<myFile(fileInfo.fileName(),fileInfo.filePath(),fileInfo.lastModified(),fileMd5(fileInfo.filePath()));
            }
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
            break;
        }
    }
    if(l<=r&&files[mid].name==v)
    {
        while(mid>=0&&files[mid].name==v) mid--;
        mid++;
        return mid;
    }
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
        else if(!useHash)
        {
            int f=find(fileInfo.fileName());
            if(f!=-1)
            {
                while(files[f].name==fileInfo.fileName())
                {
                    if(files[f].path.mid(target.length())==fileInfo.filePath().mid(source.length())) break;
                    f++;
                }
                if(files[f].name!=fileInfo.fileName()) f=-1;
            }
            QString toDir=target+fileInfo.filePath().mid(source.length());
            if(f==-1) //copyFileToPath(fileInfo.filePath(),toDir);
            {
                copyfrom<<fileInfo.filePath();
                copyto<<toDir;
                tot++;
            }
            else
            {
                if(fileInfo.lastModified()>files[f].time)
                {
                    QFile::remove(files[f].path);
                    files[f].time=fileInfo.lastModified();
                    copyfrom<<fileInfo.filePath();
                    copyto<<toDir;
                    tot++;
                }
            }
        }
        else
        {
            QString hash=fileMd5(fileInfo.filePath());
            if(hashs.contains(hash)) continue;
            int f=find(fileInfo.fileName());
            if(f!=-1)
            {
                while(files[f].name==fileInfo.fileName())
                {
                    if(files[f].path.mid(target.length())==fileInfo.filePath().mid(source.length())) break;
                    f++;
                }
                if(files[f].name!=fileInfo.fileName()) f=-1;
            }
            QString toDir=target+fileInfo.filePath().mid(source.length());
            if(f==-1) //copyFileToPath(fileInfo.filePath(),toDir);
            {
                copyfrom<<fileInfo.filePath();
                copyto<<toDir;
                tot++;
            }
            else
            {
                int i=1;
                QString in="-1";
                QString newN=files[f].path;
                int pos=0;
                while(pos<newN.length()&&newN[pos]!='.') pos++;
                newN.insert(pos,in);
                while(!QFile::rename(files[f].path,newN))
                {
                      i++;
                      newN[pos+1]='0'+i;
                }
                copyfrom<<fileInfo.filePath();
                copyto<<toDir;
                tot++;
            }
        }
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    useHash=ui->checkBox->isChecked();
}

