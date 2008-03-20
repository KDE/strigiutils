#ifndef XESAMTYPES
#define XESAMTYPES

#include <QtCore/QMetaType>

typedef QVector<QList<QVariant> > VariantListVector;
Q_DECLARE_METATYPE(VariantListVector)
typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)

#endif
