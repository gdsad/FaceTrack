#pragma once
#ifndef FACIALMESH_H
#define FACIALMESH_H

// Libreria de Windows util para que funcione correctamente en QT5
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <iostream>		                    // operaciones basicas I/O
#include <fstream>		                    // manejo de archiovs en c++
#include <algorithm>	                    //funciones utiles para manejar datos
#include <opencv2/highgui/highgui.hpp>		// creacion, lectura, escritura y de imagenes, creacion y uso de ventanas, entrada de teclado.
#include <opencv2/core/core.hpp>			// estrucuta de datos, soporte de alg. lineal, etc.
#include <opencv2/imgproc/imgproc.hpp>		// procesamiento digital de imagenes
#include <opencv2/calib3d/calib3d.hpp>		// vision multiple, como camaras estereo, correspondencia o reconstruccion 3D

#define GET_VARIABLE_NAME(variable)       (#variable)
#define RESIZE_FACTOR                       3
//                                          Scalar((b), (g), (r), 0)
#define COLOR_ROJO                          cv::Scalar((0), (0), (255), 0)
#define COLOR_AZUL                          cv::Scalar((255), (0), (0), 0)
#define COLOR_VERDE                         cv::Scalar((0), (255), (0), 0)
#define COLOR_BLANCO                        cv::Scalar((255), (255), (255), 0)
#define COLOR_NEGRO                         cv::Scalar((0), (0), (0), 0)
#define This                                (*this)
#define RAD2DEG(x)                          ((x)*180.0/M_PI)
#define DEG2RAD(x)                          ((x)*M_PI/180.0)
#define ONEDEG                              DEG2RAD(1.0)

#define CLASE_NULL                          0
#define CLASE_ANIMATION                     1
#define CLASE_ANIMATION_CENTRAL             2

// No utilizo using namespace cv, ya que genera un problema con QT5
using namespace std;

typedef cv::Point3f punto3d; // Punto en el espacio 3D
typedef vector<punto3d> puntos3d; // Vector de puntos en el espacio 3D
typedef cv::Point2f punto2d; // Punto en el plano 2D
typedef vector<punto2d> puntos2d; // Vector de puntos en el espacio 3D
typedef vector<float> values; // Vector de float que contiene los valores de activacion de cada Unit


/*Estructura que permite cargar un renglon de la matriz de una Unit
[ INDICE | coef.X | coef.Y | coef.Z ] */
typedef struct renglon_matriz {
    int i;
    punto3d coef_xyz;
    inline renglon_matriz()
        : i(0)
        , coef_xyz(0.0f, 0.0f, 0.0f) {};
    inline renglon_matriz(int _i, float _x, float _y, float _z)
        : i(_i)
        , coef_xyz(_x, _y, _z) {};
} renglon_matriz;


/* Estructura que permite cargar una Unit de la mascara
   name -> Nombre de la Unit
   matriz -> Matriz con vertices y coeficientes */
typedef struct unit {
    char* name;
    vector<renglon_matriz> matriz;
    inline unit() : name(NULL) {};
} unit;


typedef vector<unit> units; // Aqui se almacenaran todos los Animation Units o los Shape Units


/* Permite almacenar una terna con los indices de los vertices que forman
   un triangulo de la mesh de la cara */
typedef struct face {
    int a;
    int b;
    int c;
    inline face()
        : a(0)
        , b(0)
        , c(0) {};
    inline face(int _a, int _b, int _c)
        : a(_a)
        , b(_b)
        , c(_c) {};
} face;
typedef vector<face> faces;



class facialmesh {
public:

    // <><><><><><><><><><><><><><><> METODOS DE LA CLASE <><><><><><><><><><><><><><><>

    // Constructor
    facialmesh(string filename);


    // ========================== HERRAMIENTAS PARA LA CARGA DEL MODELO ========================== \\

    void load_mesh(string filename);
    //Cargo los parametros de la mascara para una persona
    void load_person(string shapefile);
    // Transforma un string a un float
    float str2float(string s);
    // Divide una string segun el 'delim' que se le pase como entrada y devuelve un vector con las strings obtenidas de la division
    void split_line(const string& s, char delim, vector<string>& elems);
    void leer_parametros(vector<vector<float> >* param, ifstream& file);


    // =================================== ACTUALIZA MODELO ====================================== \\

    // Actualiza la proyeccion 2D del modelo
    void actualizar_2D();
    // Actualiza la influencia de los Shape para cada vertice
    void actualizar_shape();
    // Actualiza la influencia de los Animation cada vertice
    void actualizar_animation();
    // Actualiza los vertices 3D teniendo en cuenta la influencia de los Shape y los Animation
    void actualizar_vertices();
    // Actualiza los shape, los animation y la proyeccion 2D
    void actualizar_modelo();


    // =================================== RESETEAR MODELO ====================================== \\

    // Resetea todos los coeficientes de animacion
    void resetear_animation();
    // Resetea todos los coeficientes de animacion
    void resetear_shape();
    // Resetea rotacion y traslacion del modelo
    void resetear_pose();
    // Resetea todos los coeficientes de animacion y shape
    void resetear_modelo();


    // ============================= ROTACION Y TRASLACION DE CAMARA ============================ \\

    // Permite rotar la posicion de la camara
    void rotar(double ang_xz, double ang_xy, double ang_yz);
    // Permite traladar la posicion de la camara
    void trasladar(double d_x, double d_y, double d_z);
    /* Permite la rotacion y traslacion de la ubicacion de la camara, lo que logra
       el mismo efecto en la proyeccion 2D de la mascara. Al finalizar la transformacion
       actualiza la proyeccion 2D. */
    void rotartrasladar(double ang_xz, double ang_xy, double ang_yz, double d_x, double d_y, double d_z);


    // ============================== DEFORMACION DE LA MASCARA ================================= \\

    /* Setea un valor de activacion 'value' de un shape especifico 'SHAPE'.
       Se utiliza += ya que si varios animation comparten un vertice, la posicion
       final del vertice debera ser la combinacion de todos los AUV q lo afecten  */
    void set_shape_value(int SHAPE, float value);
    // Setea un valor de activacion 'value' de un animation especifico 'AUV'
    void set_animation_value(int AUV, float value);


    // ===================== GENERACION DE MESH FACIAL Y PUNTOS DE INTERES ====================== \\

    // Grafica la mesh del modelo sobre una imagen pasada como entrada
    cv::Mat graficar_mesh(cv::Mat img);
    // Grafica los vertices que pertenecen a un 'AUV' de color rojo sobre una imagen pasada como entrada
    cv::Mat graficar_auv(cv::Mat img, int AUV);
    // Grafica los vertices que pertenecen a una 'SHAPE' de color verde sobre una imagen pasada como entrada
    cv::Mat graficar_shape(cv::Mat img, int SHAPE);
    // Grafica el vertice pasado como entrada del color que se le pasa como entrada
    cv::Mat graficar_vertice(cv::Mat img, int vertice, cv::Scalar COLOR);
    // Grafica los puntos utilizados por Lucas para la estimacion de Pose
    cv::Mat graficar_pose_tracking_points(cv::Mat img);


    // ====================================== KEYFRAME ========================================= \\

    // Permite cargar el keyframe y lo convierte a escala de grises
    void set_keyframe_image(cv::Mat keyframe);
    // Permite obtener el keyframe
    cv::Mat get_keyframe(void);
    // Permite guardar la posicion actual de los vertices de la mascara en el KeyFrame
    void set_keyframe_model(void);
    // Renderiza el KeyFrame para el cuadro actual teniendo en cuenta la rotacion de la cara
    // Admite que el initial_keyframe sea una imagen a color o en escala de grises
    // Aunque si se utiliza set_keyframe_image, el keyframe sera convertido a escala de grises.
    cv::Mat render_keyframe(void);
    // Renderiza el keyframe sobre la imagen pasada como entrada. Pinta el interior de la boca de negro
    cv::Mat render_keyframe_v2(cv::Mat img);
    // Renderiza el keyframe sobre la imagen pasada como entrada.
    cv::Mat render_keyframe_v3(cv::Mat img);


    // <><><><><><><><><><><><><><><> ATRIBUTOS DE LA CLASE <><><><><><><><><><><><><><><> \\

    puntos2d *vertices2D, * vertices2D_keyframe, *vertices2D_noanimation;
    puntos3d *vertices3D, * vertices3D_keyframe, * vertices3D_original, *vertices3D_noanimation, *animation3D, *shape3D;
    vector<int> *pose_tracking_indxs, *candide_lankmarks_indxs;
    vector<int> vertex_class;

    faces faces;

    // Matrices de Animation y Shape del modelo
    units animation_parameters, shape_parameters;

    // Coeficientes de deformacion del modelo
    values animation_coef, shape_coef, animation_coef_k;

    // Parametros de la camara
    cv::Mat camera_matrix, distCoeffs;
    cv::Mat Rvector, Tvector;

    // KeyFrame
    cv::Mat initial_keyframe;

};

// =================== Para mover en un futuro a un archivo de toolbox =================== \\

cv::Mat GetSquareImage(const cv::Mat& img, int target_width);
cv::Mat GetFittedImage(const cv::Mat& img, int target_height,int target_width);
void grayscale(cv::Mat &source, cv::Mat &destination);

#endif //FACIALMESH
