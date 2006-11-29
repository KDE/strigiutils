#ifndef DLGFILTERS_H
#define DLGFILTERS_H
 
#include <QDialog>

class FilterWidget;
class DlgFilters : public QDialog {
Q_OBJECT
private:
    FilterWidget* filterwidget;
public:
    DlgFilters(const QList<QPair<bool,QString> >& rules, QWidget *parent = 0);
    const QList<QPair<bool,QString> >& getFilters() const;
};
 
#endif
