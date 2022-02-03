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
    if(url=="") return;
    *str=url;
    if(url.length()>20)
    {
        url="..."+url.mid(url.length()-20);
    }
    if(type==1)
        this->setText("源\n"+url);
    else
        this->setText("目标\n"+url);
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
