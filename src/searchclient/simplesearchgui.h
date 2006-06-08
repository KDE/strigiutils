#ifndef SIMPLESEARCHGUI_H
#define SIMPLESEARCHGUI_H

#include <QtGui/QWidget>

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QLabel;
class QPushButton;
class QComboBox;
class SearchTabs;

class SimpleSearchGui : public QWidget {
Q_OBJECT
private:
    bool starting;
    bool indexing;
    bool running;
    std::string socketfile;
    QStackedWidget* mainview;
    SearchTabs* tabs;
    QLabel* statusview;
    QLineEdit* queryfield;
    QListWidget* indexeddirs;
    QPushButton* adddir;
    QPushButton* removedir;
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
    void addDirectory();
    void removeDirectory();
public:
    SimpleSearchGui();
};

#endif
