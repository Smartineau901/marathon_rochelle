#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Initialisation de l'interface graphique
    ui->setupUi(this);

    // Instanciation de la socket
    tcpSocket = new QTcpSocket(this);

    // Attachement d'un slot qui sera appelé à chaque fois que des données arrivent (mode asynchrone)
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(gerer_donnees()));

    // Idem pour les erreurs
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(afficher_erreur(QAbstractSocket::SocketError)));


    pTimer = new QTimer();

    // Association du "tick" du timer à l'appel d'une méthode SLOT
    connect(pTimer, SIGNAL(timeout()), this, SLOT(demander_trame()));

    // Instanciation de l'image (dans constructeur)
    pCarte = new QImage();
    // Chargement depuis un fichier
    pCarte->load(":/PlanTransparent.png");
    // Affichage dans un QLabel, ici label_carte
    ui->labelCarteTransparente->setPixmap(QPixmap::fromImage(*pCarte));

    pCartePlan = new QImage();
    // Chargement depuis un fichier
    pCarte->load(":/carte_la_rochelle_plan.png");
    // Affichage dans un QLabel, ici label_carte
    ui->labelCartePlan->setPixmap(QPixmap::fromImage(*pCartePlan));

    pCarteSatellite = new QImage();
    // Chargement depuis un fichier
    pCarte->load(":/carte_la_rochelle_satellite.png");
    // Affichage dans un QLabel, ici label_carte
    ui->labelCarteSattelite->setPixmap(QPixmap::fromImage(*pCarteSatellite));





}


MainWindow::~MainWindow()
{
    // Destruction de la socket
    tcpSocket->abort();
    delete tcpSocket;

    // Arrêt du timer
    pTimer->stop();

    // Destruction de l'objet
    delete pTimer;

    delete pCarte;
    delete pCartePlan;
    delete pCarteSatellite;
    // Destruction de l'interface graphique
    delete ui;
}

void MainWindow::on_connexionButton_clicked()
{
    // Récupération des paramètres
    QString adresse_ip = ui->lineEdit_ip->text();
    unsigned short port_tcp = ui->lineEdit_port->text().toInt();

    // Connexion au serveur
    tcpSocket->connectToHost(adresse_ip, port_tcp);
    pTimer->start(1000);
}

void MainWindow::on_deconnexionButton_clicked()
{
    // Déconnexion du serveur
    tcpSocket->close();
    pTimer->stop();

}

void MainWindow::on_envoiButton_clicked()
{
    // Préparation de la requête
    QByteArray requete;
    requete = "RETR\r\n";

    // Envoi de la requête
    tcpSocket->write(requete);
}


void MainWindow::gerer_donnees()
{
    // Réception des données
    QByteArray reponse = tcpSocket->readAll();
    QString trame = QString(reponse);
    QStringList liste = trame.split(",");
    int age = ui->Age->value();




    QString postype = liste[6];
    qDebug() << "trame : " <<trame;

    // Affichage
    ui->lineEdit_reponse->setText(trame);


    //Décodage de la Trame

    QString horaire = liste[1];
    int heures = liste[1].mid(0,2).toInt();
    int minutes = liste[1].mid(2,2).toInt();
    int secondes = liste[1].mid(4,2).toInt();
    qDebug() << "heure :"<<  liste[1].mid(0,2);
    qDebug() << "minutes :"<<  liste[1].mid(2,2);
    qDebug() << "secondes :"<<  liste[1].mid(4,2);
    int timestamp = (heures * 3600 + minutes * 60 + secondes);
    qDebug() << "timestamp : " << timestamp;
    QString timestampQString = QString ("%1").arg(timestamp);

        // Latitude

    QString Latitude = liste[2];
    double degres_lat = liste[2].mid(0,2).toDouble();
    qDebug() << "Degrés :" << degres_lat ;
    double minutes_lat = liste[2].mid(2,7).toDouble();
    qDebug() <<"Latitude_Minutes :" << minutes_lat;

    double latitude = (degres_lat + minutes_lat/ 60.0);
    qDebug() <<"Latitude :" << latitude;
    //Mise en okace de latitude dans le lineEdit
    ui->Latitude->setText(QString::number(latitude));


    QString N_or_S = liste[3];
    if( N_or_S == "S"){
        latitude = (degres_lat + (minutes_lat / 60))*(-1);
    }else if(N_or_S == "N"){
        latitude = degres_lat + (minutes_lat / 60);

    }else{
        latitude =(degres_lat + (minutes_lat / 60));
    }


        //Longitude degrés et minutes
    double degres_long = liste[4].mid(0,3).toDouble();
    qDebug() << "Degrés :" << degres_long ;
    double minutes_long = liste[4].mid(3,7).toDouble();
    qDebug() <<"Latitude_Minutes :" << minutes_long;

        //Calcule de la Longitude
    double longitude = (degres_long + minutes_long / 60);
    qDebug() <<"Longitude :" << longitude;
    ui->Longitude->setText(QString::number(longitude));




    QString W_or_E = liste[5];
    if( W_or_E == "W")
    {
        longitude = (degres_long + (minutes_long / 60))*(-1);
    }
    else if(W_or_E == "E")
    {
        longitude = degres_long + (minutes_long / 60);
    }
    else
    {
        longitude =(degres_long + (minutes_long / 60));
    }

    //Type de positionnement
    int positionnement = liste[6].mid(0,1).toInt();
    if (liste[6].mid(0,1).toInt() == 1)
    {
        qDebug() << "Type de positionnement : GPS";
    }

    else
    {
        qDebug() << "Type de positionnement :" << positionnement;
    }

    //Nb de Satellites
    QString nb_satellite = liste[7];
    nb_satellite = liste[7].mid(0,2);
    qDebug() << "Nombre de Satellites  :" << nb_satellite ;

     //Précision horizontale
    QString precision_horizontale = liste[8];
    qDebug() <<"Précision Horizontale :" << precision_horizontale;

     //Affichache Altitude
    QString altitude = liste[9];
    qDebug() <<"Altitude du Courreur :" << altitude;

     //Unité de l'Altitude
    QString unite_altitude = liste[10];
    qDebug() << "Altitude en :" << unite_altitude;

    QString hauteur_geo = liste[11];
    qDebug() << "Hauteur en Géodésique au dessus de l'éllipsoïde WGS84 " << hauteur_geo;

    QString unite_hauteur = liste[12];
    qDebug() << "Géodésique en :" << unite_altitude;

    QString tps_last_maj = liste[13];
    qDebug() << "Temps depuis la dernière mise à jours : " << tps_last_maj;


    QString frequence_cardiaque = liste[14].mid(1,3);
    qDebug() << "Fréquence Cardiaque du courreur :" << frequence_cardiaque;
    ui->Frequence_cardiaque->setText(frequence_cardiaque);

    int frequence_cardiaque_max = 220 - age;
    ui->FC_max->setText(QString::number(frequence_cardiaque_max));




    float px = 694.0 * ((longitude + 1.195703) / ( -1.136125 + 1.195703) );
    float py = 638.0 * ( 1.0 - (latitude - 46.135451) / (46.173311 - 46.135451));


    qDebug() << "px :"<< px  ;
    qDebug() << "py :"<< py  ;

    // Préparation du contexte de dessin sur une image existante
    QPainter p(pCarte);
    // Choix de la couleur
    p.setPen(Qt::red);

    if ((lastpx != 0.0) && (lastpy != 0.0))
    {
    // Dessin d'une ligne
    p.drawLine(lastpx, lastpy, px, py);
    // Fin du dessin et application sur l'image
    p.end();
    ui->labelCarteTransparente->setPixmap(QPixmap::fromImage(*pCarte));
    }

    else{
    }
    lastpx = px;
    lastpy = py;

    //bool;

}

void MainWindow::demander_trame()
{
    qDebug() << "tic";

    QByteArray requete;
    requete = "RETR\r\n";
    // Envoi de la requête
    tcpSocket->write(requete);

}

void MainWindow::mettre_a_jour_ihm()
{
    qDebug() <<"tic";
}

void MainWindow::afficher_erreur(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Hôte introuvable"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Connexion refusée"));
            break;
        default:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Erreur : %1.")
                                     .arg(tcpSocket->errorString()));
    }
}
