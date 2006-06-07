#include "simplesearchgui.h"
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedWidget>
#include <QtGui/QLabel>
#include <QtGui/QTextBrowser>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <QtGui/QTextDocument>
#include <QtGui/QComboBox>
#include <QtCore/QCoreApplication>
#include <string>
#include <vector>
#include "socketclient.h"
using namespace std;

SimpleSearchGui::SimpleSearchGui() {
    mainview = new QStackedWidget();
    itemview = new QTextBrowser();

    QWidget* statuswidget = new QWidget();
    QVBoxLayout *statuslayout = new QVBoxLayout;
    statusview = new QLabel();
    statusview->setAlignment(Qt::AlignTop);
    statusview->setMargin(25);
    indexing = false;
    running = false;
    starting = true;
    indexeddirs = new QListWidget();
    adddir = new QPushButton("add directory");
    removedir = new QPushButton("remove directory");
    toggleindexing = new QPushButton("start indexing");
    toggledaemon = new QPushButton("stop daemon");
    statuslayout->addWidget(statusview);
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(toggledaemon);
    hlayout->addWidget(toggleindexing);
    statuslayout->addLayout(hlayout);
    vector<string> backends = ClientInterface::getBackEnds();
    if (backends.size() > 1) {
        backendsList = new QComboBox();
        for (uint i = 0; i< backends.size(); ++i) {
            QString backend = backends[i].c_str();
            backendsList->insertItem(i, backend);
        }
        statuslayout->addWidget(backendsList);
    } else {
        backendsList = 0;
    }
    statuslayout->addWidget(indexeddirs);
    hlayout = new QHBoxLayout;
    hlayout->addWidget(adddir);
    hlayout->addWidget(removedir);
    statuslayout->addLayout(hlayout);
    statuswidget->setLayout(statuslayout);

    mainview->addWidget(itemview);
    mainview->addWidget(statuswidget);
    mainview->setCurrentIndex(1);

    queryfield = new QLineEdit();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mainview);
    layout->addWidget(queryfield);
    setLayout(layout);

    connect(queryfield, SIGNAL(textChanged(const QString&)),
        this, SLOT(query(const QString&)));
    connect(&executer, SIGNAL(queryFinished(const QString&)),
        this, SLOT(handleQueryResult(const QString&)));
    connect(itemview, SIGNAL(anchorClicked(const QUrl&)),
        this, SLOT(openItem(const QUrl&)));
    connect(toggleindexing, SIGNAL(clicked()),
        this, SLOT(toggleIndexing()));
    connect(toggledaemon, SIGNAL(clicked()),
        this, SLOT(toggleDaemon()));
    connect(adddir, SIGNAL(clicked()),
        this, SLOT(addDirectory()));
    connect(removedir, SIGNAL(clicked()),
        this, SLOT(removeDirectory()));
    itemview->setEnabled(false);
    queryfield->setFocus(Qt::ActiveWindowFocusReason);

    socketfile = getenv("HOME");
    socketfile += "/.kitten/socket";

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
    timer->start(2000);
    updateStatus();
}
void
SimpleSearchGui::query(const QString& item) {
    QString query = item.trimmed();
    if (query.length() == 0) {
        mainview->setCurrentIndex(1);
    } else {
        mainview->setCurrentIndex(0);
        itemview->setEnabled(false);
        itemview->clear();
        itemview->append("searching...");
        executer.query(query);
    }
}
void
SimpleSearchGui::updateStatus() {
    static bool first = true;
    static bool attemptedstart = false;
    if (!first && !statusview->isVisible()) return;
    first = false;
    SocketClient client;
    client.setSocketName(socketfile.c_str());
    map<string,string> s = client.getStatus();
    if (s.size() == 0) {
        running = false;
        /*if (!attemptedstart) {
            s["Status"] = "Starting daemon";
            startDaemon();
            attemptedstart = true;
        } else {*/
            s["Status"] = "Daemon is not running";
        //}
    } else {
        attemptedstart = true;
        starting = false;
        running = true;
        if (indexeddirs->count() == 0) {
            updateDirectories();
        }
    }
    toggleindexing->setEnabled(running);
    queryfield->setEnabled(running);
    queryfield->setEnabled(!starting);
    toggledaemon->setText((running)?"stop daemon":"start daemon");
    if (backendsList) {
        backendsList->setEnabled(!running);
    }
    bool idxng = s["Status"] == "indexing";
    if (idxng != indexing) {
        indexing = idxng;
        toggleindexing->setText((indexing)?"stop indexing":"start indexing");
    }
    
    map<string,string>::const_iterator i;
    QString text;
    for (i = s.begin(); i != s.end(); ++i) {
        text += i->first.c_str();
        text += ":\t";
        text += i->second.c_str();
        text += "\n";
    }
    statusview->setText(text);
}
void
SimpleSearchGui::startDaemon() {
    starting = true;
    // try to start the daemon
    QFileInfo exe = QCoreApplication::applicationDirPath()
	+ "/../../daemon/kittendaemon";
    QStringList args;
    if (backendsList) {
        args += backendsList->currentText();
    }
    if (exe.exists()) {
	// start not installed version
	QProcess::startDetached(exe.absoluteFilePath(), args);
    } else {
        exe = QCoreApplication::applicationDirPath()+"/kittendaemon";
        if (exe.exists()) {
            QProcess::startDetached(exe.absoluteFilePath(), args);
        } else {
	    // start installed version
	    QProcess::startDetached("kittendaemon");
        }
    }
}
void
SimpleSearchGui::handleQueryResult(const QString& item) {
    itemview->clear();
    ClientInterface::Hits hits = executer.getResults();
    if (hits.hits.size() > 0) {
        QString html;
        if (hits.error.length() == 0) {
            itemview->setEnabled(true);
            vector<ClientInterface::Hit>::const_iterator i;
            for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
                QString path = i->uri.c_str();
                QString name;
                map<string, string>::const_iterator t
                    = i->properties.find("title");
                if (t != i->properties.end()) {
                    name = t->second.c_str();
                }
                int l = path.lastIndexOf('/');
                html += "<div><a href='"+path+"'>";
                if (l != -1) {
                    if (name.length()) {
                        html += name;
                    } else {
                        html += path.mid(l+1);
                    }
                    path = path.left(l);
                    html += "</a> from folder <a href='"+path+"'>" + path;
                } else {
                    if (name.length()) {
                        html += name;
                    } else {
                        html += path;
                    }
                }
                html += "</a><br/>score: ";
                html += QString::number(i->score) + "<br/><i>";
                html += QString(i->fragment.c_str()).left(100).replace("<", "&lt;");
                html += "</i></div>";
            }
        } else {
            html = "<h2>";html+=hits.error.c_str();html+="</h2>";
        }
        itemview->setHtml(html);
    } else {
        itemview->append("no results");
    }
}
void
SimpleSearchGui::openItem(const QUrl& url) {
    // if the file does not exist on the file system remove items of the end
    // until it does
    QString file = url.toString();
    itemview->setSource(itemview->source());
    QFileInfo info(file);
    while (!info.exists()) {
        int pos = file.lastIndexOf('/');
        if (pos <= 0) return;
        file = file.left(pos);
        info.setFile(file);
    }
    if (file.endsWith(".tar") || file.endsWith(".tar.bz2")
            || file.endsWith(".tar.gz")) {
        file = "tar:"+url.toString();
    } else if (file.endsWith(".zip") || file.endsWith(".jar")) {
        file = "zip:"+url.toString();
    }
    QStringList args;
    args << "openURL" << file;
    QProcess::execute("kfmclient", args);
}
void
SimpleSearchGui::toggleDaemon() {
    if (running) {
        SocketClient client;
        client.setSocketName(socketfile.c_str());
        client.stopDaemon();
        indexeddirs->clear();
    } else {
        startDaemon();
    }
}
void
SimpleSearchGui::toggleIndexing() {
    SocketClient client;
    client.setSocketName(socketfile.c_str());
    if (indexing) {
        client.stopIndexing();
    } else {
        client.startIndexing();
    }
}
void
SimpleSearchGui::addDirectory() {
    // open file dialog
    QString dir = QFileDialog::getExistingDirectory (this);
    if (dir.size() <= 0) return;
    for (int i=0; i<indexeddirs->count(); ++i) {
        QString text = indexeddirs->item(i)->text();
        if (dir.startsWith(text)) {
            return;
        }
        if (text.startsWith(dir)) {
            indexeddirs->takeItem(i);
            i = 0;
        }
    }
    indexeddirs->addItem(dir);
    setDirectories();
}
void
SimpleSearchGui::removeDirectory() {
    int i = indexeddirs->currentRow();
    if (i == -1) return;
    indexeddirs->takeItem(i);
    setDirectories();
}
void
SimpleSearchGui::setDirectories() {
    vector<string> s;
    for (int i=0; i<indexeddirs->count(); ++i) {
        QString text = indexeddirs->item(i)->text();
        s.push_back((const char*)text.toUtf8());
    }
    SocketClient client;
    client.setSocketName(socketfile.c_str());
    client.setIndexedDirectories(s);
    updateDirectories();
}
void
SimpleSearchGui::updateDirectories() {
    indexeddirs->clear();
    SocketClient client;
    client.setSocketName(socketfile.c_str());
    vector<string> s = client.getIndexedDirectories();
    indexeddirs->clear();
    for (uint i=0; i<s.size(); ++i) {
        QString dir(s[i].c_str());
        indexeddirs->addItem(dir);
    }
}
