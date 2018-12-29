#include "qfiltertable.h"

#define COLUMN_COUNT 8

QFilterTable::QFilterTable(QObject *parent)
{
    rules.clear();
}

int QFilterTable::rowCount(const QModelIndex &parent) const
{
    return rules.size();
}

int QFilterTable::columnCount(const QModelIndex &parent) const
{
    return COLUMN_COUNT;
}

QVariant QFilterTable::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    switch(role)
    {
        case Source_IP:{
            if(rules[index.row()].source_mask == ""){
                return rules[index.row()].source_ip;
            }
            return rules[index.row()].source_ip + "/" + rules[index.row()].source_mask;
        }
        case Source_Port:{
            if(rules[index.row()].source_port == ""){
                return "N/A";
            }
            return rules[index.row()].source_port;
        }
        case Dest_IP:{
            if(rules[index.row()].dest_mask == ""){
                return rules[index.row()].dest_ip;
            }
            return rules[index.row()].dest_ip + "/" + rules[index.row()].dest_mask;
        }
        case Dest_Port:{
            if(rules[index.row()].dest_port == ""){
                return "N/A";
            }
            return rules[index.row()].dest_port;
        }
        case Protocol:
            return rules[index.row()].protocol;
        case Log:
            return rules[index.row()].log;
        case Action:
            return rules[index.row()].action;
        case Chain:
            return rules[index.row()].chain;
    }
    return QVariant();
}

QHash<int, QByteArray> QFilterTable::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Source_IP] = "Source_IP";
    roles[Source_Port] = "Source_Port";
    roles[Dest_IP] = "Dest_IP";
    roles[Dest_Port] = "Dest_Port";
    roles[Protocol] = "Protocol";
    roles[Chain] = "Chain";
    roles[Log] = "Log";
    roles[Action] = "Action";
    return roles;
}

void QFilterTable::addFilterItem(QRule *qrule)
{
    Rule *rule = new Rule(qrule->source_ip, qrule->source_mask, qrule->source_port,
                          qrule->dest_ip, qrule->dest_mask, qrule->dest_port,
                          qrule->protocol, qrule->chain, qrule->action, qrule->log);
    if(!rules.contains(*rule)){
        rules.push_back(*rule);
        emit addFilter(rule->to_string());
    }
    endResetModel();
    delete rule;
}

void QFilterTable::delFilterItem()
{
    if((selected_row < 0) || (selected_row >= rules.size())){
        return;
    }
    rules.removeAt(selected_row);
    emit delFilter(selected_row);
    endResetModel();
}

void QFilterTable::saveFilter(QString filename)
{
    if(filename.contains("file://")){
        int index = filename.indexOf("file://");
        filename = filename.mid(index + strlen("file://"));
    }
    if(!filename.contains('.')){
        filename += ".cfg";
    }
    qDebug() << filename;
    QFile cfgFile(filename);
    if(!cfgFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&cfgFile);
    foreach (Rule rule, rules) {
        out << rule.to_string() << "\n";
        qDebug() << rule.to_string();
    }
    cfgFile.close();
    qDebug() << "save filter file successfully";
}

void QFilterTable::openFilter(QString filename)
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
        Rule *rule = new Rule(line);
        if(!rules.contains(*rule)){
            rules.push_back(*rule);
            emit addFilter(line);
        }
        line = in.readLine();
        delete rule;
    }
    endResetModel();
    cfgFile.close();
    qDebug() << "open filter file successfully";
}

void QFilterTable::set_selected_row(int row)
{
    selected_row = row;
    qDebug() << row;
}





