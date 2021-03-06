#include "sncfapi.h"

SncfApi::SncfApi(int id,QObject *parent): AbstractApi(parent,id)
{
    //Crée les network acces managers
    manager = new QNetworkAccessManager(parent);
    manager_prochain_depart = new QNetworkAccessManager(parent);

    map_formulaire = new QMap<QString, QString>;

    //Attend la réponse des managers
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(result_liste_gare(QNetworkReply*)));
    connect(manager_prochain_depart, SIGNAL(finished(QNetworkReply*)), this, SLOT(result_prochain_depart(QNetworkReply*)));

    //Lance la requete
    //connect(ui->pushButton_go,SIGNAL(clicked(bool)),this,SLOT(button_clicked()));

    QString position = "geofilter.distance="+QString::number(latitude)+"%2C"+QString::number(longitude)+"%2C"+"10000"; // mettre le radian en parametre
    QString ul_liste_gare = "https://data.sncf.com/api/records/1.0/search/?dataset=liste-des-gares&"+position;
    QUrl url(ul_liste_gare);
    //qDebug() << ul_liste_gare;
    manager->get(QNetworkRequest(url));
}

/*
 * Fonction pour lancer la requete
void ApiSncf::button_clicked()
{
    t1 = QDateTime::currentMSecsSinceEpoch();
    QString position = "geofilter.distance="+ui->lineEdit_latitude->text()+"%2C"+ui->lineEdit_longitude->text()+"%2C"+"10000"; // mettre le radian en parametre
    QString ul_liste_gare = "https://data.sncf.com/api/records/1.0/search/?dataset=liste-des-gares&"+position;
    QUrl url(ul_liste_gare);
    qDebug() << ul_liste_gare;
    manager_liste_gare->get(QNetworkRequest(url));

}*/

void SncfApi::result_liste_gare(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
        return;  // ...only in a blog post

    // Now parse this JSON according to your needs !
    //qDebug() << "json liste gare";
    QJsonDocument jsdoc;
    jsdoc = QJsonDocument::fromJson(reply->readAll());
    QString libelle_gare = jsdoc.toVariant().toMap()["records"].toJsonArray().at(0).toVariant().toMap()["fields"].toMap()["libelle_gare"].toString();
    //qDebug() << libelle_gare;
    //ui->label_gare->setText(libelle_gare);
    map_formulaire->insert("Libelle Gare",libelle_gare);
    code_uic_gare = jsdoc.toVariant().toMap()["records"].toJsonArray().at(0).toVariant().toMap()["fields"].toMap()["code_uic"].toString();
    QString token_sncf = "b9b428ee-3f1c-4c4e-82c0-6448e4c99ed7";
    QString ul_prochain_depart = "https://"+token_sncf+"@api.sncf.com/v1/coverage/sncf/stop_areas/stop_area:OCE:SA:"+code_uic_gare+"/departures?datetime=20181212T174530";
    //qDebug() << ul_prochain_depart << "\t" << code_uic_gare;
    QUrl url(ul_prochain_depart);
    manager_prochain_depart->get(QNetworkRequest(url));

}


void SncfApi::result_prochain_depart(QNetworkReply* reply)
{

    //Parse le json
    QJsonDocument jsdoc;
    jsdoc = QJsonDocument::fromJson(reply->readAll());
    int total_result = jsdoc.toVariant().toMap()["pagination"].toMap()["total_result"].toInt();
    if(total_result == 0){
        //ui->textEdit->setPlainText("Pas de service");
        qDebug() << jsdoc.toVariant().toMap()["error"].toMap()["message"].toString();
        qDebug() << jsdoc.toVariant().toMap()["error"].toMap()["id"].toString();
    }
    else{
        int i = 0;
        QString direction;
        QString commercial_mode;
        QString code;
        QString ligne;
        QString affiche;
        QString date;
        //QString heure, minute;
        //QString string_color;

        for(i = 0; i < total_result ; i++){
            direction = jsdoc.toVariant().toMap()["departures"].toJsonArray().at(i).toObject().toVariantMap()["display_informations"].toMap()["direction"].toString();
            commercial_mode = jsdoc.toVariant().toMap()["departures"].toJsonArray().at(i).toObject().toVariantMap()["display_informations"].toMap()["commercial_mode"].toString();
            code = jsdoc.toVariant().toMap()["departures"].toJsonArray().at(i).toObject().toVariantMap()["display_informations"].toMap()["code"].toString();
            if(code == "" ) code = jsdoc.toVariant().toMap()["departures"].toJsonArray().at(i).toObject().toVariantMap()["display_informations"].toMap()["headsign"].toString();
            ligne = commercial_mode+ " " + code;
            date = jsdoc.toVariant().toMap()["departures"].toJsonArray().at(i).toObject().toVariantMap()["stop_date_time"].toMap()["departure_date_time"].toString();
            affiche = ligne+" "+date[9]+date[10]+"H"+date[11]+date[12]+" Direction "+direction+"\n";
            map_formulaire->insert("direction",direction);
            map_formulaire->insert("Ligne",ligne);
            map_formulaire->insert("Date",date);
            //ui->textEdit->insertPlainText(affiche);
            //QString string_color = jsdoc.toVariant().toMap()["departures"].toJsonArray().at(i).toObject().toVariantMap()["display_informations"].toMap()["color"].toString();
            //QColor color_ligne("#"+string_color);
        }

    }
    t2 = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "Time elapsed " << (t2-t1);
}
