#include "keyframeselection.h"
#include "ui_keyframeselection.h"
#include <iostream>
#include <string>
#include <QFileDialog>
#include "MatToQImage.h"
#include <QGraphicsDropShadowEffect>

KeyFrameSelection::KeyFrameSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyFrameSelection)
{
    std::cout << "Inicializando KFSelection..." << std::endl;
    ui->setupUi(this);
    ui->video_slider->setDisabled(true);
    ui->select_keyframe->setDisabled(true);
    connect(ui->video_slider,SIGNAL(valueChanged(int)),this,SLOT(actualizar_frame()));

    // Sombras
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(40);
    shadow->setXOffset(10);
    shadow->setYOffset(10);
    shadow->setColor(QColor(0, 0, 0, 130));
    ui->img_label->setGraphicsEffect(shadow);

    QGraphicsDropShadowEffect *shadow_open = new QGraphicsDropShadowEffect();
    shadow_open->setBlurRadius(20);
    shadow_open->setXOffset(0);
    shadow_open->setYOffset(0);
    shadow_open->setColor(QColor(0, 0, 0, 100));
    ui->open_video->setGraphicsEffect(shadow_open);

    QGraphicsDropShadowEffect *shadow_convertir = new QGraphicsDropShadowEffect();
    shadow_convertir->setBlurRadius(20);
    shadow_convertir->setXOffset(0);
    shadow_convertir->setYOffset(0);
    shadow_convertir->setColor(QColor(0, 0, 0, 200));
    ui->convertir->setGraphicsEffect(shadow_convertir);

    QGraphicsDropShadowEffect *shadow_select = new QGraphicsDropShadowEffect();
    shadow_select->setBlurRadius(20);
    shadow_select->setXOffset(0);
    shadow_select->setYOffset(0);
    shadow_select->setColor(QColor(0, 0, 0, 200));
    ui->select_keyframe->setGraphicsEffect(shadow_select);

    QSizePolicy img_size;
    img_size.setHorizontalPolicy(QSizePolicy::Minimum);
    img_size.setVerticalPolicy(QSizePolicy::Minimum);
    ui->img_label->setSizePolicy(img_size);

    std::cout << "KFSelection inicializado." << std::endl;
}

KeyFrameSelection::~KeyFrameSelection()
{
    delete ui;
}

void KeyFrameSelection::actualizar_frame()
{
    int frame_number  = ui->video_slider->value();
    if (frame_number < cap.get(cv::CAP_PROP_FRAME_COUNT)-1)
    {
        cap.set(cv::CAP_PROP_POS_FRAMES,frame_number - 1);
        cap.read(frame);
        ui->txt_frame_actual->setText(QString::number(frame_number));
    }
    frame = GetFittedImage(frame,563,1000);
    //QImage imgQ = MatToQImage(GetFittedImage(frame,ui->img_label->height(),ui->img_label->width()));
    QImage imgQ = MatToQImage(frame);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(), ui->img_label->width()*0.5625));
}

void KeyFrameSelection::resizeEvent(QResizeEvent *event){
    if(cap.isOpened()){
        ui->img_label->setMinimumHeight(int(ui->img_label->width()*0.5625) );
        QImage imgQ = MatToQImage(frame);
        ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(), ui->img_label->width()*0.5625));
    }
}


void KeyFrameSelection::on_open_video_clicked()
{
    QString shape_dir;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Video"), shape_dir, tr("Video Files (*.avi *.mp4 *.mov *.mkv)"));
    if(fileName == NULL)
        return;
    std::cout << "Se cargara el archivo de video: " << fileName.toUtf8().constData() << std::endl;

    // Cargar video
    cap.open(fileName.toStdString());
    if (!cap.isOpened()) {
        int ret = QMessageBox::critical(this, tr("Error abriendo el video."),
                                       tr("Archivo corrupto o formato \n"
                                          "de video no aceptado."),
                                       QMessageBox::Ok);
        ui->video_slider->setDisabled(true);
        return;
    }
    else{
        ui->video_slider->setDisabled(false);
        ui->select_keyframe->setDisabled(false);
        int totalframes = cap.get(cv::CAP_PROP_FRAME_COUNT);
        //std::cout << "Total Frames: " << totalframes << std::endl;
        //std::cout << cap.get(cv::CAP_PROP_FOURCC) << std::endl;
        ui->video_slider->setRange(0,totalframes);
        ui->txt_frame_max->setText(QString::number(totalframes));
    }
    cap.set(cv::CAP_PROP_POS_FRAMES,0);
    cap.read(frame);

    ui->img_label->setMinimumHeight(int(ui->img_label->width()*0.5625) );
    frame = GetFittedImage(frame,563,1000);
    QImage imgQ = MatToQImage(frame);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->height()));
    filename = fileName.toStdString();

}

void KeyFrameSelection::on_convertir_clicked()
{
    QString shape_dir;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Video"), shape_dir, tr("Video Files (*.mp4 *.mov *.mkv)"));
    if(fileName == NULL)
        return;
    // Cargar video
    cap.open(fileName.toStdString());
    if (!cap.isOpened()) {
        int ret = QMessageBox::critical(this, tr("Error abriendo el video."),
                                       tr("Archivo corrupto o formato \n"
                                          "de video no aceptado."),
                                       QMessageBox::Ok);
        ui->video_slider->setDisabled(true);
        return;
    }
    else{
    std::cout << "Se convertira el archivo de video: " << fileName.toUtf8().constData() << std::endl;
    int ex = static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));
    char aux;
    aux = ex & 0XFF;
    std::cout << aux << std::endl;
    aux = (ex & 0XFF00) >> 8;
    std::cout << aux << std::endl;
    aux = (ex & 0XFF0000) >> 16;
    std::cout << aux << std::endl;
    aux = (ex & 0XFF000000) >> 24;
    std::cout << aux << std::endl;

    QFileInfo info(fileName);
    QString data_dir = info.absolutePath();
    std::cout << "Se va a hacer el remplazo de nombre" << std::endl;
    std::string out = fileName.toStdString().substr(0,fileName.toStdString().find_last_of(".")) + std::string(".avi");
    std::cout << out << std::endl;
    cv::VideoWriter video(out ,cv::VideoWriter::fourcc('M','J','P','G'),cap.get(cv::CAP_PROP_FPS), cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH),cap.get(cv::CAP_PROP_FRAME_HEIGHT)));
    cv::Mat out_frame;
    while(cap.read(out_frame))
    {
        cv::flip(out_frame, out_frame, 0);
        video.write(out_frame);
    }
    video.release();
    cap.release();
    std::cout << "Video Convertido." << std::endl;
  }

}

void KeyFrameSelection::on_select_keyframe_clicked()
{
    frame = GetFittedImage(frame, 563,1000);
    emit keyframe_selected();
    //cap.release();
}
