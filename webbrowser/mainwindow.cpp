#include "mainwindow.h"
#include <QtWebKit/QWebHistory>
#include <QMessageBox>

MainWindow::MainWindow()
{
    layout = new QGridLayout();

    prevBtn = new QPushButton();
    nextBtn = new QPushButton();
    refreshBtn = new QPushButton();

    prevBtn->setFixedSize(30,30);
    prevBtn->setFlat(true);
    prevBtn->setIcon(QIcon("icons/prev.gif"));
    nextBtn->setFixedSize(30,30);
    nextBtn->setFlat(true);
    nextBtn->setIcon(QIcon("icons/next.gif"));
    refreshBtn->setFixedSize(30,30);
    refreshBtn->setFlat(true);
    refreshBtn->setIcon(QIcon("icons/refresh.gif"));

    webview = new QWebView();
    webview->page()->settings()->setAttribute(QWebSettings::PluginsEnabled, true);

    omnibox = new OmniBox();

    omnibox->setFixedHeight(30);
    omnibox->setWebView(webview);

    layout->setMargin(1);
    layout->addWidget(prevBtn,0,0);
    layout->addWidget(nextBtn,0,1);
    layout->addWidget(refreshBtn,0,2);
    layout->addWidget(omnibox,0,3,1,4);
    layout->addWidget(webview,1,0,4,7);

    QObject::connect(webview,SIGNAL(titleChanged(QString)),this,SLOT(setWindowTitle(QString)));
    QObject::connect(prevBtn,SIGNAL(pressed()),webview,SLOT(back()));
    QObject::connect(nextBtn,SIGNAL(pressed()),webview,SLOT(forward()));
    QObject::connect(refreshBtn,SIGNAL(pressed()),webview,SLOT(reload()));
    QObject::connect(webview,SIGNAL(loadFinished(bool)),this,SLOT(setBtnStat()));
    //QObject::connect(webview,SIGNAL(loadFinished(bool)),omnibox,SLOT(setUrl()));
    QObject::connect(webview,SIGNAL(loadFinished(bool)),omnibox,SLOT(cleanProgress()));
    QObject::connect(webview->page()->networkAccessManager(),SIGNAL(finished(QNetworkReply*)),this,SLOT(handleWebPageError(QNetworkReply*)));
    QObject::connect(webview,SIGNAL(loadProgress(int)),omnibox,SLOT(paintProgress(int)));

    webview->setUrl(QUrl("http://www.google.com.tw"));

    this->setLayout(layout);
    this->resize(800,600);
}

MainWindow::~MainWindow()
{

}

void MainWindow::setWebUrl() {
    this->webview->setUrl(QUrl(this->omnibox->text()));
}

void MainWindow::setBtnStat() {
    this->prevBtn->setEnabled(this->webview->history()->canGoBack());
    this->nextBtn->setEnabled(this->webview->history()->canGoForward());
}

void MainWindow::showUrl(QString url) {
    this->show();
    this->webview->setUrl(QUrl(url));
}

void MainWindow::handleWebPageError(QNetworkReply *reply) {
    if(reply->error()==QNetworkReply::ContentNotFoundError || reply->error()==QNetworkReply::HostNotFoundError) {
        this->webview->setUrl(QUrl("404.html"));
    } else if(reply->error()==QNetworkReply::NoError) {
        omnibox->setUrl();
    }
}
