#include <QAbstractListModel>
#include <vector>

class FilterModel : public QAbstractListModel {
Q_OBJECT
private slots:
    void clicked(const QModelIndex& index);
    void slotUp();
    void slotDown();
    void slotDelete();
    void slotAdd();
    void slotNew();
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
