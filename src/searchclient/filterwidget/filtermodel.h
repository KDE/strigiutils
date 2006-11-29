#include <QAbstractListModel>
#include <QPair>

class FilterModel : public QAbstractListModel {
public:
    FilterModel(QObject* parent) : QAbstractListModel(parent) {}
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
        if (index.row() < 0 || index.row() >= (int)filters.size()) {
            return QVariant();
        }

        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return filters[index.row()].second;
        } else if (role == Qt::DecorationRole) {
            return (filters[index.row()].first)?Qt::green:Qt::red;
        }

        return QVariant();
    }
    QList<QPair<bool,QString> > filters;
    void include(const QModelIndex &index, bool state);
    void addFilter(const QModelIndex& index, bool state);
    void delFilter(const QModelIndex& index);
    void moveUp(const QModelIndex&);
    void moveDown(const QModelIndex&);

    bool setData(const QModelIndex& index, const QVariant& value,
        int role=Qt::EditRole);
};
