#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connexionButton_clicked();

    void on_deconnexionButton_clicked();

    void on_envoiButton_clicked();

    void gerer_donnees();

    void afficher_erreur(QAbstractSocket::SocketError);

    void mettre_a_jour_ihm();

    void demander_trame();

    void on_buttonSatellite_clicked();

    void on_buttonPlan_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QTimer *pTimer;
    QImage *pCarte;
    QImage *pCartePlan;
    QImage *pCarteSatellite;
    QSqlDatabase bdd;
    float lastpx;
    float lastpy;
    double lat_rad;
    double long_rad;
    double lastlat_rad;
    double lastlong_rad;
    double calcul_distance;
    double distance;
    double lastdistance;
    int timestamp;
    int last_timestamp;

};

#endif // MAINWINDOW_H
