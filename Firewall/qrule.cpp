#include "qrule.h"

QRule::QRule(QObject *parent)
{

}


void QRule::set_source_ip(const QString &source_ip){
    this->source_ip = source_ip;
}

void QRule::set_source_mask(const QString &source_mask){
    if(source_mask == "N/A"){
        this->source_mask = "";
    }
    else{
        this->source_mask = source_mask;
    }
}

void QRule::set_source_port(const QString &source_port){
    if(source_port == "N/A"){
        this->source_port = "";
    }
    else{
        this->source_port = source_port;
    }
}

void QRule::set_dest_ip(const QString &dest_ip){
    this->dest_ip = dest_ip;
}

void QRule::set_dest_mask(const QString &dest_mask){
    if(dest_mask == "N/A"){
        this->dest_mask = "";
    }
    else{
        this->dest_mask = dest_mask;
    }
}

void QRule::set_dest_port(const QString &dest_port){
    if(dest_port == "N/A"){
        this->dest_port = "";
    }
    else{
        this->dest_port = dest_port;
    }
}

void QRule::set_protocol(const QString &protocol){
    this->protocol = protocol;
}


void QRule::set_chain(const QString &chain){
    this->chain = chain;
}

void QRule::set_action(const QString &action){
    this->action = action;
}

void QRule::set_log(const QString &log){
    this->log = log;
}

QString QRule::rule_to_string() {
    return source_ip + "/" + source_mask + ":" + source_port + "#"
         + dest_ip + "/" + dest_mask + ":" + dest_port + "#"
         + protocol + "#" + chain + "#"
         + action + "#" + log;
}
