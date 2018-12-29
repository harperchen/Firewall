#ifndef QLINKTABLE_H
#define QLINKTABLE_H

#include <QAbstractTableModel>
#include "rule.h"
#include "utils.h"

class QLinkTable : public QAbstractTableModel
{
    Q_OBJECT

    enum Roles{
        Source = Qt::UserRole + 1,
        Dest,
        Protocol,
        Timeout,
        Info
    };

public:
    explicit QLinkTable(QObject *parent = 0);

    int rowCount   (const QModelIndex& parent = QModelIndex())        const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex())        const Q_DECL_OVERRIDE;
    QVariant data  (const QModelIndex &index, int role)               const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames()                                const Q_DECL_OVERRIDE;

    Q_INVOKABLE void sort(int column, Qt::SortOrder order);
    Q_INVOKABLE void filter(QString str);

    QList<Link> links;

public slots:
    void setlinkItem(QString str);
};

#endif // QLINKTABLE_H
