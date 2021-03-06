#ifndef PHARMACIEAPI_H
#define PHARMACIEAPI_H

#include "abstractapi.h"

#include <QtNetwork>


class PharmacieApi : public AbstractApi
{
    Q_OBJECT

public:
    PharmacieApi(QObject *parent, int id);

    QNetworkRequest request;
    QNetworkReply *reply;

    QString rayon = "500";

private slots:
    void listePharmacie(QNetworkReply *reply);
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError);
    void slotSslErrors(QList<QSslError>);
};

#endif // PHARMACIEAPI_H
