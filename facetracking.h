#pragma once
#ifndef FACETRACKING_H
#define FACETRACKING_H

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

// OpenCV 4.3.0
#include "opencv2/opencv.hpp"               // todo opencv
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/face.hpp"
#include "opencv2/video/tracking.hpp"

// AlgLib 3.16 (p/ LevMarq)
#include "ap.h"
#include "optimization.h"

// C++
#include <iostream>
#include <exception>
#include <vector>
#include <string>

// Propias
#include "facialmesh.h"
#include "PointState.h"



#define RESIZE_F                           1
#define LANDMARKS_ALL                      0
#define LANDMARKS_POSE                     1
#define LANDMARKS_ANIMATION                2
#define LANDMARKS_CANDIDE				   3
#define LANDMARKS_BOCA					   4
#define LANDMARKS_CEJAS					   5


class facetracking {

public:
    facetracking();

    // ================================ DETECCION de CARA ================================ \\

    // Obtencion de ROI donde se ubica la cara del sujeto utilizando HaarCascades
    cv::Rect detectFace(cv::Mat img, bool &face_detected);

    // ========================== UBICACION de PUNTOS de INTERES ========================= \\

    // Actualiza la posicion de los puntos del atributo landmarks utilizando Facemark de OpenCV 4.3.0
    void facelandmarks(cv::Mat img, cv::Rect face, bool& detect);
    // Dibuja los landmarks de interes
    cv::Mat drawlandmarks(cv::Mat img);


    // =============================== TRACKING DE PUNTOS =============================== \\

    void buscar_puntos(cv::Mat& prev_img, cv::Mat& curr_img, puntos2d& prev_points, int level,
        puntos2d& puntos_nuevos, vector<int>& indx_OK, cv::Size winsize = cv::Size(20, 20));
    void buscar_puntosV2(cv::Mat prev_img, cv::Mat curr_img, puntos2d prev_points, int level,
        puntos2d* puntos_nuevos, cv::Size winsize, values* delta);


    // ============================= ESTIMACION DE PARAMETROS ============================= \\

    void estimar_pose(cv::Mat prev_img, cv::Mat img, puntos2d puntos_pose, puntos2d* puntos, float* error);
    void estimar_animacion(cv::Mat prev_img, cv::Mat img, puntos2d puntos_prev, values coef_prev, values* anim_coefs, values* au_error);
    void ajustar_animacion_levmar(puntos2d *puntos2D_target, std::vector<int>* point_indxs, facialmesh *modelo);


    // ======================== ALGORITMO DE TRACKING Y ESTIMACION ========================= \\

    void step(cv::Mat prev_img, cv::Mat img);

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
    // LandMarks Estabilizados con Kalman
    std::vector<PointState> stabilized_landmarks;

    std::vector<std::string> text_on_image;
    values delta_raw_landmarks, delta_stb_landmarks, delta_raw_stb;
};

#endif // FACETRACKING
