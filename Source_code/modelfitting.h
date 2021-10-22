#ifndef MODELFITTING_H
#define MODELFITTING_H

#include <QWidget>
#include "facialmesh.h"
namespace Ui {
class ModelFitting;
}

class ModelFitting : public QWidget
{
    Q_OBJECT

public:
    explicit ModelFitting(QWidget *parent = nullptr);
    ~ModelFitting();

    void init(void);
    void conectar(void);
    void desconectar(void);
    facialmesh *candide;
    cv::Mat kf_image;
    bool keyframe = false;

public slots:
    void actualizar_posicion();
    void actualizar_animation();
    void actualizar_animation_slider();
    void actualizar_shape();
    void actualizar_shape_slider();

private slots:
    void on_boton_reset_clicked();
    void on_animation_back_pressed();
    void on_animation_next_pressed();
    void on_shape_back_pressed();
    void on_shape_next_pressed();
    void on_tabWidget_currentChanged(int index);
    void on_render_model_clicked();
    void on_save_model_clicked();
    void on_load_model_clicked();
    void on_key_frame_ready_clicked();
    void resizeEvent(QResizeEvent *event);

signals:
    void keyframe_ready();

private:
    Ui::ModelFitting *ui;
};

#endif // MODELFITTING_H
