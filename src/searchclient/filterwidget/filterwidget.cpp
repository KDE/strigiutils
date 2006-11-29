#include "filterwidget.h"
#include "filtermodel.h"
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAbstractListModel>
#include <QDebug>

FilterWidget::FilterWidget(QWidget* p) :QWidget(p) {

    listview = new QListView();
    listview->setViewMode(QListView::ListMode);
    model = new FilterModel(this);
    listview->setModel(model);
    incl = new QPushButton(tr("include"));
    up = new QPushButton(tr("move up"));
    down = new QPushButton(tr("move down"));
    del = new QPushButton(tr("remove filter"));
    add = new QPushButton(tr("add filter"));
    incl->setCheckable(true);

    QHBoxLayout *layout = new QHBoxLayout();
    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(incl);
    vl->addWidget(up);
    vl->addWidget(down);
    vl->addWidget(add);
    vl->addWidget(del);
    vl->addStretch();

    layout->addLayout(vl);
    layout->addWidget(listview);

    setLayout(layout);

    connect(listview->selectionModel(),
        SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
        this, SLOT(rowChanged(const QModelIndex&,const QModelIndex&)));

    connect(add,  SIGNAL(clicked(bool)), this, SLOT(addFilter()));
    connect(del,  SIGNAL(clicked(bool)), this, SLOT(delFilter()));
    connect(incl, SIGNAL(toggled(bool)), this, SLOT(include(bool)));
    connect(up,   SIGNAL(clicked(bool)), this, SLOT(moveUp()));
    connect(down, SIGNAL(clicked(bool)), this, SLOT(moveDown()));

    updateButtons();
}
void
FilterWidget::setFilters(const QList<QPair<bool,QString> >& f) {
    model->filters = f;
    qDebug() << f.size();
}
const QList<QPair<bool,QString> >&
FilterWidget::getFilters() const {
    return model->filters;
}
void
FilterWidget::rowChanged(const QModelIndex& current, const QModelIndex& prev) {
    updateButtons();
}
void
FilterWidget::updateButtons() {
    QModelIndex i = listview->currentIndex();
    bool valid = i.isValid();
    int r = i.row();
    
    up->setEnabled(r > 0);
    incl->setEnabled(valid);
    if (valid) {
        bool c = model->filters[r].first;
        incl->setChecked(c);
        incl->setText((c) ?tr("include") :tr("exclude"));
    }
    down->setEnabled(valid && r < (int)model->filters.size()-1);
    del->setEnabled(valid && model->filters.size());
    if (!valid || model->filters.size() == 1) {
        listview->setCurrentIndex(i.sibling(0,0));
    }
    listview->setFocus(Qt::OtherFocusReason);
}
void
FilterWidget::include(bool state) {
    model->include(listview->currentIndex(), state);
    updateButtons();
}
void
FilterWidget::addFilter() {
    QModelIndex i = listview->currentIndex();
    model->addFilter(listview->currentIndex(), incl->isChecked());
    listview->setCurrentIndex(i.sibling(i.row(), 0));
    updateButtons();
}
void
FilterWidget::delFilter() {
    model->delFilter(listview->currentIndex());
    updateButtons();
}
void
FilterWidget::moveUp() {
    QModelIndex i = listview->currentIndex();
    model->moveUp(i);
    listview->setCurrentIndex(i.sibling(i.row()-1, 0));
    updateButtons();
}
void
FilterWidget::moveDown() {
    QModelIndex i = listview->currentIndex();
    model->moveDown(listview->currentIndex());
    listview->setCurrentIndex(i.sibling(i.row()+1, 0));
    updateButtons();
}
