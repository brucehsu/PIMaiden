#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebPage>
#include <QtGui/QToolBar>
#include <QtGui/QStatusBar>
#include <QNetworkReply>
#include <omnibox.h>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    void showUrl(QString url);

private:
    QGridLayout *layout;
    QToolBar *toolbar;
    QWebView *webview;
    QPushButton *prevBtn, *nextBtn, *refreshBtn;
    OmniBox *omnibox;

public slots:
    void setWebUrl();
    void setBtnStat();
    void handleWebPageError(QNetworkReply*);
};

#endif // MAINWINDOW_H
