#ifndef APISNCF_H
#define APISNCF_H

#include "abstractapi.h"

class SncfApi : public AbstractApi
{
    Q_OBJECT

public:
    SncfApi(int id = 0,QObject *parent=0);
    QNetworkAccessManager *manager_prochain_depart;
    QString code_uic_gare;
    qint64 t1;
    qint64 t2;

private slots:
    void result_liste_gare(QNetworkReply *reply);
    void result_prochain_depart(QNetworkReply* reply);
    //void button_clicked();
};

#endif // APISNCF_H
