#ifndef SIMPLESEARCHGUI_H
#define SIMPLESEARCHGUI_H

#include <QtGui/QWidget>
#include "queryexecuter.h"

class QLineEdit;
class QListWidget;
class QTextBrowser;
class QTextDocument;
class QListWidgetItem;
class QStackedWidget;
class QLabel;
class QUrl;
class QPushButton;
class QComboBox;
class SimpleSearchGui : public QWidget {
Q_OBJECT
private:
    bool starting;
    bool indexing;
    bool running;
    std::string socketfile;
    QStackedWidget* mainview;
    QLabel* statusview;
    QLineEdit* queryfield;
    QTextBrowser* itemview;
    QListWidget* indexeddirs;
    QPushButton* adddir;
    QPushButton* removedir;
    QueryExecuter executer;
    QPushButton* toggleindexing;
    QPushButton* toggledaemon;
    QComboBox* backendsList;

    void startDaemon();
    void setDirectories();
    void updateDirectories();
private slots:
    void updateStatus();
    void toggleDaemon();
    void toggleIndexing();
    void query(const QString&);
    void handleQueryResult(const QString&);
    void openItem(const QUrl&);
    void addDirectory();
    void removeDirectory();
public:
    SimpleSearchGui();
};

#endif
