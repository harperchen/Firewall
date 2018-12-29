#ifndef QFILTERTABLE_H
#define QFILTERTABLE_H

#include <QFile>
#include <QAbstractTableModel>

#include "rule.h"
#include "qrule.h"

class QFilterTable : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int selected_row WRITE set_selected_row)

    enum Roles{
        Source_IP = Qt::UserRole + 1,
        Source_Port,
        Dest_IP,
        Dest_Port,
        Protocol,
        Chain,
        Log,
        Action
    };

public:
    explicit QFilterTable(QObject *parent = 0);

    int rowCount   (const QModelIndex& parent = QModelIndex())        const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex())        const Q_DECL_OVERRIDE;
    QVariant data  (const QModelIndex &index, int role)               const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames()                                const Q_DECL_OVERRIDE;

    Q_INVOKABLE void addFilterItem(QRule *qrule);
    Q_INVOKABLE void delFilterItem();
    Q_INVOKABLE void saveFilter(QString filename);
    Q_INVOKABLE void openFilter(QString filename);

    void set_selected_row(int row);

    QList<Rule> rules;
    int selected_row;
signals:
    void addFilter(QString);
    void delFilter(int);
};

#endif // QFILTERTABLE_H
