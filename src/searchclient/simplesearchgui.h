#ifndef SIMPLESEARCHGUI_H
#define SIMPLESEARCHGUI_H

#include <QtGui/QWidget>
#include "queryexecuter.h"

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QLabel;
class SimpleSearchGui : public QWidget {
Q_OBJECT
private:
    QStackedWidget* mainview;
    QLabel* statusview;
    QLineEdit* queryfield;
    QListWidget* itemview;
    QueryExecuter executer;

private slots:
    void updateStatus();
    void query(const QString&);
    void handleQueryResult(const QString&);
    void openItem(QListWidgetItem*);
public:
    SimpleSearchGui();
};

#endif
