#include "qlinktable.h"

#define COLUMN_COUNT 5

QLinkTable::QLinkTable(QObject *parent)
{
    links.clear();
}

int QLinkTable::rowCount(const QModelIndex &parent) const
{
    int i = 0;
    foreach (Link link, links) {
        if(link.isVisible){
            i++;
        }
    }
    return i;
}

int QLinkTable::columnCount(const QModelIndex &parent) const
{
    return COLUMN_COUNT;
}

QVariant QLinkTable::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(!links[index.row()].isVisible){
        return QVariant();
    }
    switch (role) {
        case Source:
            return links[index.row()].source_ip;
        case Dest:
            return links[index.row()].dest_ip;
        case Protocol:
            return links[index.row()].protocol;
        case Timeout:
            return links[index.row()].timeout;
        case Info:{
            if(links[index.row()].protocol == "ICMP"){
                return links[index.row()].info;
            }
            else{
                return "Source port : " + links[index.row()].source_port
                      + " Dest port : " + links[index.row()].dest_port
                      + links[index.row()].info;
            }
        }
    }
    return QVariant();
}

QHash<int, QByteArray> QLinkTable::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Source] = "Source";
    roles[Dest] = "Dest";
    roles[Protocol] = "Protocol";
    roles[Timeout] = "Timeout";
    roles[Info] = "Info";
    return roles;
}

void QLinkTable::sort(int column, Qt::SortOrder order)
{
    switch (column) {
        case 0:{
            if(order == Qt::AscendingOrder){
                qSort(links.begin(), links.end(), AscendingOrder_sip);
            }
            else if(order == Qt::DescendingOrder){
                qSort(links.begin(), links.end(), DescendingOrder_sip);
            }
            break;
        }
        case 1:{
            if(order == Qt::AscendingOrder){
                qSort(links.begin(), links.end(), AscendingOrder_dip);
            }
            else if(order == Qt::DescendingOrder){
                qSort(links.begin(), links.end(), DescendingOrder_dip);
            }
            break;

        }
        case 2:{
            if(order == Qt::AscendingOrder){
                qSort(links.begin(), links.end(), AscendingOrder_prot);
            }
            else if(order == Qt::DescendingOrder){
                qSort(links.begin(), links.end(), DescendingOrder_prot);
            }
            break;
        }
        case 3:{
            if(order == Qt::AscendingOrder){
                qSort(links.begin(), links.end(), AscendingOrder_time);
            }
            else if(order == Qt::DescendingOrder){
                qSort(links.begin(), links.end(), DescendingOrder_time);
            }
            break;
        }
        default:
            break;
    }
    endResetModel();
}

void QLinkTable::filter(QString str)
{
    qDebug() << "filter";
    for(int i = 0; i < links.size(); i++){
        Link link = links.at(i);
        QString link_str = link.to_string();
        qDebug() << link_str;
        if(link_str.contains(str)){
            link.isVisible = true;
        }
        else{
            link.isVisible = false;
        }
        links.removeAt(i);
        links.insert(i, link);
    }
    endResetModel();
}


void QLinkTable::setlinkItem(QString str)
{
    links.clear();
    if(str.size() == 0){
        endResetModel();
        return;
    }
    QStringList lists = str.split('\n');
    for(int i = 0; i < lists.size() - 1; i++){
        QString list = lists.at(i);
        Link link(list);
        if(!links.contains(link)){
            links.push_back(link);
        }
    }
    endResetModel();
}
