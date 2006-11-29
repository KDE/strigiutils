#include <QWidget>

class QListView;
class FilterModel;
class QModelIndex;
class QPushButton;
class FilterWidget : public QWidget {
Q_OBJECT
private:
    QListView* listview;
    FilterModel* model;
    QPushButton* up;
    QPushButton* down;
    QPushButton* del;
    QPushButton* add;
    QPushButton* incl;
public:
    FilterWidget();
    void setFilters(const std::vector<std::pair<bool,std::string> >& f);
    std::vector<std::pair<bool,std::string> > getFilters() const;
};
