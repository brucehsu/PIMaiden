#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QLineEdit>
#include <QDir>
#include <QFileInfoList>
#include <QDateTime>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QFtp>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QProgressBar>
#include <QMessageBox>

class FtpClient : public QWidget
{
    Q_OBJECT

public:
    FtpClient(QWidget *parent = 0);
    void showSite(QString url, int port=21);
    void showSite(QString url, QString username, QString password, int port=21);
    ~FtpClient();

private:
    QTreeWidget *localFileView, *remoteFileView;
    QDir localDir;
    QFileInfoList fileInfo;
    QLineEdit *localDirEdit, *remoteDirEdit;
    QPushButton *uploadBtn,*downloadBtn,*deleteBtn;
    QGridLayout *layout;
    QTextEdit *msg;
    QFtp *ftpConnection;
    QString siteUrl;
    QProgressBar *transferProgressBar;
    int remote;
    QString currentFile;
    QFile *accessingFile;
    int currentId;
    void disableBtns();
    void enableBtns();

public slots:
    void refreshLocalDir();
    void refreshLocalDir(int, bool);
    void setMessageOutput(int,QString);
    void appendStateMessage(int);
    void appendCommandMessage();
    void refreshRemoteDir(int, bool);
    void refreshRemoteDir(QUrlInfo);
    void sortByHeader(QTreeWidgetItem*, int);
    void switchToRemoteDir(QTreeWidgetItem*, int);
    void switchToLocalDir(QTreeWidgetItem*, int);
    void uploadFile();
    void downloadFile();
    void setCurrentFile(QTreeWidgetItem*, int);
    void deleteFile();
    void updateDataTransferProgress (qint64 done, qint64 total);
};

#endif // FTPCLIENT_H
