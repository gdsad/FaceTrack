#ifndef TRACKINGVIDEO_H
#define TRACKINGVIDEO_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/videoio.hpp>

#include "facetracking.h"
#include "logfile.h"

#include <QWidget>
#include <QThread>
#include <QObject>

#include <iostream>
#include <time.h>

#define OUTPUT_VIDEO_H      563
#define OUTPUT_VIDEO_W      1000
#define TEMPORARY_LOG   true

class Worker : public QObject
{
    Q_OBJECT
public:
    facetracking tracker;
    cv::Mat kf_image;
    cv::VideoCapture cap;
    logfile logfile;

public slots:
    void doWork();

signals:
    void resultReady();
    void updateLoadingBar(int);
};

namespace Ui {
class TrackingVideo;
}

class TrackingVideo : public QWidget
{
    Q_OBJECT

public:
    explicit TrackingVideo(QWidget *parent = nullptr);
    ~TrackingVideo();
    cv::VideoCapture cap;
    cv::Mat kf_image, frame, prev_frame;
    facetracking tracker;
    logfile logfile;
    QString nombre, apellido, dni, fecha, edad;

public slots:
    void procesarvideo();
    void videoprocesado();

private slots:
    void on_boton_resultados_clicked();
    void on_boton_reproducir_clicked();
    void resizeEvent(QResizeEvent *event);

signals:
    void show_results();

private:
    Ui::TrackingVideo *ui;
};



#endif // TRACKINGVIDEO_H
