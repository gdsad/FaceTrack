#ifndef RESULTS_H
#define RESULTS_H

#include <QWidget>
#include "logfile.h"
#include <QGraphicsView>
#include <QtCharts>
#include <QPdfWriter>
#include <QPainter>
#include <QDesktopServices>
#include <QTableWidget>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <MatToQImage.h>
#include <iostream>
#include <iomanip>
#include <exception>


/* Estas dos definiciones son importantes para cuando se exportan los puntos 2D re-escalados */
#define TARGET_WIDTH    1000
#define TARGET_HEIGHT   563

namespace Ui {
class Results;
}

class Results : public QWidget
{
    Q_OBJECT

public:
    explicit Results(QWidget *parent = nullptr);
    ~Results();

    cv::VideoCapture cap;
    cv::Mat kf_image;
    std::string filename;
    int max_frame, max_seconds;
    datalog trackingdata;
    QString nombre, apellido, dni, fecha, edad;
    QChart *GraphGestos=new QChart(); 

    QBarSeries *maximos = new QBarSeries();
    QChart *GraphDistOjoSupInf=new QChart();
    QChart *GraphDistOjoBoca=new QChart();
    QChart *maximosGestos = new QChart();

    QLineSeries *lineaVertical = new  QLineSeries();
    QLineSeries *lineaVertical1 = new  QLineSeries();
    QLineSeries *lineaVertical2 = new  QLineSeries();
    QSplineSeries *distOjoBocaL = new QSplineSeries();
    QSplineSeries *distOjoBocaR = new QSplineSeries();
    QSplineSeries *distOjoSupInfL = new QSplineSeries();
    QSplineSeries *distOjoSupInfR = new QSplineSeries();

    vector <float> maxGestos,maxDist,minDist,promDist;

    std::vector<QSplineSeries*> gestos;
    float umbral_elevar_ceja_R = 0.01f, umbral_elevar_ceja_L = 0.01f, umbral_fruncir_ceno_R = 0.01f, umbral_fruncir_ceno_L = 0.01f,
          umbral_sonreir_R = 0.01f, umbral_sonreir_L = 0.01f, umbral_beso_L = 0.01f, umbral_beso_R = 0.01f, umbral_cerrar_ojo_R = 0.01f,
          umbral_cerrar_ojo_L = 0.01f, umbral_cerrar_ojo_fuerte_R = 0.01f, umbral_cerrar_ojo_fuerte_L = 0.01f;

public slots:
    void graficar_distancias(string medida);
    void grafica_dinamica_gestos();
    void update_gestos(int status);
    void graficar_maximos();
    void setValues();
    virtual bool eventFilter(QObject *object, QEvent *ev) override;


private slots:
    void on_PDF_clicked();
    void on_exp_gestos_clicked();
    void on_exp_2d_clicked();
    void on_exp_3d_clicked();

private:
    Ui::Results *ui;
};


#endif // RESULTS_H
