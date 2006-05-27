#ifndef SIMPLESEARCHGUI_H
#define SIMPLESEARCHGUI_H

#include <QtGui/QWidget>
#include "queryexecuter.h"

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QLabel;
class QPushButton;
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
    QListWidget* itemview;
    QueryExecuter executer;
    QPushButton* toggleindexing;
    QPushButton* toggledaemon;

    void startDaemon();
private slots:
    void updateStatus();
    void toggleDaemon();
    void toggleIndexing();
    void query(const QString&);
    void handleQueryResult(const QString&);
    void openItem(QListWidgetItem*);
public:
    SimpleSearchGui();
};

#endif
