#ifndef ONLYMESH_H
#define ONLYMESH_H

#include <QWidget>
#include "facialmesh.h"
namespace Ui {
class OnlyMesh;
}

class OnlyMesh : public QWidget
{
    Q_OBJECT

public:
    explicit OnlyMesh(QWidget *parent = nullptr);
    ~OnlyMesh();

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
    void on_boton_reset_2_clicked();
    void on_animation_back_2_pressed();
    void on_animation_next_2_pressed();
    void on_shape_back_2_pressed();
    void on_shape_next_2_pressed();
    void on_tabWidget_2_currentChanged(int index);
    void on_render_model_2_clicked();
    void on_save_model_2_clicked();
    void on_load_model_2_clicked();
    void on_key_frame_ready_2_clicked();
    void resizeEvent(QResizeEvent *event);

signals:
    void go2init();

private:
    Ui::OnlyMesh *ui;
};

#endif // ONLYMESH_H
