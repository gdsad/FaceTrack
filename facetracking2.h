#pragma once
#ifndef FACETRACKING_H
#define FACETRACKING_H

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

// OpenCV
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"

// AlgLib - LevMarq
#include "ap.h"
#include "optimization.h"

#include <iostream>


#include "opencv2/face.hpp"
#include "opencv2/video/tracking.hpp"

#include "facialmesh.h"

#define FACE_MODEL			"candide3_rodri.wfm"
#define RESIZE_F                           1
#define LANDMARKS_POSE                     1
#define LANDMARKS_ANIMATION               2
#define LANDMARKS_ALL                      3


class facetracking {

public:
    facetracking();

    // ================================ DETECCION de CARA ================================ \\

    // Obtencion de ROI donde se ubica la cara del sujeto utilizando HaarCascades
    cv::Rect detectFace(cv::Mat img, bool &face_detected);

    // ========================== UBICACION de PUNTOS de INTERES ========================= \\

    // Actualiza la posicion de los puntos del atributo landmarks utilizando Facemark de OpenCV 4.3.0
    void facelandmarks(cv::Mat img, cv::Rect face, bool& detect, int type);
    // Dibuja los landmarks de interes
    void drawlandmarks(cv::Mat img);


    // ============================= TRACKING FLUJO OPTICO =============================== \\

    // Obtiene el flujo optico de los puntos de interes entre dos imagenes y devuelve la estimacion
    // de los puntos de interes en la imagen actual
    void optflow(cv::Mat prev_img, cv::Mat img, std::vector < cv::Point2f> prev_points,
        std::vector < cv::Point2f> *points, std::vector<uchar>* status, std::vector<float>* error);


    // ESTIMACION DE PARAMETROS \\

    //Pose

    // Estimo la rotacion y translacion del modelo de la mascara utilizando los landmarks trackeados con Facemark
    void estimar_pose_landmarks(cv::Mat img);
    void estimar_pose_lk_kf(cv::Mat prev_img, cv::Mat img);

    void ajustar_pose_levmar(puntos3d* opoints, puntos2d* ipoints, cv::Mat *Rvector, cv::Mat *Tvector);

    // Estimo la pose usando algoritmo de Lucas Terissi
    void estimar_pose_lk(cv::Mat prev_img, cv::Mat img, puntos2d &puntos);
    void buscar_puntos(cv::Mat &prev_img, cv::Mat &curr_img, puntos2d &prev_points, int level,
                                    puntos2d &puntos_nuevos, vector<int> &indx_OK, cv::Size winsize = cv::Size(20,20));


    //Animacion
    void estimar_animacion_landmarks(cv::Mat img, puntos2d &puntos);
    void estimar_animacion_lk(cv::Mat prev_img, cv::Mat img, puntos2d*puntos_prev, puntos2d &puntos);
    void ajustar_animacion_levmar(puntos2d *puntos2D_target, std::vector<int>* point_indxs, facialmesh *modelo);



    // <><><><><><><><><><><><><><><> ATRIBUTOS DE LA CLASE <><><><><><><><><><><><><><><> \\

    // Objeto de la mascara
    facialmesh* candide;
    // Creo una instancia de Facemark para la deteccion de los landmarks
    cv::Ptr<cv::face::Facemark> facemark;
    // Landmarks obtenidos con Facemark
    puntos2d landmarks;
    // Parametros para el Optical Flow LK
    cv::TermCriteria termcrit;
    cv::Size winSize;
    // Creo los objetos clasificadores de cascada
    cv::CascadeClassifier HC_face;
    // Para la estimacion de pose con los landmarks
    puntos3d posevertex, animationvertex;
};




#endif // FACETRACKING
