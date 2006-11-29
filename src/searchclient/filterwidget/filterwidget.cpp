#include "filterwidget.h"
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAbstractListModel>
#include <QDebug>

class Model : public QAbstractListModel {
public:
    Model(QWidget* parent) : QAbstractListModel(parent) {}
    int rowCount(const QModelIndex &parent = QModelIndex()) const {
        return (parent.isValid())?0:filters.size();
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const {
        return (parent.isValid())?0:1;
    }
    Qt::ItemFlags flags( const QModelIndex& index) const {
        return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
        if (index.row() < 0 || index.row() >= filters.size()) {
            return QVariant();
        }

        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return filters[index.row()].second.c_str();
        } else if (role == Qt::DecorationRole) {
            return (filters[index.row()].first)?Qt::green:Qt::red;
        }

        return QVariant();
    }
    std::vector<std::pair<bool,std::string> > filters;
};

FilterWidget::FilterWidget() {
    listview = new QListView();
    listview->setViewMode(QListView::ListMode);
    model = new Model(this);
    listview->setModel(model);
    up = new QPushButton("up");
    down = new QPushButton("down");
    del = new QPushButton("remove");
    add = new QPushButton("add");
    incl = new QPushButton("plus");
    incl->setCheckable(true);


    QHBoxLayout *layout = new QHBoxLayout();
    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(incl);
    vl->addWidget(up);
    vl->addWidget(down);
    vl->addWidget(add);
    vl->addWidget(del);

    layout->addLayout(vl);
    layout->addWidget(listview);

    setLayout(layout);

    connect(listview, SIGNAL(clicked(const QModelIndex&)),
        this, SLOT(clicked(const QModelIndex&)));
}
void
FilterWidget::setFilters(const std::vector<std::pair<bool,std::string> >& f) {
    model->filters = f;
}
std::vector<std::pair<bool,std::string> >
FilterWidget::getFilters() const {
    return model->filters;
}
void
FilterWidget::clicked(const QModelIndex& i) {
    qDebug()<<i;
    qDebug()<<i.flags();
}
