#ifndef QRULE_H
#define QRULE_H

#include <QObject>
#include <QDebug>
#include <QString>

class QRule : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString source_ip   WRITE set_source_ip)
    Q_PROPERTY(QString source_mask WRITE set_source_mask)
    Q_PROPERTY(QString source_port WRITE set_source_port)

    Q_PROPERTY(QString dest_ip   WRITE set_dest_ip)
    Q_PROPERTY(QString dest_mask WRITE set_dest_mask)
    Q_PROPERTY(QString dest_port WRITE set_dest_port)

    Q_PROPERTY(QString protocol  WRITE set_protocol)
    Q_PROPERTY(QString chain     WRITE set_chain)
    Q_PROPERTY(QString action    WRITE set_action)
    Q_PROPERTY(QString log       WRITE set_log)

public:
    explicit QRule(QObject *parent = nullptr);

    void set_source_ip(const QString &source_ip);
    void set_source_mask(const QString &source_mask);
    void set_source_port(const QString &source_port);

    void set_dest_ip(const QString &dest_ip);
    void set_dest_mask(const QString &dest_mask);
    void set_dest_port(const QString &dest_port);

    void set_protocol(const QString &protocol);
    void set_chain(const QString &chain);
    void set_action(const QString &action);
    void set_log(const QString &log);

    Q_INVOKABLE QString rule_to_string();

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

#endif // QRULE_H
