#include "ftpclient.h"

FtpClient::FtpClient(QWidget *parent)
    : QWidget(parent)
{
    layout = new QGridLayout();

    localFileView = new QTreeWidget();
    remoteFileView = new QTreeWidget();

    uploadBtn = new QPushButton();
    downloadBtn = new QPushButton();
    deleteBtn = new QPushButton();

    localDirEdit = new QLineEdit();
    remoteDirEdit = new QLineEdit();

    transferProgressBar = new QProgressBar();

    msg = new QTextEdit();

    ftpConnection = new QFtp(this);

    uploadBtn->setMaximumWidth(30);
    uploadBtn->setIcon(QIcon(":/upload.png"));
    uploadBtn->setToolTip("Upload");
    downloadBtn->setMaximumWidth(30);
    downloadBtn->setIcon(QIcon(":/download.png"));
    downloadBtn->setToolTip("Download");
    deleteBtn->setMaximumWidth(30);
    deleteBtn->setIcon(QIcon(":/delete.png"));
    deleteBtn->setToolTip("Delete");

    transferProgressBar->setTextVisible(false);

    msg->setTextInteractionFlags(Qt::TextSelectableByKeyboard);

    QStringList header;
    header <<  "File Name" << "File Size" << "Last modified time";
    localFileView->setRootIsDecorated(false);
    localFileView->setColumnCount(3);
    localFileView->header()->setMovable(false);
    localFileView->setHeaderLabels(header);
    localFileView->header()->setClickable(true);
    remoteFileView->setRootIsDecorated(false);
    remoteFileView->header()->setMovable(false);
    remoteFileView->setColumnCount(3);
    remoteFileView->setHeaderLabels(header);
    remoteFileView->header()->setClickable(true);

    layout->addWidget(localDirEdit,0,0,1,3);
    //layout->addWidget(remoteDirEdit,0,4,1,3);
    layout->addWidget(localFileView,1,0,6,3);
    layout->addWidget(remoteFileView,0,4,7,3);
    layout->addWidget(uploadBtn,2,3,1,1);
    layout->addWidget(downloadBtn,3,3,1,1);
    layout->addWidget(deleteBtn,4,3,1,1);
    layout->addWidget(transferProgressBar,10,0,1,-1);
    layout->addWidget(msg,8,0,2,7);

    QObject::connect(ftpConnection,SIGNAL(rawCommandReply(int,QString)),this,SLOT(setMessageOutput(int,QString)));
    QObject::connect(ftpConnection,SIGNAL(stateChanged(int)),this,SLOT(appendStateMessage(int)));
    QObject::connect(ftpConnection,SIGNAL(listInfo(QUrlInfo)),this,SLOT(refreshRemoteDir(QUrlInfo)));
    QObject::connect(ftpConnection,SIGNAL(commandFinished(int,bool)),this,SLOT(refreshRemoteDir(int,bool)));
    QObject::connect(ftpConnection,SIGNAL(commandFinished(int,bool)),this,SLOT(refreshLocalDir(int,bool)));
    QObject::connect(ftpConnection,SIGNAL(commandStarted(int)),this,SLOT(appendCommandMessage()));
    QObject::connect(ftpConnection,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(updateDataTransferProgress(qint64,qint64)));
    QObject::connect(localFileView,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(sortByHeader(QTreeWidgetItem,int)));
    QObject::connect(localFileView,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(setCurrentFile(QTreeWidgetItem*,int)));
    QObject::connect(localFileView,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(switchToLocalDir(QTreeWidgetItem*,int)));
    QObject::connect(remoteFileView,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(sortByHeader(QTreeWidgetItem,int)));
    QObject::connect(remoteFileView,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(setCurrentFile(QTreeWidgetItem*,int)));
    QObject::connect(remoteFileView,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(switchToRemoteDir(QTreeWidgetItem*,int)));
    QObject::connect(localDirEdit,SIGNAL(returnPressed()),this,SLOT(switchToLocalDir()));
    QObject::connect(uploadBtn,SIGNAL(clicked()),this,SLOT(uploadFile()));
    QObject::connect(downloadBtn,SIGNAL(clicked()),this,SLOT(downloadFile()));
    QObject::connect(deleteBtn,SIGNAL(clicked()),this,SLOT(deleteFile()));


    this->siteUrl = "host";
    this->showSite(siteUrl,"username","password");
    this->remote = 0;

    localDir.root();
    localDirEdit->setText(localDir.absolutePath());
    refreshLocalDir();
    this->setLayout(layout);
    this->resize(800,600);
}

FtpClient::~FtpClient()
{

}

void FtpClient::refreshLocalDir() {
    localDir.refresh();
    localFileView->clear();
    fileInfo = localDir.entryInfoList();
    for(int i=1; i < fileInfo.size() ; i++ ) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        if(fileInfo.at(i).isDir()) {
            item->setIcon(0,QIcon(":/folder.png"));
        } else {
            QString tmp;
            item->setIcon(0,QIcon(":/page_white_text.png"));
            item->setText(1,tmp.setNum((fileInfo.at(i).size())/1024,10) + "kb");
        }
        item->setText(0,fileInfo.at(i).fileName());
        item->setText(2,fileInfo.at(i).lastModified().toString("yyyy-MM-dd hh:mm:ss"));
        localFileView->addTopLevelItem(item);
    }
}

void FtpClient::refreshLocalDir(int id,bool error) {
    if(id==currentId&&!error) {
        if(accessingFile!=NULL) {
            accessingFile->close();
            delete accessingFile;
            accessingFile = NULL;
        }
        this->refreshLocalDir();
    }
}

void FtpClient::showSite(QString url, int port) {
    this->siteUrl = url;
    this->ftpConnection->connectToHost(url,port);
    this->ftpConnection->login();
    this->ftpConnection->list();
}

void FtpClient::showSite(QString url, QString username, QString password, int port) {
    this->siteUrl = url;
    this->ftpConnection->connectToHost(url,port);
    this->ftpConnection->login(username,password);
    this->ftpConnection->list();
}

void FtpClient::setMessageOutput(int code, QString msg) {
    this->msg->append(msg + "\n");
}

void FtpClient::appendStateMessage(int state) {
    switch(state) {
        case QFtp::HostLookup:
            this->msg->append("Looking up for host " + siteUrl);
            break;

        case QFtp::Connecting:
            this->msg->append("Connecting to host...");
            break;

        case QFtp::Connected:
            this->msg->append("Connected.");
            break;

        case QFtp::LoggedIn:
            this->msg->append("Logged in.");
            break;
    }
}

void FtpClient::appendCommandMessage() {
    switch(ftpConnection->currentCommand()) {
    case QFtp::List:
        this->msg->append("Listing directory");
        break;
    case QFtp::Cd:
        this->msg->append("Changing directory");
        break;
    case QFtp::Get:
        this->msg->append("Fetching file");
        break;
    case QFtp::Put:
        this->msg->append("Uploading file");
        break;
    case QFtp::Remove:
        this->msg->append("Deleting file");
        break;
    }
}

void FtpClient::refreshRemoteDir(QUrlInfo info) {
    if(this->remoteFileView->topLevelItemCount()==0) {
        QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
        tmpItem->setIcon(0,QIcon(":/folder.png"));
        tmpItem->setText(0,"..");
        remoteFileView->addTopLevelItem(tmpItem);
    }
    QTreeWidgetItem *item = new QTreeWidgetItem();
    if(info.isDir()) {
        item->setIcon(0,QIcon(":/folder.png"));
    } else {
        QString tmp;
        item->setIcon(0,QIcon(":/page_white_text.png"));
        item->setText(1,tmp.setNum((info.size())/1024,10) + "kb");
    }
    item->setText(0,info.name());
    item->setText(2,info.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
    remoteFileView->addTopLevelItem(item);
}

void FtpClient::refreshRemoteDir(int id, bool error) {
    if(id==currentId&&!error) {
        if(accessingFile!=NULL) {
            accessingFile->close();
            delete accessingFile;
            accessingFile = NULL;
        }
        enableBtns();
        this->remoteFileView->clear();
        QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
        tmpItem->setIcon(0,QIcon(":/folder.png"));
        tmpItem->setText(0,"..");
        remoteFileView->addTopLevelItem(tmpItem);
        this->ftpConnection->list();
        enableBtns();
    }
}

void FtpClient::sortByHeader(QTreeWidgetItem * item, int column) {
    if(item==this->localFileView->headerItem()) {
        this->localFileView->sortItems(column,Qt::AscendingOrder);
    } else if(item==this->remoteFileView->headerItem()) {
        this->remoteFileView->sortItems(column,Qt::AscendingOrder);
    }
}

void FtpClient::switchToRemoteDir(QTreeWidgetItem * item, int column) {
    if(item->text(1)!="") return;
    this->ftpConnection->cd(item->text(0));
    this->remoteFileView->clear();
    QTreeWidgetItem *tmpItem = new QTreeWidgetItem();
    tmpItem->setIcon(0,QIcon(":/folder.png"));
    tmpItem->setText(0,"..");
    remoteFileView->addTopLevelItem(tmpItem);
    this->ftpConnection->list();
}

void FtpClient::switchToLocalDir(QTreeWidgetItem * item, int column) {
    if(item->text(1)!="") return;
    localDir.cd(item->text(0));
    localDirEdit->setText(localDir.absolutePath());
    this->refreshLocalDir();
}

void FtpClient::switchToLocalDir() {
    QString path = localDirEdit->text();
    if(QFile::exists(path)) {
        localDir.setPath(path);
    } else {
        localDirEdit->setText(localDir.absolutePath());
    }
    this->refreshLocalDir();
}

void FtpClient::uploadFile() {
    if(this->localFileView->selectedItems().count()==0) {
        return;
    }
    accessingFile = new QFile(localDir.absolutePath() + "/" + this->localFileView->selectedItems().at(0)->text(0));
    disableBtns();
    ftpConnection->put(accessingFile,this->localFileView->selectedItems().at(0)->text(0));
    currentId = ftpConnection->currentId();
}

void FtpClient::downloadFile() {
    if(this->remoteFileView->selectedItems().count()==0) {
        return;
    }
    accessingFile = new QFile(localDir.absolutePath() + "/" + this->remoteFileView->selectedItems().at(0)->text(0));
    if(!accessingFile->open(QIODevice::WriteOnly)) {
        return;
    }
    disableBtns();
    ftpConnection->get(this->remoteFileView->selectedItems().at(0)->text(0),accessingFile);
    currentId = ftpConnection->currentId();
}

void FtpClient::disableBtns() {
    this->uploadBtn->setEnabled(false);
    this->downloadBtn->setEnabled(false);
    this->deleteBtn->setEnabled(false);
}

void FtpClient::enableBtns() {
    this->uploadBtn->setEnabled(true);
    this->downloadBtn->setEnabled(true);
    this->deleteBtn->setEnabled(true);
}

void FtpClient::setCurrentFile(QTreeWidgetItem * item, int column) {
    if(item->treeWidget()==remoteFileView) {
        this->remote = 1;
        this->currentFile = item->text(0);
    } else {
        this->remote = 2;
        this->currentFile = this->localDir.absolutePath() + "/" + item->text(0);
    }
}

void FtpClient::deleteFile() {
    if(this->remote==1) {
        this->disableBtns();
        ftpConnection->remove(this->currentFile);
        currentId = ftpConnection->currentId();
    } else if(this->remote==2) {
        this->disableBtns();
        //QFile *tmp = new QFile(this->currentFile);
        this->enableBtns();
        this->refreshLocalDir();
        this->currentFile = "";
    }
}

void FtpClient::updateDataTransferProgress(qint64 done, qint64 total) {
    this->transferProgressBar->setMaximum(total);
    this->transferProgressBar->setValue(done);
}
