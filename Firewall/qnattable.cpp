#include "qnattable.h"

#define COLUMN_COUNT 5

QNatTable::QNatTable(QObject *parent)
{
    nats.clear();
}

int QNatTable::rowCount(const QModelIndex &parent) const
{
    return nats.size();
}

int QNatTable::columnCount(const QModelIndex &parent) const
{
    return COLUMN_COUNT;
}

QVariant QNatTable::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }
    switch (role) {
        case Before_IP:{
            if(nats[index.row()].before_mask == ""){
                return nats[index.row()].before_ip;
            }
            return nats[index.row()].before_ip + "/" + nats[index.row()].before_mask;
        }
        case Before_Port:{
            if(nats[index.row()].before_port == ""){
                return "N/A";
            }
            return nats[index.row()].before_port;
        }
        case After_IP:{
            if(nats[index.row()].after_mask == ""){
                return nats[index.row()].after_ip;
            }
            return nats[index.row()].after_ip + "/" + nats[index.row()].after_mask;
        }
        case After_Port:{
            if(nats[index.row()].after_port == ""){
                return "N/A";
            }
            return nats[index.row()].after_port;
        }
        case Type:
            return nats[index.row()].nat_type;

    }
    return QVariant();
}

QHash<int, QByteArray> QNatTable::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Before_IP] = "Before_IP";
    roles[Before_Port] = "Before_Port";
    roles[After_IP] = "After_IP";
    roles[After_Port] = "After_Port";
    roles[Type] = "Type";
    return roles;
}

void QNatTable::addNatItem(QNat *qnat)
{
    Nat *nat = new Nat(qnat->before_ip, qnat->before_mask, qnat->before_port,
                       qnat->after_ip, qnat->after_mask, qnat->after_port,
                       qnat->nat_type);
    if(!nats.contains(*nat)){
        nats.push_back(*nat);
        emit addNat(nat->to_string());
    }
    endResetModel();
    delete nat;

}

void QNatTable::delNatItem()
{
    if((selected_row < 0) && (selected_row >= nats.size())){
        return;
    }
    nats.removeAt(selected_row);
    emit delNat(selected_row);
    endResetModel();
}

void QNatTable::saveNat(QString filename)
{
    if(filename.contains("file://")){
        int index = filename.indexOf("file://");
        filename = filename.mid(index + strlen("file://"));
    }
    if(!filename.contains('.')){
        filename += ".cfg";
    }
    QFile cfgFile(filename);
    if(!cfgFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&cfgFile);
    foreach (Nat nat, nats) {
        out << nat.to_string() << "\n";
    }
    cfgFile.close();
    qDebug() << "save nat file successfully";

}

void QNatTable::openNat(QString filename)
{
    if(filename.contains("file://")){
        int index = filename.indexOf("file://");
        filename = filename.mid(index + strlen("file://"));
    }
    if(!filename.contains('.')){
        filename += ".cfg";
    }
    QFile cfgFile(filename);
    if(!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&cfgFile);
    QString line = in.readLine();
    while(!line.isNull()){
        Nat *nat = new Nat(line);
        if(!nats.contains(*nat)){
            nats.push_back(*nat);
            emit addNat(line);
        }
        line = in.readLine();
        delete nat;
    }
    endResetModel();
    cfgFile.close();
    qDebug() << "open nat file successfully";
}

void QNatTable::set_selected_row(int row)
{
    selected_row = row;
}
