#include "onlymesh.h"
#include "ui_onlymesh.h"
#include "MatToQImage.h"
#include <iostream>
#include <fstream>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>

#include "facialmesh.h"

OnlyMesh::OnlyMesh(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlyMesh)
{
    ui->setupUi(this);
    std::cout << "Inicializando OnlyMesh...." << std::endl;
    conectar();
    std::cout << "OnlyMesh Inicializado." << std::endl;


}

OnlyMesh::~OnlyMesh()
{
    delete ui;
}

void OnlyMesh::init(void){
    // Creo la lista de opciones para las Animation Units
    ui->animation_list_2->addItem("Seleccionar...");
    for(int k=0; k < int(candide->animation_parameters.size()); k++){
        ui->animation_list_2->addItem(QString::number(k) + QString(". ") + candide->animation_parameters[k].name);
    }
    std::cout<< "----> Valores cargados en la lista de ANIMATION. " << std::endl;

    // Creo la lista de opciones para las Shape Units
    ui->shape_list_2->addItem("Seleccionar...");
    for(int k=0; k < int(candide->shape_parameters.size()); k++){
        ui->shape_list_2->addItem(QString::number(k) + QString(". ") + candide->shape_parameters[k].name);
    }
    std::cout<< "----> Valores cargados en la lista de SHAPE. " << std::endl;

    // Seteo el indice de los tabs para que arranque en el primer Tab
    ui->tabWidget_2->setCurrentIndex(0);
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
    ui->tabWidget_2->setGraphicsEffect(shadow_tab);

    // Size
    QSizePolicy img_size;
    img_size.setHorizontalPolicy(QSizePolicy::Minimum);
    img_size.setVerticalPolicy(QSizePolicy::Minimum);
    ui->img_label->setSizePolicy(img_size);

}

void OnlyMesh::conectar(){
    // Conexiones para actualizar la pose del modelo, cuando se cambie el
    // valor de los sliders se ejecutara la funcion actualizar_posicion()
    connect(ui->rotar_XZ_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->rotar_XY_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->rotar_YZ_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->trasladar_X_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->trasladar_Y_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    connect(ui->trasladar_Z_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    std::cout<< "----> Senales para control de POSE seteadas. " << std::endl;

    // Conexiones para actualizar la Animation del modelo
    connect(ui->animation_list_2,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_animation_slider()));
    connect(ui->animation_list_2,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_animation()));
    connect(ui->animation_slider_2,SIGNAL(valueChanged(int)), this, SLOT(actualizar_animation()));
    std::cout<< "----> Senales para control de ANIMATION seteadas. " << std::endl;

    // Conexiones para actualizar la Shape del modelo
    connect(ui->shape_list_2,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_shape_slider()));
    connect(ui->shape_list_2,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_shape()));
    connect(ui->shape_slider_2,SIGNAL(valueChanged(int)), this, SLOT(actualizar_shape()));
    std::cout<< "----> Senales para control de SHAPE seteadas. " << std::endl;

    // Actualizo los sliders cuando cambio de tab
    connect(ui->boton_reset_2, SIGNAL(clicked()),this, SLOT(actualizar_shape_slider()));
    connect(ui->boton_reset_2, SIGNAL(clicked()),this, SLOT(actualizar_animation_slider()));
    std::cout<< "----> Senales para actualizacion de sliders." << std::endl;

    // Grafico los vertices si se marca la check box
    connect(ui->animation_checkbox_2,SIGNAL(toggled(bool)),this,SLOT(actualizar_animation()));
    connect(ui->shape_checkbox_2,SIGNAL(toggled(bool)),this,SLOT(actualizar_shape()));
    std::cout<< "----> Senales de los CHECKBOX seteadas. " << std::endl;
}

void OnlyMesh::desconectar(){
    // Conexiones para actualizar la pose del modelo, cuando se cambie el
    // valor de los sliders se ejecutara la funcion actualizar_posicion()
    disconnect(ui->rotar_XY_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->rotar_YZ_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->rotar_XZ_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->trasladar_X_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->trasladar_Y_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    disconnect(ui->trasladar_Z_2, SIGNAL(valueChanged(int)), this, SLOT(actualizar_posicion()));
    std::cout<< "----> Senales para control de POSE desconectadas. " << std::endl;

    // Conexiones para actualizar la Animation del modelo
    disconnect(ui->animation_list_2,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_animation_slider()));
    disconnect(ui->animation_list_2,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_animation()));
    disconnect(ui->animation_slider_2,SIGNAL(valueChanged(int)), this, SLOT(actualizar_animation()));
    std::cout<< "----> Senales para control de ANIMATION desconectadas. " << std::endl;

    // Conexiones para actualizar la Shape del modelo
    disconnect(ui->shape_list_2,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_shape_slider()));
    disconnect(ui->shape_list_2,SIGNAL(currentIndexChanged(int)), this, SLOT(actualizar_shape()));
    disconnect(ui->shape_slider_2,SIGNAL(valueChanged(int)), this, SLOT(actualizar_shape()));
    std::cout<< "----> Senales para control de SHAPE desconectadas. " << std::endl;

    // Actualizo los sliders cuando cambio de tab
    disconnect(ui->boton_reset_2, SIGNAL(clicked()),this, SLOT(actualizar_shape_slider()));
    disconnect(ui->boton_reset_2, SIGNAL(clicked()),this, SLOT(actualizar_animation_slider()));
    std::cout<< "----> Senales para actualizacion de sliders desconectadas." << std::endl;

    // Grafico los vertices si se marca la check box
    disconnect(ui->animation_checkbox_2,SIGNAL(toggled(bool)),this,SLOT(actualizar_animation()));
    disconnect(ui->shape_checkbox_2,SIGNAL(toggled(bool)),this,SLOT(actualizar_shape()));
    std::cout<< "----> Senales de los CHECKBOX desconectadas. " << std::endl;
}

void OnlyMesh::resizeEvent(QResizeEvent *event){
   if(candide != nullptr){
       ui->img_label->setMinimumHeight(int(ui->img_label->width()*0.5625) );
       int tab = ui->tabWidget_2->currentIndex();
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

void OnlyMesh::actualizar_posicion(){
    ui->txt_rotXZ_2->setText("Rotar XZ: " + QString::number(candide->Rvector.at<double>(0, 0)));
    ui->txt_rotXY_2->setText("Rotar XY: " + QString::number(candide->Rvector.at<double>(0, 2)));
    ui->txt_rotYZ_2->setText("Rotar YZ: " + QString::number(candide->Rvector.at<double>(0, 1)));
    ui->txt_trasX_2->setText("Trasladar X: " + QString::number(candide->Tvector.at<double>(0, 0)));
    ui->txt_trasY_2->setText("Trasladar Y: " + QString::number(-candide->Tvector.at<double>(0, 1)));
    ui->txt_trasZ_2->setText("Trasladar Z: " + QString::number(candide->Tvector.at<double>(0, 2)));
    candide->rotartrasladar(ui->rotar_XZ_2->value()/1000.0, ui->rotar_XY_2->value()/1000.0, ui->rotar_YZ_2->value()/1000.0,
                           ui->trasladar_X_2->value()/100.0, ui->trasladar_Y_2->value()/100.0, ui->trasladar_Z_2->value()/100.0);
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));
}

void OnlyMesh::actualizar_animation(){
    int AUV = ui->animation_list_2->currentIndex() - 1 * (ui->animation_list_2->currentIndex() > 0);
    if (ui->animation_list_2->currentIndex() == 0){
        ui->animation_txt_2->setText(QString("Seleccione una animación"));
    }
    else{
        candide->set_animation_value(AUV,ui->animation_slider_2->value()/100.0);
        ui->animation_txt_2->setText(QString::number(candide->animation_coef[AUV]*1.00));
    }
    candide->actualizar_modelo();
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    if(ui->animation_checkbox_2->isChecked() && ui->animation_list_2->currentIndex() > 0){
        imagen = candide->graficar_auv(imagen, AUV);
    }
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));
}

void OnlyMesh::actualizar_animation_slider(){
    int AUV = ui->animation_list_2->currentIndex() - 1 * (ui->animation_list_2->currentIndex() > 0);
    if(ui->animation_list_2->currentIndex() == 0){
        ui->animation_slider_2->setHidden(1);
    }
    else{
        ui->animation_slider_2->setHidden(0);
        ui->animation_txt_2->setHidden(0);
        ui->animation_checkbox_2->setHidden(0);
        ui->animation_slider_2->setValue(candide->animation_coef[AUV]*100.0);
        ui->animation_txt_2->setText(QString::number(candide->animation_coef[AUV]*1.0));
    }
}

void OnlyMesh::actualizar_shape(){
    int SHAPE = ui->shape_list_2->currentIndex() - 1 * (ui->shape_list_2->currentIndex() > 0);
    if (ui->shape_list_2->currentIndex() == 0){
        ui->shape_txt_2->setText(QString("Seleccione una forma"));
    }
    else{
        candide->set_shape_value(SHAPE,ui->shape_slider_2->value()/100.0);
        ui->shape_txt_2->setText(QString::number(candide->shape_coef[SHAPE]*1.00));
    }
    candide->actualizar_modelo();
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    if(ui->shape_checkbox_2->isChecked() && ui->shape_list_2->currentIndex() > 0){
        imagen = candide->graficar_shape(imagen, SHAPE);
    }
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));
}

void OnlyMesh::actualizar_shape_slider(){
    int SHAPE = ui->shape_list_2->currentIndex() - 1 * (ui->shape_list_2->currentIndex() > 0);
    if(ui->shape_list_2->currentIndex() == 0){
        ui->shape_slider_2->setHidden(1);
    }
    else{
        ui->shape_slider_2->setHidden(0);
        ui->shape_txt_2->setHidden(0);
        ui->shape_checkbox_2->setHidden(0);
        ui->shape_slider_2->setValue(candide->shape_coef[SHAPE]*100.0);
        ui->shape_txt_2->setText(QString::number(ui->shape_slider_2->value()/100.0));
    }
}

void OnlyMesh::on_animation_back_2_pressed(){
    if(ui->animation_list_2->currentIndex() > 0)
       ui->animation_list_2->setCurrentIndex(ui->animation_list_2->currentIndex() - 1 );
}

void OnlyMesh::on_animation_next_2_pressed(){
    if(ui->animation_list_2->currentIndex() < int(candide->animation_parameters.size()))
       ui->animation_list_2->setCurrentIndex(ui->animation_list_2->currentIndex() + 1 );
}

void OnlyMesh::on_shape_back_2_pressed(){
    if(ui->shape_list_2->currentIndex() > 0)
       ui->shape_list_2->setCurrentIndex(ui->shape_list_2->currentIndex() - 1 );
}

void OnlyMesh::on_shape_next_2_pressed(){
    if(ui->shape_list_2->currentIndex() < int(candide->shape_parameters.size()))
       ui->shape_list_2->setCurrentIndex(ui->shape_list_2->currentIndex() + 1 );
}

void OnlyMesh::on_boton_reset_2_clicked(){
    //Reinicio sliders, listas y checkbox
    ui->animation_slider_2->setValue(0);
    ui->shape_slider_2->setValue(0);
    ui->rotar_XY_2->setValue(0);
    ui->rotar_XZ_2->setValue(0);
    ui->rotar_YZ_2->setValue(3141);
    ui->trasladar_X_2->setValue(0);
    ui->trasladar_Y_2->setValue(0);
    ui->trasladar_Z_2->setValue(800);
    ui->animation_checkbox_2->setChecked(0);
    ui->shape_checkbox_2->setChecked(0);
    ui->animation_list_2->setCurrentIndex(0);
    ui->shape_list_2->setCurrentIndex(0);

    candide->resetear_modelo();

    //Actualizo posicion para actualizar los sliders y graficar
    actualizar_posicion();
}

void OnlyMesh::on_tabWidget_2_currentChanged(int index){
    if(index == 0) actualizar_posicion();
    else if(index == 1) actualizar_animation();
    else if(index == 2) actualizar_shape();
}

void OnlyMesh::on_render_model_2_clicked()
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

void OnlyMesh::on_save_model_2_clicked()
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

void OnlyMesh::on_load_model_2_clicked()
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
    ui->rotar_XZ_2->setValue(candide->Rvector.at<double>(0,0)*1000.0);
    ui->rotar_XY_2->setValue(candide->Rvector.at<double>(0,2)*1000.0);
    ui->rotar_YZ_2->setValue(candide->Rvector.at<double>(0,1)*1000.0);
    ui->trasladar_X_2->setValue(candide->Tvector.at<double>(0,0)*100.0);
    ui->trasladar_Y_2->setValue(-candide->Tvector.at<double>(0,1)*100.0);
    ui->trasladar_Z_2->setValue(candide->Tvector.at<double>(0,2)*100.0);
    conectar();
    //Actualizo posicion para actualizar los sliders y graficar
    actualizar_posicion();
    cv::Mat imagen;
    if(keyframe) imagen = candide->render_keyframe();
    else imagen = candide->graficar_mesh(kf_image);
    QImage imgQ = MatToQImage(imagen);
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ).scaled(ui->img_label->width(),ui->img_label->width()*0.5625));

}

void OnlyMesh::on_key_frame_ready_2_clicked()
{
    emit go2init();
}
