#ifndef SIMPLESEARCHGUI_H
#define SIMPLESEARCHGUI_H

#include <QtGui/QWidget>
#include "queryexecuter.h"

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class SimpleSearchGui : public QWidget {
Q_OBJECT
private:
    QLineEdit* queryfield;
    QListWidget* itemview;
    QueryExecuter executer;

private slots:
    void query(const QString&);
    void handleQueryResult(const QString&);
    void addItem(const QString&);
    void openItem(QListWidgetItem*);
public:
    SimpleSearchGui();
};

#endif
