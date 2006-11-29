#include <QWidget>

class QListView;
class FilterModel;
class QModelIndex;
class QItemSelection;
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

    void updateButtons();
private slots:
    void rowChanged(const QModelIndex&,const QModelIndex&);
    void include(bool state);
    void addFilter();
    void delFilter();
    void moveUp();
    void moveDown();
public:
    FilterWidget(QWidget* parent=0);
    void setFilters(const QList<QPair<bool,QString> >& f);
    const QList<QPair<bool,QString> >& getFilters() const;
};
