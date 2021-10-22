#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Ventanas de la aplicacion
#include "keyframeselection.h"
#include "modelfitting.h"
#include "trackingvideo.h"
#include "results.h"
#include "onlymesh.h"

// Librerias propias
#include "facialmesh.h"
#include "facetracking.h"

// Archivo con mascara Candide
#define CANDIDE_MODEL       "candide3_v3.wfm"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    facialmesh *candide;
    bool pressed;
    QPoint current;
    uint8_t GLOBAL_STATE = 0;


public slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private slots:

    void move2modelfitting();
    void on_boton_empezar_clicked();
    void move2trackingvideo();
    void move2results();
    void move2init();

    void on_btn_close_clicked();
    void on_btn_minimize_clicked();
    void on_btn_maximize_clicked();
    void on_boton_toggle_clicked();
    void on_boton_mascara_clicked();
    void on_boton_inicio_clicked();
    void on_boton_about_clicked();
    void on_boton_salir_clicked();
    void on_boton_instrucciones_clicked();

private:
    Ui::MainWindow *ui;
    KeyFrameSelection kf_selection;
    ModelFitting m_fitting;
    TrackingVideo tr_video;
    Results results;
    OnlyMesh mesh;
    int prev_index;

};
#endif // MAINWINDOW_H
