#include "filterwidget.h"
#include "filtermodel.h"
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAbstractListModel>
#include <QDebug>

FilterWidget::FilterWidget() {
    listview = new QListView();
    listview->setViewMode(QListView::ListMode);
    model = new FilterModel(this);
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
        model, SLOT(clicked(const QModelIndex&)));
}
void
FilterWidget::setFilters(const std::vector<std::pair<bool,std::string> >& f) {
    model->filters = f;
}
std::vector<std::pair<bool,std::string> >
FilterWidget::getFilters() const {
    return model->filters;
}
