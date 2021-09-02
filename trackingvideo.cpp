#include "trackingvideo.h"
#include "ui_trackingvideo.h"
#include "MatToQImage.h"
#include "facetracking.h"
#include "QProgressBar"
#include <QGraphicsDropShadowEffect>

void Worker::doWork(){
    //referencia https://stackoverflow.com/questions/35673201/qt-5-update-qprogressbar-during-qthread-work-via-signal/35673612
    cv::Mat frame, prev_frame;
    prev_frame = GetFittedImage(kf_image,OUTPUT_VIDEO_H,OUTPUT_VIDEO_W);
    tracker.candide->pose_tracking_indxs = new std::vector<int>{ 37,94,38,111,112,29,62,23,56, 5 }; // Quite el vertice 10 pq sino al mover la boca la mascara se mueve

    //Guardo resultado en un video para luego mostrarlo
    std::string out = logfile.filename.substr(0, logfile.filename.find(".")) + std::string("_tracking.avi");
    cv::VideoWriter video(out, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), cap.get(cv::CAP_PROP_FPS), cv::Size(OUTPUT_VIDEO_W, OUTPUT_VIDEO_H));

    // Init Log File
    logfile.start_log(tracker);
    logfile.write_log(tracker,cap.get(cv::CAP_PROP_POS_FRAMES));

    //Init Tracking
    bool detected = false;
    cv::Rect face = tracker.detectFace(prev_frame, detected);

    if (detected) {
        detected = false;
        tracker.facelandmarks(prev_frame, face, detected);
        if (detected){
            for (cv::Point2f lp : tracker.landmarks)
            {
                tracker.stabilized_landmarks.emplace_back(lp);
            }
        }
    }

    while(cap.read(frame)){
        frame = GetFittedImage(frame,OUTPUT_VIDEO_H,OUTPUT_VIDEO_W);
        cv::Mat frame_orig = frame.clone();

        // ======> Tracking
        tracker.step(prev_frame, frame);
        prev_frame = frame.clone();

        // Write Log
        logfile.write_log(tracker,cap.get(cv::CAP_PROP_POS_FRAMES));

        // Guardo video procesado
        video.write(tracker.candide->graficar_mesh(frame));

        // Actualizo Loading Bar
        emit updateLoadingBar(cap.get(cv::CAP_PROP_POS_FRAMES));
    }

    //End Log
    logfile.end_log();

    emit resultReady();
}

TrackingVideo::TrackingVideo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackingVideo)
{
    std::cout << "Inicializando TrackingVideo..." << std::endl;
    ui->setupUi(this);
    ui->boton_reproducir->setDisabled(true);
    ui->boton_resultados->setDisabled(true);
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    ui->fecha->setText(QString::number(now->tm_mday) + "/" + QString::number(now->tm_mon + 1) + "/" + QString::number(now->tm_year + 1900));

    // Sombras
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setXOffset(4);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 100));
    ui->progressBar->setGraphicsEffect(shadow);

    QGraphicsDropShadowEffect *shadow_img = new QGraphicsDropShadowEffect();
    shadow_img->setBlurRadius(40);
    shadow_img->setXOffset(10);
    shadow_img->setYOffset(10);
    shadow_img->setColor(QColor(0, 0, 0, 130));
    ui->img_label->setGraphicsEffect(shadow_img);

    QGraphicsDropShadowEffect *shadow_button = new QGraphicsDropShadowEffect();
    shadow_button->setBlurRadius(20);
    shadow_button->setXOffset(0);
    shadow_button->setYOffset(0);
    shadow_button->setColor(QColor(0, 0, 0, 200));
    ui->boton_resultados->setGraphicsEffect(shadow_button);

    // Size
    QSizePolicy img_size;
    img_size.setHorizontalPolicy(QSizePolicy::Minimum);
    img_size.setVerticalPolicy(QSizePolicy::Minimum);
    ui->img_label->setSizePolicy(img_size);

    // Bug cuando el video es largo si escribo en las LineEdit la barra de progreso
    // deja de actualizarse, por lo que pongo visible los campos luego de que se procese
    // el video.
    ui->nombre->setVisible(false);
    ui->apellido->setVisible(false);
    ui->edad->setVisible(false);
    ui->dni->setVisible(false);
    ui->fecha->setVisible(false);
    ui->label_nombre->setVisible(false);
    ui->label_apellido->setVisible(false);
    ui->label_edad->setVisible(false);
    ui->label_dni->setVisible(false);
    ui->label_fecha->setVisible(false);
    ui->label_datos->setVisible(false);

    std::cout << "TrackingVideo inicializado." << std::endl;
}

TrackingVideo::~TrackingVideo()
{
    delete ui; 
}

void TrackingVideo::resizeEvent(QResizeEvent *event){
   ui->img_label->setMinimumHeight(int(ui->img_label->width()*0.5625) );
   if(ui->boton_reproducir->isEnabled()){
       qDebug() << "Entro a resizeEvent TrackingVideo";
       ui->img_label->setPixmap(ui->img_label->pixmap()->scaled(ui->img_label->width(),ui->img_label->width()*0.5625));

   }
}

void TrackingVideo::procesarvideo(){

    tracker.candide->pose_tracking_indxs = new std::vector<int>{ 37,94,38,111,112,29,62,23,56, 5 }; // Quite el vertice 10 pq sino al mover la boca la mascara se mueve

    ui->progressBar->setRange(cap.get(cv::CAP_PROP_POS_FRAMES), cap.get(cv::CAP_PROP_FRAME_COUNT));
    ui->progressBar->setValue(cap.get(cv::CAP_PROP_POS_FRAMES));
    ui->progressBar->setTextVisible(true);
    ui->progressBar->setAlignment(Qt::AlignCenter);
    ui->progressBar->setHidden(false);

    QThread* thread = new QThread;
    Worker* worker = new Worker; // Do not set a parent. The object cannot be moved if it has a parent.
    worker->tracker = tracker;
    worker->kf_image = kf_image;
    worker->cap = cap;
    worker->logfile.set_log_filename(logfile.filename, TEMPORARY_LOG);
    worker->logfile.set_total_frames(cap.get(cv::CAP_PROP_FRAME_COUNT) - cap.get(cv::CAP_PROP_POS_FRAMES));
    worker->moveToThread(thread);

    connect(worker, SIGNAL(updateLoadingBar(int)), ui->progressBar, SLOT(setValue(int)));
    connect(thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
    connect(worker, SIGNAL(resultReady()), this, SLOT(videoprocesado()));

    thread->start();
}

void TrackingVideo::videoprocesado()
{
    ui->progressBar->setFormat("PROCESADO ✔");
    ui->progressBar->setAlignment(Qt::AlignCenter);
    ui->boton_reproducir->setDisabled(false);
    ui->boton_resultados->setDisabled(false);

    // Bug cuando el video es largo si escribo en las LineEdit la barra de progreso
    // deja de actualizarse, por lo que pongo visible los campos luego de que se procese
    // el video.
    ui->nombre->setVisible(true);
    ui->apellido->setVisible(true);
    ui->edad->setVisible(true);
    ui->dni->setVisible(true);
    ui->fecha->setVisible(true);
    ui->label_nombre->setVisible(true);
    ui->label_apellido->setVisible(true);
    ui->label_edad->setVisible(true);
    ui->label_dni->setVisible(true);
    ui->label_fecha->setVisible(true);
    ui->label_datos->setVisible(true);

    cv::VideoCapture processed;
    processed.open(logfile.filename.substr(0, logfile.filename.find(".")) + std::string("_tracking.avi"));
    processed.read(frame);
    QImage imgQ = MatToQImage(frame);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));
    processed.release();
}

void TrackingVideo::on_boton_resultados_clicked()
{
    nombre= ui->nombre->text();
    apellido= ui->apellido->text();
    edad= ui->edad->text();
    dni= ui->dni->text();
    fecha= ui->fecha->text();
    emit show_results();
}

void TrackingVideo::on_boton_reproducir_clicked()
{
    cv::VideoCapture processed;
    processed.open(logfile.filename.substr(0, logfile.filename.find(".")) + std::string("_tracking.avi"));
    while(processed.read(frame)){
        QImage imgQ = MatToQImage(frame);
        ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));
        cv::waitKey(1000/processed.get(cv::CAP_PROP_FPS));
    }
}
