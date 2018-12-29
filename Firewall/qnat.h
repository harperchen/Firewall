#ifndef QNAT_H
#define QNAT_H

#include <QObject>
#include <QString>

class QNat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString before_ip   WRITE set_before_ip)
    Q_PROPERTY(QString before_mask WRITE set_before_mask)
    Q_PROPERTY(QString before_port WRITE set_before_port)

    Q_PROPERTY(QString after_ip   WRITE set_after_ip)
    Q_PROPERTY(QString after_mask WRITE set_after_mask)
    Q_PROPERTY(QString after_port WRITE set_after_port)

    Q_PROPERTY(QString nat_type WRITE set_nat_type)

public:
    QNat(QObject *parent = nullptr);

    void set_before_ip(const QString &before_ip);
    void set_before_mask(const QString &before_mask);
    void set_before_port(const QString &before_port);

    void set_after_ip(const QString &after_ip);
    void set_after_mask(const QString &after_mask);
    void set_after_port(const QString &after_port);

    void set_nat_type(const QString &nat_type);

    Q_INVOKABLE QString nat_to_string();

    QString before_ip;
    QString before_mask;
    QString before_port;

    QString after_ip;
    QString after_mask;
    QString after_port;

    QString nat_type;
};

#endif // QNAT_H


