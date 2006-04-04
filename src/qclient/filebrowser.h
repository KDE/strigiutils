#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QSplitter>

class QDirModel;
class QTreeView;
class QModelIndex;
class QTextBrowser;

class FileBrowser : public QSplitter {
Q_OBJECT
private:
    QTreeView *view;
    QDirModel *model;
    QTextBrowser *browser;
public:
    FileBrowser();
    ~FileBrowser();
public slots:
    void clicked(const QModelIndex& index);
};

#endif
