#ifndef WEBBROWSER_H
#define WEBBROWSER_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtGui/QToolBar>
#include <QtGui/QStatusBar>
#include <QNetworkReply>
#include <QLineEdit>
#include <omnibox.h>

class WebBrowser : public QWidget
{
    Q_OBJECT

public:
    WebBrowser();
    ~WebBrowser();
    void showUrl(QString url);

private:
    QGridLayout *layout;
    QToolBar *toolbar;
    QWebView *webview;
    QPushButton *prevBtn, *nextBtn, *refreshBtn,*closeSearchBarBtn,*shortenerBtn;
    OmniBox *omnibox;
    QLineEdit *searchBar;

public slots:
    void setWebUrl();
    void setBtnStat();
    void handleWebPageError(QNetworkReply*);
    void keyPressEvent(QKeyEvent *);
    void showSearchBar();
    void hideSearchBar();
    void findTextInPage();
    void shortenUrl();
};

#endif // WEBBROWSER
