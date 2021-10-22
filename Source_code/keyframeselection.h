#ifndef KEYFRAMESELECTION_H
#define KEYFRAMESELECTION_H
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/videoio.hpp>
#include "facialmesh.h"

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class KeyFrameSelection;
}

class KeyFrameSelection : public QWidget
{
    Q_OBJECT

public:
    explicit KeyFrameSelection(QWidget *parent = nullptr);
    ~KeyFrameSelection();
    cv::Mat frame;
    cv::VideoCapture cap;
    std::string filename;

public slots:
    void actualizar_frame();

private slots:
    void on_open_video_clicked();
    void on_convertir_clicked();
    void on_select_keyframe_clicked();
    void resizeEvent(QResizeEvent *event);

signals:
    void keyframe_selected();

private:
    Ui::KeyFrameSelection *ui;
};

#endif // KEYFRAMESELECTION_H
