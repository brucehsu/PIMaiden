#include "dbmanager.h"
#include <QMessageBox>

DbManager::DbManager()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("history.db");
    result = new QList<QStringList*>();
    /*if(db.open()) {
        query.exec("CREATE TABLE history(id INTEGER PRIMARY KEY ASC, url UNIQUE, title, datetime)");
        QMessageBox::information(0, "Information",
                    query.lastError().text() + qApp->libraryPaths().at(0),
                    QMessageBox::Ok, QMessageBox::Ok);
    }*/
    if (!db.open()) {
            QMessageBox::critical(0, tr("Cannot open database"),
                tr("Unable to establish a database connection.\n"
                   "This example needs SQLite support. Please read "
                   "the Qt SQL driver documentation for information how "
                   "to build it."), QMessageBox::Cancel);
            return;
    }
    QSqlQuery query;
    query.exec("CREATE TABLE history(id INTEGER PRIMARY KEY ASC, url UNIQUE, title, datetime)");
}

QList<QStringList*>* DbManager::searchKeyword(QString text) {
    QSqlQuery query;
    query.exec("SELECT * FROM history WHERE url LIKE '%" + text +"%' OR title LIKE '%" + text + "%'");
    free(result);
    result = new QList<QStringList*>();
    QStringList *urlList = new QStringList();
    QStringList *titleList = new QStringList();
    result->append(urlList);
    result->append(titleList);
    while(query.next()) {
        urlList->append(query.value(1).toString());
        titleList->append(query.value(2).toString());
    }
    return result;
}

bool DbManager::insertUrl(QString url, QString title) {
    QSqlQuery query;
    bool x = query.exec("INSERT INTO history(url,title) VALUES (\'" + url + "\', \'" + title + "\')");
}
