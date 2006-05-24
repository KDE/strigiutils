#include "simplesearchgui.h"
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <string>
#include <vector>
#include "socketclient.h"
using namespace std;

SimpleSearchGui::SimpleSearchGui() {
    itemview = new QListWidget();
    queryfield = new QLineEdit();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(itemview);
    layout->addWidget(queryfield);
    setLayout(layout);

    connect(queryfield, SIGNAL(textChanged(const QString&)),
        this, SLOT(query(const QString&)));
    connect(&executer, SIGNAL(queryFinished(const QString&)),
        this, SLOT(handleQueryResult(const QString&)));
    connect(itemview, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(openItem(QListWidgetItem*)));
    itemview->setEnabled(false);
    queryfield->setFocus(Qt::ActiveWindowFocusReason);
}
void
SimpleSearchGui::query(const QString& item) {
    itemview->setEnabled(false);
    itemview->clear();
    itemview->addItem("searching...");
    executer.query(item.trimmed());
}
void
SimpleSearchGui::handleQueryResult(const QString& item) {
    itemview->clear();
    vector<string> results = executer.getResults();

    if (results.size() > 0) {
        if (results[0] != "error") {
            itemview->setEnabled(true);
        }
        for (uint i=0; i<results.size(); ++i) {
            itemview->addItem(results[i].c_str());
        }
    } else {
        itemview->addItem("no results");
    }
}
void
SimpleSearchGui::openItem(QListWidgetItem* i) {
    // if the file does not exist on the file system remove items of the end
    // until it does
    QString file = i->text();
    QFileInfo info(file);
    while (!info.exists()) {
        int pos = file.lastIndexOf('/');
        if (pos <= 0) return;
        file = file.left(pos);
        info.setFile(file);
    }
    if (file.endsWith(".tar") || file.endsWith(".tar.bz2")
            || file.endsWith(".tar.gz")) {
        file = "tar:"+i->text();
    } else if (file.endsWith(".zip") || file.endsWith(".jar")) {
        file = "zip:"+i->text();
    }
    QStringList args;
    args << "openURL" << file;
    QProcess::execute("kfmclient", args);
    qDebug() << i->text();
}
