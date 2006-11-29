#include <QWidget>

class QListView;
class Model;
class QModelIndex;
class QPushButton;
class FilterWidget : public QWidget {
Q_OBJECT
private:
    QListView* listview;
    Model* model;
    QPushButton* up;
    QPushButton* down;
    QPushButton* del;
    QPushButton* add;
    QPushButton* incl;
public:
    FilterWidget();
    void setFilters(const std::vector<std::pair<bool,std::string> >& f);
    std::vector<std::pair<bool,std::string> > getFilters() const;
private slots:
    void clicked(const QModelIndex& index);
};
