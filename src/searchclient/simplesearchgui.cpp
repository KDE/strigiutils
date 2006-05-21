#include "simplesearchgui.h"
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
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
    connect(itemview, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(openItem(QListWidgetItem*)));
}
void
SimpleSearchGui::query(const QString& item) {
    itemview->clear();

    SocketClient client;
    std::string socket = getenv("HOME");
    socket += "/.kitten/socket";
    client.setSocketName(socket.c_str());
    vector<string> results = client.query((const char*)item.toUtf8());

    if (results.size() > 0 && results[0] == "error") {
        for (int i=0; i<results.size(); ++i) {
            qDebug() << results[i].c_str();
        }
    } else {
        for (int i=0; i<results.size(); ++i) {
            itemview->addItem(results[i].c_str());
        }
    }
}
void
SimpleSearchGui::addItem(const QString& item) {
    itemview->addItem(item);
}
void
SimpleSearchGui::openItem(QListWidgetItem* i) {
    QStringList args;
    args << "openURL" << i->text();
    QProcess::execute("kfmclient", args);
    qDebug() << i->text();
}
