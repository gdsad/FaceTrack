#include "modelfitting.h"
#include "ui_modelfitting.h"
#include "MatToQImage.h"
#include <iostream>
#include <fstream>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>

#include "facialmesh.h"

ModelFitting::ModelFitting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelFitting)
{
    ui->setupUi(this);
    std::cout << "Inicializando ModelFitting...." << std::endl;
    conectar();
    std::cout << "ModelFitting Inicializado." << std::endl;
}

ModelFitting::~ModelFitting()
{
    delete ui;
}

void ModelFitting::init(void){
    // Creo la lista de opciones para las Animation Units
    ui->animation_list->addItem("Seleccionar...");
    for(int k=0; k < int(candide->animation_parameters.size()); k++){
        ui->animation_list->addItem(candide->animation_parameters[k].name);
    }
    std::cout<< "----> Valores cargados en la lista de ANIMATION. " << std::endl;

    // Creo la lista de opciones para las Shape Units
    ui->shape_list->addItem("Seleccionar...");
    for(int k=0; k < int(candide->shape_parameters.size()); k++){
        ui->shape_list->addItem(candide->shape_parameters[k].name);
    }
    std::cout<< "----> Valores cargados en la lista de SHAPE. " << std::endl;

    // Seteo el indice de los tabs para que arranque en el primer Tab
    ui->tabWidget->setCurrentIndex(0);
    std::cout<< "----> Seteo posicion inicial del tab en POSE. " << std::endl;

    // Ubico mascara en el centro del espacio de trabajo
    candide->camera_matrix = (cv::Mat_<double>(3, 3) << 750, 0, 1000/2, 0, 750, 563/2, 0, 0, 1);

    // Sombras
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(40);
    shadow->setXOffset(10);
    shadow->setYOffset(10);
    shadow->setColor(QColor(0, 0, 0, 130));
    ui->img_label->setGraphicsEffect(shadow);

    QGraphicsDropShadowEffect *shadow_tab = new QGraphicsDropShadowEffect();
    shadow_tab->setBlurRadius(20);
    shadow_tab->setXOffset(5);
    shadow_tab->setYOffset(5);
    shadow_tab->setColor(QColor(0, 0, 0, 100));
    ui->tabWidget->setGraphicsEffect(shadow_tab);

    // Size
    QSizePolicy img_size;
    img_size.setHorizontalPolicy(QSizePolicy::Minimum);
    img_size.setVerticalPolicy(QSizePolicy::Minimum);
    ui->img_label->setSizePolicy(img_size);

}

void ModelFitting::conectar(){
    // Conexiones para actualizar la pose del modelo, cuando se cambie el
    // valor de los sliders se ejecutara la funcion actualizar_posicion()
    connect(ui->rotar_XZ, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->rotar_XY, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->rotar_YZ, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->trasladar_X, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->trasladar_Y, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->trasladar_Z, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    std::cout<< "----> Senales para control de POSE seteadas. " << std::endl;

    // Conexiones para actualizar la Animation del modelo
    connect(ui->animation_list,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_animation_slider()));
    connect(ui->animation_list,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_animation()));
    connect(ui->animation_slider,SIGNAL(valueChanged(int)), this, SLOT(actualizar_animation()));
    std::cout<< "----> Senales para control de ANIMATION seteadas. " << std::endl;

    // Conexiones para actualizar la Shape del modelo
    connect(ui->shape_list,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_shape_slider()));
    connect(ui->shape_list,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_shape()));
    connect(ui->shape_slider,SIGNAL(valueChanged(int)), this, SLOT(actualizar_shape()));
    std::cout<< "----> Senales para control de SHAPE seteadas. " << std::endl;

    // Actualizo los sliders cuando cambio de tab
    connect(ui->boton_reset, SIGNAL(clicked()),this, SLOT(actualizar_shape_slider()));
    connect(ui->boton_reset, SIGNAL(clicked()),this, SLOT(actualizar_animation_slider()));
    std::cout<< "----> Senales para actualizacion de sliders." << std::endl;

    // Grafico los vertices si se marca la check box
    connect(ui->animation_checkbox,SIGNAL(toggled(bool)),this,SLOT(actualizar_animation()));
    connect(ui->shape_checkbox,SIGNAL(toggled(bool)),this,SLOT(actualizar_shape()));
    std::cout<< "----> Senales de los CHECKBOX seteadas. " << std::endl;
}

void ModelFitting::desconectar(){
    // Conexiones para actualizar la pose del modelo, cuando se cambie el
    // valor de los sliders se ejecutara la funcion actualizar_posicion()
    disconnect(ui->rotar_XY, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->rotar_YZ, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->rotar_XZ, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->trasladar_X, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->trasladar_Y, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->trasladar_Z, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    std::cout<< "----> Senales para control de POSE desconectadas. " << std::endl;

    // Conexiones para actualizar la Animation del modelo
    disconnect(ui->animation_list,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_animation_slider()));
    disconnect(ui->animation_list,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_animation()));
    disconnect(ui->animation_slider,SIGNAL(valueChanged(int)), this, SLOT(actualizar_animation()));
    std::cout<< "----> Senales para control de ANIMATION desconectadas. " << std::endl;

    // Conexiones para actualizar la Shape del modelo
    disconnect(ui->shape_list,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_shape_slider()));
    disconnect(ui->shape_list,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_shape()));
    disconnect(ui->shape_slider,SIGNAL(valueChanged(int)), this, SLOT(actualizar_shape()));
    std::cout<< "----> Senales para control de SHAPE desconectadas. " << std::endl;

    // Actualizo los sliders cuando cambio de tab
    disconnect(ui->boton_reset, SIGNAL(clicked()),this, SLOT(actualizar_shape_slider()));
    disconnect(ui->boton_reset, SIGNAL(clicked()),this, SLOT(actualizar_animation_slider()));
    std::cout<< "----> Senales para actualizacion de sliders desconectadas." << std::endl;

    // Grafico los vertices si se marca la check box
    disconnect(ui->animation_checkbox,SIGNAL(toggled(bool)),this,SLOT(actualizar_animation()));
    disconnect(ui->shape_checkbox,SIGNAL(toggled(bool)),this,SLOT(actualizar_shape()));
    std::cout<< "----> Senales de los CHECKBOX desconectadas. " << std::endl;
}


void ModelFitting::resizeEvent(QResizeEvent *event){
   if(candide != nullptr){
       ui->img_label->setMinimumHeight(int(ui->img_label->width()*0.5625) );
       int tab = ui->tabWidget->currentIndex();
       if(tab == 0){
           actualizar_posicion();
       }
       else if(tab == 1){
           actualizar_animation();
       }
       else if(tab == 2){
           actualizar_shape();
       }
   }
}


void ModelFitting::actualizar_posicion(){
    ui->txt_rotXZ->setText("Rotar XZ: " + QString::number(candide->Rvector.at<double>(0, 0)));
    ui->txt_rotXY->setText("Rotar XY: " + QString::number(candide->Rvector.at<double>(0, 2)));
    ui->txt_rotYZ->setText("Rotar YZ: " + QString::number(candide->Rvector.at<double>(0, 1)));
    ui->txt_trasX->setText("Trasladar X: " + QString::number(candide->Tvector.at<double>(0, 0)));
    ui->txt_trasY->setText("Trasladar Y: " + QString::number(-candide->Tvector.at<double>(0, 1)));
    ui->txt_trasZ->setText("Trasladar Z: " + QString::number(candide->Tvector.at<double>(0, 2)));
    candide->rotartrasladar(ui->rotar_XZ->value()/1000.0, ui->rotar_XY->value()/1000.0, ui->rotar_YZ->value()/1000.0,
                           ui->trasladar_X->value()/100.0, ui->trasladar_Y->value()/100.0, ui->trasladar_Z->value()/100.0);
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));
}

void ModelFitting::actualizar_animation(){
    int AUV = ui->animation_list->currentIndex() - 1 * (ui->animation_list->currentIndex() > 0);
    if (ui->animation_list->currentIndex() == 0){
        ui->animation_txt->setText(QString("Seleccione una animación"));
    }
    else{
        candide->set_animation_value(AUV,ui->animation_slider->value()/100.0);
        ui->animation_txt->setText(QString::number(candide->animation_coef[AUV]*1.00));
    }
    candide->actualizar_modelo();
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    if(ui->animation_checkbox->isChecked() && ui->animation_list->currentIndex() > 0){
        imagen = candide->graficar_auv(imagen, AUV);
    }
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));
}

void ModelFitting::actualizar_animation_slider(){
    int AUV = ui->animation_list->currentIndex() - 1 * (ui->animation_list->currentIndex() > 0);
    if(ui->animation_list->currentIndex() == 0){
        ui->animation_slider->setHidden(1);
    }
    else{
        ui->animation_slider->setHidden(0);
        ui->animation_txt->setHidden(0);
        ui->animation_checkbox->setHidden(0);
        ui->animation_slider->setValue(candide->animation_coef[AUV]*100.0);
        ui->animation_txt->setText(QString::number(candide->animation_coef[AUV]*1.0));
    }
}

void ModelFitting::actualizar_shape(){
    int SHAPE = ui->shape_list->currentIndex() - 1 * (ui->shape_list->currentIndex() > 0);
    if (ui->shape_list->currentIndex() == 0){
        ui->shape_txt->setText(QString("Seleccione una forma"));
    }
    else{
        candide->set_shape_value(SHAPE,ui->shape_slider->value()/100.0);
        ui->shape_txt->setText(QString::number(candide->shape_coef[SHAPE]*1.00));
    }
    candide->actualizar_modelo();
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    if(ui->shape_checkbox->isChecked() && ui->shape_list->currentIndex() > 0){
        imagen = candide->graficar_shape(imagen, SHAPE);
    }
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));
}

void ModelFitting::actualizar_shape_slider(){
    int SHAPE = ui->shape_list->currentIndex() - 1 * (ui->shape_list->currentIndex() > 0);
    if(ui->shape_list->currentIndex() == 0){
        ui->shape_slider->setHidden(1);
    }
    else{
        ui->shape_slider->setHidden(0);
        ui->shape_txt->setHidden(0);
        ui->shape_checkbox->setHidden(0);
        ui->shape_slider->setValue(candide->shape_coef[SHAPE]*100.0);
        ui->shape_txt->setText(QString::number(ui->shape_slider->value()/100.0));
    }
}

void ModelFitting::on_animation_back_pressed(){
    if(ui->animation_list->currentIndex() > 0)
       ui->animation_list->setCurrentIndex(ui->animation_list->currentIndex() - 1 );
}

void ModelFitting::on_animation_next_pressed(){
    if(ui->animation_list->currentIndex() < int(candide->animation_parameters.size()))
       ui->animation_list->setCurrentIndex(ui->animation_list->currentIndex() + 1 );
}

void ModelFitting::on_shape_back_pressed(){
    if(ui->shape_list->currentIndex() > 0)
       ui->shape_list->setCurrentIndex(ui->shape_list->currentIndex() - 1 );
}

void ModelFitting::on_shape_next_pressed(){
    if(ui->shape_list->currentIndex() < int(candide->shape_parameters.size()))
       ui->shape_list->setCurrentIndex(ui->shape_list->currentIndex() + 1 );
}

void ModelFitting::on_boton_reset_clicked(){
    //Reinicio sliders, listas y checkbox
    ui->animation_slider->setValue(0);
    ui->shape_slider->setValue(0);
    ui->rotar_XY->setValue(0);
    ui->rotar_XZ->setValue(0);
    ui->rotar_YZ->setValue(3141);
    ui->trasladar_X->setValue(0);
    ui->trasladar_Y->setValue(0);
    ui->trasladar_Z->setValue(800);
    ui->animation_checkbox->setChecked(0);
    ui->shape_checkbox->setChecked(0);
    ui->animation_list->setCurrentIndex(0);
    ui->shape_list->setCurrentIndex(0);

    candide->resetear_modelo();

    //Actualizo posicion para actualizar los sliders y graficar
    actualizar_posicion();
}

void ModelFitting::on_tabWidget_currentChanged(int index){
    if(index == 0) actualizar_posicion();
    else if(index == 1) actualizar_animation();
    else if(index == 2) actualizar_shape();
}

void ModelFitting::on_render_model_clicked()
{
    keyframe = !keyframe;
    candide->set_keyframe_image(candide->get_keyframe());
    candide->set_keyframe_model();
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));

}

void ModelFitting::on_save_model_clicked()
{

    QString shape_dir;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Guardar Shape"),shape_dir, tr("Shape File (*.shape)"));

    if(fileName == NULL)
        return;
    std::cout << "Se guardara el archivo de Shape: " << fileName.toUtf8().constData() << std::endl;
    ofstream myfile;
    myfile.open (fileName.toStdString());
    myfile << "# POSE:" << std::endl;
    myfile << candide->Tvector.at<double>(0,0) << " " << candide->Tvector.at<double>(0,1) << " " << candide->Tvector.at<double>(0,2)<<std::endl;
    myfile << candide->Rvector.at<double>(0,0) << " " << candide->Rvector.at<double>(0,1) << " " << candide->Rvector.at<double>(0,2)<<std::endl;
    myfile << "\n# ANIMATION:" << endl;
    for(int k=0; k < int(candide->animation_parameters.size()); k++){
        myfile << candide->animation_coef[k] << std::endl;
    }
    myfile << "\n# SHAPE:" << endl;
    for(int k=0; k < int(candide->shape_parameters.size()); k++){
        myfile << candide->shape_coef[k] << std::endl;
    }
    myfile << "\n# END" << endl;
    myfile.close();
}

void ModelFitting::on_load_model_clicked()
{
    QString shape_dir;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Shape"), shape_dir, tr("Shape File (*.shape)"));
    if(fileName == NULL)
        return;
    desconectar();
    std::cout << "Se cargara el archivo de Shape: " << fileName.toUtf8().constData() << std::endl;
    std::cout << candide->Rvector << " " << candide->Tvector << std::endl;
    candide->load_person(fileName.toUtf8().constData());
    std::cout << candide->Rvector.at<double>(0,0) << " " << candide->Tvector.at<double>(0,0) << std::endl;
    ui->rotar_XZ->setValue(candide->Rvector.at<double>(0,0)*1000.0);
    ui->rotar_XY->setValue(candide->Rvector.at<double>(0,2)*1000.0);
    ui->rotar_YZ->setValue(candide->Rvector.at<double>(0,1)*1000.0);
    ui->trasladar_X->setValue(candide->Tvector.at<double>(0,0)*100.0);
    ui->trasladar_Y->setValue(-candide->Tvector.at<double>(0,1)*100.0);
    ui->trasladar_Z->setValue(candide->Tvector.at<double>(0,2)*100.0);
    conectar();
    //Actualizo posicion para actualizar los sliders y graficar
    actualizar_posicion();
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));

}

void ModelFitting::on_key_frame_ready_clicked()
{
    candide->set_keyframe_image(candide->get_keyframe());
    candide->set_keyframe_model();
    emit keyframe_ready();
}
