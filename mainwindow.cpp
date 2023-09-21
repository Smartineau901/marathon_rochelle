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
    pCarte->load(":/carte_la_rochelle_plan.png");
    // Affichage dans un QLabel, ici label_carte
    ui->labelCarte->setPixmap(QPixmap::fromImage(*pCarte));


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
    QString horaire = liste[1];
    QString lat = liste[2];
    QString N_or_S = liste[3];
    QString lon = liste[4];
    QString W_or_E = liste[5];
    QString postype = liste[6];
    QString nb_satellite = liste[7];
    QString precision_horizontale = liste[8];
    QString altitude = liste[9];
    QString unite_altitude = liste[10];
    QString hauteur_geo = liste[11];
    QString unite_hauteur = liste[12];
    QString tps_last_maj = liste[13];
    QString frequence_cardiaque = liste[14];
    qDebug() << "trame : " <<trame;

    // Affichage
    ui->lineEdit_reponse->setText(trame);


    //Décodage de la Trame

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
    double degres_lat = liste[2].mid(0,2).toDouble();
    qDebug() << "Degrés :" << degres_lat ;
    double minutes_lat = liste[2].mid(2,3).toDouble();
    qDebug() <<"Latitude_Minutes :" << minutes_lat;

    double latitude = (degres_lat + minutes_lat/ 60);
    qDebug() <<"Latitude :" << latitude;


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

    if( W_or_E == "W"){
        longitude = (degres_long + (minutes_long / 60))*(-1);
    }else if(W_or_E == "E"){
        longitude = degres_long + (minutes_long / 60);

    }else{
        longitude =(degres_long + (minutes_long / 60));
    }

    //Type de positionnement
    int positionnement = liste[6].mid(0,1).toInt();
    if (liste[6].mid(0,1).toInt() == 1)
    {
        qDebug() << "Type de positionnement : GPS";
    }
    else{
        qDebug() << "Type de positionnement :" << positionnement;
    }

    //Nb de Satellites
    nb_satellite = liste[7].mid(0,2);
    qDebug() << "Nombre de Satellites  :" << nb_satellite ;



    // Préparation du contexte de dessin sur une image existante
    QPainter p(pCarte);
    // Choix de la couleur
    p.setPen(Qt::red);
    // Dessin d'une ligne
    p.drawLine(10, 20, 250, 300);
    p.drawText(10,30, "Hello world");
    p.fillRect(10, 20, 250, 300, QColor(120, 255, 0));
    // Fin du dessin et application sur l'image
    p.end();
    ui->labelCarte->setPixmap(QPixmap::fromImage(*pCarte));


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
