#ifndef QNATTABLE_H
#define QNATTABLE_H

#include <QFile>
#include <QAbstractTableModel>

#include "rule.h"
#include "qnat.h"

class QNatTable : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int selected_row WRITE set_selected_row)

    enum Roles{
        Before_IP = Qt::UserRole + 1,
        Before_Port,
        After_IP,
        After_Port,
        Type
    };

public:
    explicit QNatTable(QObject *parent = 0);

    int rowCount   (const QModelIndex& parent = QModelIndex())        const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex())        const Q_DECL_OVERRIDE;
    QVariant data  (const QModelIndex &index, int role)               const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames()                                const Q_DECL_OVERRIDE;

    Q_INVOKABLE void addNatItem(QNat *qnat);
    Q_INVOKABLE void delNatItem();
    Q_INVOKABLE void saveNat(QString filename);
    Q_INVOKABLE void openNat(QString filename);

    void set_selected_row(int row);

    QList<Nat> nats;
    int selected_row;

signals:
    void addNat(QString);
    void delNat(int);
};

#endif // QNATTABLE_H
