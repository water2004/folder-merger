#ifndef MYBUTTOM_H
#define MYBUTTOM_H

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>


class myButtom : public QPushButton
{
    Q_OBJECT
public:
    explicit myButtom(QWidget *parent = nullptr);
    QString *str;
    void setStr(QString *s);

signals:

protected:
    void dropEvent(QDropEvent*event);
    void dragEnterEvent(QDragEnterEvent *e);


};

#endif // MYBUTTOM_H
