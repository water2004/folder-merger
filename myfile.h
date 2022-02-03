#ifndef MYFILE_H
#define MYFILE_H
#include <QString>
#include <QDateTime>

class myFile
{
public:
    myFile(QString name1,QString path1,QDateTime time1,QString hash1);
    QString name;
    QString path;
    QDateTime time;
    QString hash;
    const bool operator <(myFile b) const
    {
        return name<b.name;
    }
    const bool operator ==(myFile b) const
    {
        return name==b.name;
    }
    const bool operator >(myFile b) const
    {
        return name>b.name;
    }
    void operator =(myFile b)
    {
        name=b.name;
        path=b.path;
        time=b.time;
        hash=b.hash;
    }

};
#endif // MYFILE_H
