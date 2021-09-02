#pragma once
#ifndef POINTSTATE_H
#define POINTSTATE_H

// Libreria de Windows util para que funcione correctamente en QT5
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>

class PointState
{
public:
    PointState(cv::Point2f point)
        :
        m_point(point),
        m_kalman(4, 2, 0, CV_64F)
    {
        Init();
    }

    void Update(cv::Point2f point);
    cv::Point2f GetPoint();
    bool IsPredicted();


private:
    cv::Point2f m_point;
    cv::KalmanFilter m_kalman;

    // Parametros del filto de Kalman aplicado a los Facemarks
    double m_deltaTime = 0.4; // 0.2; // Parametro -> Latency --> Probar con 1/cap.FPS?
    double m_accelNoiseMag = 0.05;// 0.3; //Parametro -> Noise

    bool m_isPredicted = false;

    void Init();
    cv::Point2f Predict();
};

#endif
