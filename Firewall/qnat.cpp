#include "qnat.h"

QNat::QNat(QObject *parent)
{

}

void QNat::set_before_ip(const QString &before_ip)
{
    this->before_ip = before_ip;
}

void QNat::set_before_mask(const QString &before_mask)
{
    if(before_mask == "N/A"){
        this->before_mask = "";
    }else{
        this->before_mask = before_mask;
    }
}

void QNat::set_before_port(const QString &before_port)
{
    if(before_port == "N/A"){
        this->before_port = "";
    }else{
        this->before_port = before_port;
    }
}

void QNat::set_after_ip(const QString &after_ip)
{
    this->after_ip = after_ip;
}

void QNat::set_after_mask(const QString &after_mask)
{
    if(after_mask == "N/A"){
        this->after_mask = "";
    }else{
        this->after_mask = after_mask;
    }
}

void QNat::set_after_port(const QString &after_port)
{
    if(after_port == "N/A"){
        this->after_port = "";
    }else{
        this->after_port = after_port;
    }
}

void QNat::set_nat_type(const QString &nat_type)
{
    this->nat_type = nat_type;
}

QString QNat::nat_to_string() {
    return before_ip + "/" + before_mask + ":" + before_port + "#"
         + after_ip + "/" + after_mask + ":" + after_port + "#"
         + nat_type;
}
