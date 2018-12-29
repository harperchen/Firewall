#ifndef RULE_H
#define RULE_H

#include <QString>
#include <QDebug>
#include <QJsonObject>

struct Rule{

    Rule(){

    }

    Rule(QString source_ip, QString source_mask, QString source_port,
         QString dest_ip, QString dest_mask, QString dest_port,
         QString protocol, QString chain, QString action, QString log){
        this->source_ip = source_ip;
        this->source_mask = source_mask;
        this->source_port = source_port;

        this->dest_ip = dest_ip;
        this->dest_mask = dest_mask;
        this->dest_port = dest_port;

        this->protocol = protocol;
        this->chain = chain;
        this->action = action;
        this->log = log;
    }

    bool operator ==(const Rule& rule){
        return (source_ip == rule.source_ip) && (source_mask == rule.source_mask) && (source_port == rule.source_port)
            && (dest_ip == rule.dest_ip) && (dest_mask == rule.dest_mask) && (dest_port == rule.dest_port)
            && (protocol == rule.protocol) && (chain == rule.chain)
            && (action == rule.action) && (log == rule.log);
    }

    Rule(QString &str){
        QStringList lists = str.split("#");

        source_port = lists.at(0);
        source_ip = source_port.split('/').at(0);
        source_port = source_port.split('/').at(1);
        source_mask = source_port.split(':').at(0);
        source_port = source_port.split(':').at(1);

        dest_port = lists.at(1);
        dest_ip = dest_port.split('/').at(0);
        dest_port = dest_port.split('/').at(1);
        dest_mask = dest_port.split(':').at(0);
        dest_port = dest_port.split(':').at(1);

        protocol = lists.at(2);
        chain = lists.at(3);
        action = lists.at(4);
        log = lists.at(5);
    }

    QString to_string(){
        return source_ip + "/" + source_mask + ":" + source_port + "#"
             + dest_ip + "/" + dest_mask + ":" + dest_port + "#"
             + protocol + "#" + chain + "#" + action + "#" + log;
    }

    QString source_ip;
    QString source_mask;
    QString source_port;

    QString dest_ip;
    QString dest_mask;
    QString dest_port;

    QString protocol;
    QString chain;
    QString action;
    QString log;

};

struct Nat{
    Nat(){

    }

    Nat(QString before_ip, QString before_mask, QString before_port,
        QString after_ip, QString after_mask, QString after_port,
        QString nat_type){
        this->before_ip = before_ip;
        this->before_mask = before_mask;
        this->before_port = before_port;

        this->after_ip = after_ip;
        this->after_mask = after_mask;
        this->after_port = after_port;

        this->nat_type = nat_type;
    }

    bool operator == (const Nat &nat){
        return (before_ip == nat.before_ip) && (before_mask == nat.before_mask) && (before_port == nat.before_port)
            && (after_ip == nat.after_ip) && (after_mask == nat.after_mask) && (after_port == nat.after_port)
            && (nat_type == nat.nat_type);
    }

    Nat(QString &str){
        QStringList lists = str.split('#');

        before_port = lists.at(0);
        before_ip = before_port.split('/').at(0);
        before_port = before_port.split('/').at(1);
        before_mask = before_port.split(':').at(0);
        before_port = before_port.split(':').at(1);

        after_port = lists.at(1);
        after_ip = after_port.split('/').at(0);
        after_port = after_port.split('/').at(1);
        after_mask = after_port.split(':').at(0);
        after_port = after_port.split(':').at(1);

        nat_type = lists.at(2);
    }

    QString to_string(){
        return before_ip + "/" + before_mask + ":" + before_port + "#"
             + after_ip + "/" + after_mask + ":" + after_port + "#"
             + nat_type;
    }

    QString before_ip;
    QString before_mask;
    QString before_port;

    QString after_ip;
    QString after_mask;
    QString after_port;

    QString nat_type;
};

struct Link{

    Link(){

    }

    Link(QString source_ip, QString source_port,
         QString dest_ip, QString dest_port,
         QString protocol, QString timeout, QString info){

        this->source_ip = source_ip;
        this->source_port = source_port;

        this->dest_ip = dest_ip;
        this->dest_port = dest_port;

        this->protocol = protocol;
        this->timeout = timeout;
        this->info = info;

        isVisible = true;
    }

    bool operator == (const Link &link){
        return (source_ip == link.source_ip && source_port == link.source_port
             && dest_ip == link.dest_ip && dest_port == link.dest_port
             && protocol == link.protocol && timeout == link.timeout && info == link.info);
    }

    Link(QString &str){
        QStringList items = str.split('#');

        source_ip = items.at(0);
        dest_ip = items.at(1);

        source_port = items.at(2);
        dest_port = items.at(3);

        protocol = items.at(4);

        if(protocol == "TCP"){
            info = "seq: " + items.at(6) + " acK_seq: " + items.at(7) + " tcp status: " + items.at(8);
        }
        timeout = items.at(9);

        if(items.size() == 12){
            info += "before ip: " + items.at(10);
            if(protocol != "ICMP"){
                info += " before port:" + items.at(11);
            }
        }
        isVisible = true;
    }

    QString to_string(){
        return source_ip + source_port + dest_ip + dest_port +
             protocol + timeout + info;
    }

    QString source_ip;
    QString source_port;

    QString dest_ip;
    QString dest_port;

    QString protocol;
    QString timeout;
    QString info;

    bool isVisible;

};

#endif // RULE_H
