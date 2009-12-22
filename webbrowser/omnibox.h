#ifndef OMNIBOX_H
#define OMNIBOX_H

#include <QLineEdit>
#include <QUrl>
#include <QKeyEvent>
#include <QWebView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHeaderView>
#include <dbmanager.h>

class OmniBox : public QLineEdit
{
        Q_OBJECT

public:
    OmniBox();
    ~OmniBox();
    void setWebView(QWebView *view);

private:
    QWebView *webview;
    QListWidget *list;
    DbManager *dbManager;
    QList<QStringList*>* candidateList;

protected:
    void keyPressEvent(QKeyEvent *event);

public slots:
    void setUrl();
    void showCompleter(QString);
    void setBackUrl();
    void paintProgress(int);
    void cleanProgress();
};

#endif // OMNIBOX_H
