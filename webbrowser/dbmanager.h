#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtSql>

class DbManager : public QObject
{
    Q_OBJECT

public:
    DbManager();
    ~DbManager();
    QList<QStringList*>* searchKeyword(QString text);
    bool insertUrl(QString url, QString title);

private:
    QList<QStringList*> *result;
    QSqlDatabase db;
};

#endif // DBMANAGER_H
