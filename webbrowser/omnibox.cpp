#include "omnibox.h"
#include <QMessageBox>

OmniBox::OmniBox()
{
    list = new QListWidget(this);
    list->setWindowFlags(Qt::ToolTip);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    dbManager = new DbManager();

    QObject::connect(this,SIGNAL(textChanged(QString)),this,SLOT(showCompleter(QString)));
    QObject::connect(this,SIGNAL(editingFinished()),list,SLOT(hide()));
    QObject::connect(this,SIGNAL(editingFinished()),this,SLOT(setBackUrl()));
}

OmniBox::~OmniBox() {
    delete dbManager;
}

void OmniBox::keyPressEvent(QKeyEvent *event) {
    if(event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return) {
        if(list->currentRow()>=0) {
            if(list->currentRow()!=list->count()-1) {
                this->setText(candidateList->at(0)->at(list->currentRow()));
            } else {
                this->setText("http://www.google.com.tw/search?hl=zh-TW&q=" + this->text());
            }
        }
        if(this->text().startsWith("http://")!=true) {
            this->setText("http://" + this->text());
        }
        list->hide();
        this->webview->setUrl(QUrl(this->text()));
    } else if(event->key()==Qt::Key_Down) {
        if(list->currentRow()<0) {
            list->setCurrentRow(0);
        } else {
            if(list->currentRow()==list->count()-1) {
                list->setCurrentRow(0);
            } else {
                list->setCurrentRow(list->currentRow()+1);
            }
        }
    } else if(event->key()==Qt::Key_Up) {
        if(list->currentRow()<0) {
            list->setCurrentRow(list->count()-1);
        } else {
            if(list->currentRow()==0) {
                list->setCurrentRow(list->count()-1);
            } else {
                list->setCurrentRow(list->currentRow()-1);
            }
        }
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

void OmniBox::setWebView(QWebView *view) {
    this->webview = view;
}

void OmniBox::setUrl() {
    this->setText(QString::fromUtf8(webview->url().toEncoded()));
    dbManager->insertUrl(QString::fromUtf8(webview->url().toEncoded()),webview->title());
}

void OmniBox::setBackUrl() {
    this->setText(QString::fromUtf8(webview->url().toEncoded()));
}

void OmniBox::showCompleter(QString text) {
    if(QString::compare(this->text(),QString::fromUtf8(webview->url().toEncoded()))!=0) {
        list->setMinimumWidth(this->width());
        QPoint listPos(0,this->height());
        int glo_x = mapToGlobal(listPos).x();
        int glo_y = mapToGlobal(listPos).y() + 1;
        list->clear();
        candidateList = dbManager->searchKeyword(this->text());
        for(int i=0;i<candidateList->at(0)->size();i++) {
            QString urlString = candidateList->at(0)->at(i);
            if(urlString.length()>50) {
                urlString.resize(50);
                urlString.append("...");
            }
            list->addItem(urlString + " - " + candidateList->at(1)->at(i));
        }
        list->addItem("Search with Google: " + this->text());
        list->move(glo_x,glo_y);
        list->show();
    } else {
        list->hide();
    }
}

void OmniBox::paintProgress(int progress) {
    QColor loadingColor = QColor(116, 192, 250);
    QColor backgroundColor = loadingColor.lighter(160);
    QPalette pal = this->palette();
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0, loadingColor);
    gradient.setColorAt(((double)progress)/100, backgroundColor);
    pal.setBrush(QPalette::Base, gradient);
    this->setPalette(pal);
}

void OmniBox::cleanProgress() {
    QColor backgroundColor = QColor(255,255,255);
    QPalette pal = this->palette();
    pal.setBrush(this->backgroundRole(),backgroundColor);
    this->setPalette(pal);
}
