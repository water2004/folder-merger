#include "mybuttom.h"
#include <QDragEnterEvent>
myButtom::myButtom(QWidget *parent) : QPushButton(parent)
{
    this->setAcceptDrops(true);
}
void myButtom::dropEvent(QDropEvent *event)
{
    const QMimeData*qm=event->mimeData();//获取MIMEData
    QString url=qm->urls()[0].toLocalFile();
    *str=url;
}

void myButtom::setStr(QString *s)
{
    str=s;
}


void myButtom::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction(); //可以在这个窗口部件上拖放对象
}
