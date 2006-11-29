#include "filtermodel.h"
#include <QDebug>

void
FilterModel::slotUp() {
}
void
FilterModel::slotDown() {
}
void
FilterModel::slotDelete() {
}
void
FilterModel::slotAdd() {
}
void
FilterModel::slotNew() {
}
void
FilterModel::clicked(const QModelIndex& i) {
    qDebug()<<i;
    qDebug()<<i.flags();
}

