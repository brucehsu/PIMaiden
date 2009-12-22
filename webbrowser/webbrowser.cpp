#include "webbrowser.h"
#include <QtWebKit/QWebHistory>
#include <QMessageBox>

WebBrowser::WebBrowser()
{
    layout = new QGridLayout();

    prevBtn = new QPushButton();
    nextBtn = new QPushButton();
    refreshBtn = new QPushButton();
    closeSearchBarBtn = new QPushButton();
    shortenerBtn = new QPushButton();

    searchBar = new QLineEdit();

    prevBtn->setFixedSize(30,30);
    prevBtn->setFlat(true);
    prevBtn->setIcon(QIcon(":/prev.gif"));
    nextBtn->setFixedSize(30,30);
    nextBtn->setFlat(true);
    nextBtn->setIcon(QIcon(":/next.gif"));
    refreshBtn->setFixedSize(30,30);
    refreshBtn->setFlat(true);
    refreshBtn->setIcon(QIcon(":/refresh.gif"));
    closeSearchBarBtn->setFixedSize(30,30);
    closeSearchBarBtn->setIcon(QIcon(":/close.gif"));
    shortenerBtn->setFixedSize(30,30);
    shortenerBtn->setIcon(QIcon(":/shorten.png"));
    shortenerBtn->setToolTip("Google URL Shortener");

    this->hideSearchBar();

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
    layout->addWidget(shortenerBtn,0,7,1,1);
    layout->addWidget(webview,1,0,4,8);
    layout->addWidget(closeSearchBarBtn,5,0,1,1);
    layout->addWidget(searchBar,5,1,1,7);

    QObject::connect(webview,SIGNAL(titleChanged(QString)),this,SLOT(setWindowTitle(QString)));
    QObject::connect(prevBtn,SIGNAL(pressed()),webview,SLOT(back()));
    QObject::connect(nextBtn,SIGNAL(pressed()),webview,SLOT(forward()));
    QObject::connect(refreshBtn,SIGNAL(pressed()),webview,SLOT(reload()));
    QObject::connect(closeSearchBarBtn,SIGNAL(pressed()),this,SLOT(hideSearchBar()));
    QObject::connect(shortenerBtn,SIGNAL(clicked()),this,SLOT(shortenUrl()));
    QObject::connect(webview,SIGNAL(loadFinished(bool)),this,SLOT(setBtnStat()));
    //QObject::connect(webview,SIGNAL(loadFinished(bool)),omnibox,SLOT(setUrl()));
    QObject::connect(webview,SIGNAL(loadFinished(bool)),omnibox,SLOT(cleanProgress()));
    QObject::connect(webview->page()->networkAccessManager(),SIGNAL(finished(QNetworkReply*)),this,SLOT(handleWebPageError(QNetworkReply*)));
    QObject::connect(webview,SIGNAL(loadProgress(int)),omnibox,SLOT(paintProgress(int)));
    QObject::connect(searchBar,SIGNAL(textChanged(QString)),this,SLOT(findTextInPage()));
    QObject::connect(searchBar,SIGNAL(returnPressed()),this,SLOT(findTextInPage()));

    webview->setUrl(QUrl("http://www.google.com.tw"));

    this->setLayout(layout);
    this->resize(800,600);
}

WebBrowser::~WebBrowser()
{
    delete omnibox;
}

void WebBrowser::setWebUrl() {
    this->webview->setUrl(QUrl(this->omnibox->text()));
}

void WebBrowser::setBtnStat() {
    this->prevBtn->setEnabled(this->webview->history()->canGoBack());
    this->nextBtn->setEnabled(this->webview->history()->canGoForward());
}

void WebBrowser::showUrl(QString url) {
    if(!url.startsWith("http://")) url = "http://" + url;
    this->webview->setUrl(QUrl(url));
    this->show();
}

void WebBrowser::handleWebPageError(QNetworkReply *reply) {
    if(reply->error()==QNetworkReply::ContentNotFoundError || reply->error()==QNetworkReply::HostNotFoundError) {
        this->webview->setUrl(QUrl("qrc:/404.html"));
    } else if(reply->error()==QNetworkReply::NoError) {
        omnibox->setUrl();
    }
}

void WebBrowser::hideSearchBar() {
    this->searchBar->setVisible(false);
    this->closeSearchBarBtn->setVisible(false);
}

void WebBrowser::showSearchBar() {
    this->searchBar->setVisible(true);
    this->closeSearchBarBtn->setVisible(true);
}

void WebBrowser::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_F:
            if(event->modifiers()==Qt::ControlModifier) {
                this->showSearchBar();
                this->searchBar->setFocus();
            }
            break;
        case Qt::Key_Escape:
            this->hideSearchBar();
            break;
        case Qt::Key_Plus :
            this->webview->setZoomFactor(this->webview->zoomFactor()+0.1);
            break;
        case Qt::Key_Minus :
            this->webview->setZoomFactor(this->webview->zoomFactor()-0.1);
            break;
    }
}

void WebBrowser::findTextInPage() {
    QPalette pal = this->searchBar->palette();
    QColor foundColor = QColor(0,180,50);
    foundColor = foundColor.lighter(200);
    QColor notFoundColor = QColor(250,0,0);
    notFoundColor = notFoundColor.lighter(170);
    if(this->searchBar->text()=="") {
        pal.setBrush(searchBar->backgroundRole(),QBrush(QColor(255,255,255)));
        this->searchBar->setPalette(pal);
        return;
    }
    if(webview->findText(this->searchBar->text(),QWebPage::FindWrapsAroundDocument)) {
        pal.setBrush(searchBar->backgroundRole(),QBrush(foundColor));
    } else {
        pal.setBrush(searchBar->backgroundRole(),QBrush(notFoundColor));
    }
    this->searchBar->setPalette(pal);
}

void WebBrowser::shortenUrl() {
    this->webview->page()->mainFrame()->evaluateJavaScript("javascript:window.googl_callback = function(response){if(response.error_message){alert(\"An error occured: \" + response.error_message);}else{prompt(\"Copy following shortened URL: \" , response.short_url);}}; var s = document.createElement(\"script\"); s.src = \"http://ggl-shortener.appspot.com/?url=\" + encodeURIComponent(window.location.href) + \"&jsonp=googl_callback\"; void(document.body.appendChild(s));");
}
