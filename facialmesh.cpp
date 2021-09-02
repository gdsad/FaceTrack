#include "facialmesh.h"
#define PI                  3.1415926

facialmesh::facialmesh(string filename) {
    std::cout << "Inicializando Candide..." << std::endl;
    load_mesh(filename);
    std::cout << "----> Modelo Cargado" << std::endl;
    // Seteo todos los parametros para utilizar la funcion cv::projectPoints (https://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html)
    // Extra: https://docs.opencv.org/2.4/_images/pinhole_camera_model.png
    float focal_length = 750.0; //1000
    // El centro en X e Y luego debera ser la nariz del sujeto obtenida utilizando LandMarks
    float centro_x = (320 * RESIZE_FACTOR) / 2;
    float centro_y = (240 * RESIZE_FACTOR) / 2;
    //centro_x = 980 /2;
    //centro_y = 760 /2;
    centro_x = 1000 / 2;
    centro_y = 563/2;
    camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, centro_x, 0, focal_length, centro_y, 0, 0, 1);
    Rvector = cv::Mat::zeros(1, 3, CV_64F);
    Tvector = cv::Mat::zeros(1, 3, CV_64F);
    distCoeffs = cv::Mat::zeros(8, 1, CV_32F);
    Tvector.at<double>(0, 2) = 8.5; //10
    Rvector.at<double>(0, 1) = PI;

    // Proyecta puntos 2D
    actualizar_2D();
    std::cout << "Candide Inicializado." << std::endl;
}



void facialmesh::load_mesh(string filename)
{
    ifstream file; //ifstream = input file stream
    string line;
    vector<string> linedata_tmp;

    file.open(filename);
    if (!file.is_open()) {
        std::printf("\nNo se puedo abrir el archivo");
        return;
    }
    std::cout << "----> Se abrio el archivo " << filename << std::endl;
    std::getline(file, line);

    while (!(string::npos != line.find("# END")))
    {
        /* ==============================================================================================
                                                      VERTICES
           ============================================================================================== */

        if (string::npos != line.find("# VERTEX LIST:")) {
            //std::printf("\n%s", line.c_str());
            // Lee la linea siguiente con la cantidad de vertices del modelo
            std::getline(file, line);
            int cant_vertices = atoi(line.c_str());

            //Estas variables almacenan el estado actual de los vertices 3D y 2D
            vertices2D = new puntos2d(cant_vertices);
            vertices3D = new puntos3d(cant_vertices);

            // Ubicacion de los vertices y su proyeccion en el Keyframe
            vertices2D_keyframe = new puntos2d(cant_vertices);
            vertices3D_keyframe = new puntos3d(cant_vertices);

            // Utilizados para deformar la mascara cambiando los parametros  de animation y shape
            vertices3D_original = new puntos3d(cant_vertices);
            animation3D = new puntos3d(cant_vertices);
            shape3D = new puntos3d(cant_vertices);

            //
            vertices3D_noanimation = new puntos3d(cant_vertices);
            vertices2D_noanimation = new puntos2d(cant_vertices);

            // Vector para clasificar todos los vertices
            vertex_class.resize(cant_vertices);

            // Leo todos los vertices
            for (int indx = 0; indx < cant_vertices; indx++)
            {
                /*  =========================== TEORIA C++ ========================= =
                    El operador de punto se aplica al objeto real. El operador de
                    flecha se usa con un puntero a un objeto
                    https://www.tutorialspoint.com/cplusplus/cpp_member_operators.htm
                    ================================================================ = */
                std::getline(file, line);
                linedata_tmp.clear();
                split_line(line, ' ', linedata_tmp);

                // La posicion de estos vertices se modificaran segun los niveles de
                // activacion de todas las Units del modelo
                vertices3D->at(indx).x = str2float(linedata_tmp.at(0)) * RESIZE_FACTOR;
                vertices3D->at(indx).y = -str2float(linedata_tmp.at(1)) * RESIZE_FACTOR;
                vertices3D->at(indx).z = str2float(linedata_tmp.at(2)) * RESIZE_FACTOR;

                // La ubicacion de estos vertices son fijas en el tiempo
                vertices3D_original->at(indx).x = str2float(linedata_tmp.at(0)) * RESIZE_FACTOR;
                vertices3D_original->at(indx).y = -str2float(linedata_tmp.at(1)) * RESIZE_FACTOR;
                vertices3D_original->at(indx).z = str2float(linedata_tmp.at(2)) * RESIZE_FACTOR;

                // Inicializo el vector con la clase de todos los vertices
                vertex_class.push_back(CLASE_NULL);
            }
            std::cout << "----> Vertices Cargados" << std::endl;
        }


        /* ==============================================================================================
                                                     FACES
           ============================================================================================== */

        if (string::npos != line.find("# FACE LIST:"))
        {
            //std::printf("\n%s", line.c_str());
            // Lee la l�nea siguiente con la cantidad de triangulos (faces) del modelo
            std::getline(file, line);
            int cant_faces = atoi(line.c_str());
            //std::printf("%d", cant_faces);
            faces.resize(cant_faces);
            // Lee todas las caras
            for (int indx = 0; indx < cant_faces; indx++)
            {
                getline(file, line);
                linedata_tmp.clear();
                split_line(line, ' ', linedata_tmp);
                faces[indx].a = str2float(linedata_tmp.at(0)); // Vertice a del triangulo
                faces[indx].b = str2float(linedata_tmp.at(1)); // Vertice b del triangulo
                faces[indx].c = str2float(linedata_tmp.at(2)); // Vertice c del triangulo
            }
            std::cout << "----> Faces Cargadas" << std::endl;
        }


        /* ==============================================================================================
                                                  POSE ESTIMATION
           ============================================================================================== */

          /* if (string::npos != line.find("# POSE ESTIMATION VERTEX LIST"))
           {
               //std::printf("\n%s", line.c_str());
               getline(file, line);
               linedata_tmp.clear();
               split_line(line, ' ', linedata_tmp);
               pose_tracking_indxs->resize(linedata_tmp.size());
               for (int k = 0; k < int(linedata_tmp.size()); k++)
               {
                   pose_tracking_indxs->at(k) = atoi(linedata_tmp.at(k).c_str());
               }
               //std::printf("\nSe leyeron todos los pose estimation");
           } */



           /* ==============================================================================================
                                                      ANIMATION UNITS
              ============================================================================================== */

        if (string::npos != line.find("# ANIMATION UNITS LIST:")) {
            //std::printf("\n%s", line.c_str());

            // Lee la l�nea siguiente con la cantidad de Animation Units disponibles
            std::getline(file, line);
            int cant_anim_units = atoi(line.c_str());
            //std::printf("%d", cant_anim_units);

            animation_parameters.reserve((cant_anim_units) * 2);
            animation_coef.reserve((cant_anim_units) * 2);
            animation_coef_k.reserve((cant_anim_units) * 2);
            // Leo todas las AUV disponibles segun lo indicado con cant_anim_units
            for (int k = 0; k < cant_anim_units; k++) {

                // Busco la proxima linea que sea el comienzo de  un AUV
                while (string::npos == line.find("# ")) std::getline(file, line);

                if (string::npos != line.find("# ")) {
                    // Para los units que no deben ser asimetricos
                    if (string::npos != line.find("Jaw")) {

                        unit simetrical_unit;
                        line = line.substr(2); //Le quito los dos primeros elementos al nombre (# )
                        simetrical_unit.name = _strdup(line.c_str());
                        std::getline(file, line);
                        int cant_de_renglones = atoi(line.c_str());
                        simetrical_unit.matriz.reserve(cant_de_renglones);

                        //std::printf("\n\n ============ AUV: %s // RENGLONES: %d ==========", simetrical_unit.name, cant_de_renglones);

                        for (int j = 0; j < cant_de_renglones; j++)
                        {
                            getline(file, line);
                            //std::printf("\n ---  %s", line.c_str());
                            linedata_tmp.clear();
                            split_line(line, ' ', linedata_tmp);
                            simetrical_unit.matriz.push_back(renglon_matriz(str2float(linedata_tmp.at(0)),
                                str2float(linedata_tmp.at(1)),
                                str2float(linedata_tmp.at(2)),
                                str2float(linedata_tmp.at(3))));
                            vertex_class[str2float(linedata_tmp.at(0))] = CLASE_ANIMATION;
                        }
                        animation_parameters.push_back(simetrical_unit);
                        animation_coef.push_back(0.0);
                        animation_coef_k.push_back(0.0);

                        simetrical_unit.matriz.clear();


                    }
                    // Para los units que deben ser asimetricos
                    else {
                        string auv_name = line;
                        auv_name = auv_name.substr(2);
                        getline(file, line);
                        int cant_de_renglones = atoi(line.c_str());
                        unit izquierda, derecha;
                        string aux = auv_name + " R";
                        izquierda.name = _strdup(aux.c_str());
                        aux = auv_name + " L";
                        derecha.name = _strdup(aux.c_str());
                        aux.clear();

                        izquierda.matriz.reserve(cant_de_renglones);
                        derecha.matriz.reserve(cant_de_renglones);

                        //std::printf("\n\n ============ AUV: %s // RENGLONES: %d ==========", auv_name.c_str(), cant_de_renglones);

                        for (int j = 0; j < cant_de_renglones; j++) {
                            getline(file, line);
                            //std::printf("\n ---  %s", line.c_str());
                            linedata_tmp.clear();
                            split_line(line, ' ', linedata_tmp);
                            vertex_class[str2float(linedata_tmp.at(0))] = CLASE_ANIMATION;

                            if (vertices3D->at(str2float(linedata_tmp.at(0))).x >= 0) {
                                izquierda.matriz.push_back(renglon_matriz(str2float(linedata_tmp.at(0)),
                                    str2float(linedata_tmp.at(1)),
                                    str2float(linedata_tmp.at(2)),
                                    str2float(linedata_tmp.at(3))));
                            }
                            if (vertices3D->at(str2float(linedata_tmp.at(0))).x <= 0) {
                                derecha.matriz.push_back(renglon_matriz(str2float(linedata_tmp.at(0)),
                                    str2float(linedata_tmp.at(1)),
                                    str2float(linedata_tmp.at(2)),
                                    str2float(linedata_tmp.at(3))));
                            }
                            if (vertices3D->at(str2float(linedata_tmp.at(0))).x == 0) {
                                //std::printf(" ---> Punto Central, %f", str2float(linedata_tmp.at(1)));
                                vertex_class[str2float(linedata_tmp.at(0))] = CLASE_ANIMATION_CENTRAL;
                            }
                        }

                        //std::printf("\n------- Izquierda: %s // Cantidad de elementos: %d ", izquierda.name, int(izquierda.matriz.size()));
                        //std::printf("\n------- Derecha: %s // Cantidad de elementos: %d ", derecha.name, int(derecha.matriz.size()));

                        // Guardo mis Animation Units L y R generadas
                        animation_parameters.push_back(izquierda);
                        animation_parameters.push_back(derecha);
                        izquierda.matriz.clear();
                        derecha.matriz.clear();

                        // Inicializo todos los parametros de activacion de las units en 0
                        animation_coef.push_back(0.0);
                        animation_coef.push_back(0.0);
                        animation_coef_k.push_back(0.0);
                        animation_coef_k.push_back(0.0);
                    }

                }
            }
            std::cout << "----> Animation Units Cargadas" << std::endl;
        }


        /* ==============================================================================================
                                                      SHAPE UNITS
           ============================================================================================== */

        if (string::npos != line.find("# SHAPE UNITS LIST:")) {
            //std::printf("\n%s", line.c_str());
            // Lee la l�nea siguiente con la Shape Units disponibles
            std::getline(file, line);
            int cant_shape_units = atoi(line.c_str());
            //std::printf("%d", cant_shape_units);

            shape_parameters.reserve(cant_shape_units);
            shape_coef.reserve(cant_shape_units);

            for (int k = 0; k < cant_shape_units; k++) {
                // Inicializo todos los parametros de activacion de las units en 0
                shape_coef.push_back(0.0);
                // Busco el nombre de la Shape Unit
                while (string::npos == line.find("# ")) std::getline(file, line);

                if (string::npos != line.find("# ")) {
                    unit shape;
                    line = line.substr(2);
                    shape.name = _strdup(line.c_str());
                    std::getline(file, line);
                    int cant_de_renglones = atoi(line.c_str());

                    //std::printf("\n\n ============ SHAPE: %s // RENGLONES: %d ==========", shape.name, cant_de_renglones);

                    for (int j = 0; j < cant_de_renglones; j++)
                    {
                        getline(file, line);
                        //std::printf("\n ---  %s", line.c_str());
                        linedata_tmp.clear();
                        split_line(line, ' ', linedata_tmp);
                        shape.matriz.push_back(renglon_matriz(str2float(linedata_tmp.at(0)),
                            str2float(linedata_tmp.at(1)),
                            str2float(linedata_tmp.at(2)),
                            str2float(linedata_tmp.at(3))));
                    }
                    shape_parameters.push_back(shape);
                }
            }
            std::cout << "----> Shape Units Cargadas" << std::endl;
        }
        std::getline(file, line);
    }
    file.close();
}

void facialmesh::load_person(string personfile) {
    ifstream file; //ifstream = input file stream
    string line;
    vector<string> linedata_tmp;

    file.open(personfile);
    if (!file.is_open()) {
        //std::printf("\nNo se puedo abrir el archivo");
        return;
    }
    std::cout << "El archivo se abrio correctamente" << std::endl;
    std::getline(file, line);

    while (!(string::npos != line.find("# END")))
    {
        if (string::npos != line.find("# POSE:")) {
            // Cargo la traslacion
            std::getline(file, line);
            linedata_tmp.clear();
            split_line(line, ' ', linedata_tmp);
            Tvector.at<double>(0,0) = str2float(linedata_tmp.at(0));
            Tvector.at<double>(0,1) = str2float(linedata_tmp.at(1));
            Tvector.at<double>(0,2) = str2float(linedata_tmp.at(2));
            // Cargo la rotacion
            std::getline(file, line);
            linedata_tmp.clear();
            split_line(line, ' ', linedata_tmp);
            Rvector.at<double>(0,0) = str2float(linedata_tmp.at(0));
            Rvector.at<double>(0,1) = str2float(linedata_tmp.at(1));
            Rvector.at<double>(0,2) = str2float(linedata_tmp.at(2));
            actualizar_2D();
        }

        if (string::npos != line.find("# ANIMATION:")) {
            for (int k = 0; k < int(animation_parameters.size()); k++) {
                std::getline(file, line);
                animation_coef[k] = str2float(line.c_str());
                animation_coef_k[k] = str2float(line.c_str());
            }
        }

        if (string::npos != line.find("# SHAPE:")){
            for (int k = 0; k < int(shape_parameters.size()); k++) {
                std::getline(file, line);
                shape_coef[k] = str2float(line.c_str());
            }
        }
        std::getline(file, line);
    }
    actualizar_modelo();
    file.close();
}

void facialmesh::split_line(const string& s, char delim, vector<string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (!item.empty())
            elems.push_back(item);
    }
}

float facialmesh::str2float(string s)
{
    size_t found = s.find(","); // indx donde esta la coma
    if (found != std::string::npos)
        s[found] = '.'; // Change ',' to '.'

    return std::atof(s.c_str());
}

void facialmesh::leer_parametros(vector<vector<float> >* param, ifstream& file)
{
    string line;
    vector<string> linedata_tmp;
    getline(file, line);
    int cantidad = atoi(line.c_str());
    param->resize(cantidad);
    for (int indx = 0; indx < cantidad; indx++)
    {
        getline(file, line);
        linedata_tmp.clear();
        split_line(line, ' ', linedata_tmp);
        param->at(indx).push_back(str2float(linedata_tmp.at(0)));
        param->at(indx).push_back(str2float(linedata_tmp.at(1)));
        param->at(indx).push_back(-str2float(linedata_tmp.at(2)));
        param->at(indx).push_back(str2float(linedata_tmp.at(3)));
    }
}

void  facialmesh::rotartrasladar(double ang_xz, double ang_xy, double ang_yz, double d_x, double d_y, double d_z)
{
    // Rotacion
    Rvector.at<double>(0, 0) = ang_xz;
    Rvector.at<double>(0, 1) = ang_yz;
    Rvector.at<double>(0, 2) = ang_xy;
    // Traslacion
    Tvector.at<double>(0, 0) = d_x;
    Tvector.at<double>(0, 1) = -d_y; //antes lo habia puesto como -d_y
    Tvector.at<double>(0, 2) = d_z;
    actualizar_2D();
}

void  facialmesh::rotar(double ang_xz, double ang_xy, double ang_yz)
{
    Rvector.at<double>(0, 0) = ang_xz;
    Rvector.at<double>(0, 1) = ang_xy;
    Rvector.at<double>(0, 2) = ang_yz;
    actualizar_2D();
}
void  facialmesh::trasladar(double d_x, double d_y, double d_z)
{
    Tvector.at<double>(0, 0) = d_x;
    Tvector.at<double>(0, 1) = -d_y;
    Tvector.at<double>(0, 2) = d_z;
    actualizar_2D();
}

void facialmesh::set_animation_value(int AUV, float value)
{
    if (AUV <  int(animation_parameters.size())) {
        animation_coef[AUV] = value;
        float central_point = 1.0;
        for (int k = 0; k < int(animation_parameters[AUV].matriz.size()); k++)
        {
            if (vertex_class[animation_parameters[AUV].matriz.at(k).i] == CLASE_ANIMATION_CENTRAL) central_point = 0.5;
            animation3D->at(animation_parameters[AUV].matriz.at(k).i).x += value * animation_parameters[AUV].matriz.at(k).coef_xyz.x * central_point;
            animation3D->at(animation_parameters[AUV].matriz.at(k).i).y += value * (-animation_parameters[AUV].matriz.at(k).coef_xyz.y) * central_point;
            animation3D->at(animation_parameters[AUV].matriz.at(k).i).z += value * animation_parameters[AUV].matriz.at(k).coef_xyz.z * central_point;
            central_point = 1.0;
        }
    }
}

void facialmesh::set_shape_value(int SHAPE, float value)
{
    if (SHAPE <  int(shape_parameters.size())) {
        shape_coef[SHAPE] = value;
        for (int k = 0; k < int(shape_parameters[SHAPE].matriz.size()); k++)
        {
            shape3D->at(shape_parameters[SHAPE].matriz.at(k).i).x += value * shape_parameters[SHAPE].matriz.at(k).coef_xyz.x;
            shape3D->at(shape_parameters[SHAPE].matriz.at(k).i).y += value * (-shape_parameters[SHAPE].matriz.at(k).coef_xyz.y);
            shape3D->at(shape_parameters[SHAPE].matriz.at(k).i).z += value * shape_parameters[SHAPE].matriz.at(k).coef_xyz.z;
        }
    }
}

void facialmesh::actualizar_animation()
{
    animation3D->clear();
    animation3D->resize(vertices3D->size());
    for (int k = 0; k < int(animation_parameters.size()); k++) {
        set_animation_value(k, animation_coef[k]);
    }
    actualizar_vertices();
}

void facialmesh::actualizar_shape()
{
    shape3D->clear();
    shape3D->resize(vertices3D->size());
    for (int k = 0; k < int(shape_parameters.size()); k++) {
        set_shape_value(k, shape_coef[k]);
    }
    actualizar_vertices();
}

void facialmesh::actualizar_vertices()
{
    for (int k = 0; k < int(vertices3D->size()); k++)
    {
        vertices3D->at(k).x = shape3D->at(k).x * RESIZE_FACTOR + animation3D->at(k).x * RESIZE_FACTOR + vertices3D_original->at(k).x;
        vertices3D->at(k).y = shape3D->at(k).y * RESIZE_FACTOR + animation3D->at(k).y * RESIZE_FACTOR + vertices3D_original->at(k).y;
        vertices3D->at(k).z = shape3D->at(k).z * RESIZE_FACTOR + animation3D->at(k).z * RESIZE_FACTOR + vertices3D_original->at(k).z;

        vertices3D_noanimation->at(k).x = shape3D->at(k).x * RESIZE_FACTOR+ vertices3D_original->at(k).x;
        vertices3D_noanimation->at(k).y = shape3D->at(k).y * RESIZE_FACTOR+ vertices3D_original->at(k).y;
        vertices3D_noanimation->at(k).z = shape3D->at(k).z * RESIZE_FACTOR+ vertices3D_original->at(k).z;
    }
    actualizar_2D();
}

void facialmesh::actualizar_2D()
{
    projectPoints(*vertices3D, Rvector, Tvector, camera_matrix, distCoeffs, *vertices2D);
    projectPoints(*vertices3D_noanimation, Rvector, Tvector, camera_matrix, distCoeffs, *vertices2D_noanimation);
}

void facialmesh::actualizar_modelo()
{
    actualizar_animation();
    actualizar_shape();
}


void facialmesh::resetear_animation()
{
    for (int k = 0; k < int(animation_parameters.size()); k++) {
        set_animation_value(k, 0);
    }
    actualizar_animation();
}

void facialmesh::resetear_shape()
{
    for (int k = 0; k < int(shape_parameters.size()); k++) {
        set_shape_value(k, 0);
    }
    actualizar_shape();
}

void facialmesh::resetear_pose()
{
    rotartrasladar(0, 0, PI, 0, 0, 8);
}

void facialmesh::resetear_modelo()
{
    resetear_animation();
    resetear_shape();
    resetear_pose();
}

cv::Mat facialmesh::graficar_mesh(cv::Mat img)
{
    cv::Mat imagen = img.clone();
    for (int k = 0; k < int(faces.size()); k++)
    {
        cv::line(imagen, vertices2D->at(faces[k].a), vertices2D->at(faces[k].b), COLOR_AZUL);
        cv::line(imagen, vertices2D->at(faces[k].b), vertices2D->at(faces[k].c), COLOR_AZUL);
        cv::line(imagen, vertices2D->at(faces[k].c), vertices2D->at(faces[k].a), COLOR_AZUL);
    }
    return imagen;
}

cv::Mat facialmesh::graficar_vertice(cv::Mat img, int vertice, cv::Scalar COLOR)
{
    cv::Mat imagen = img.clone();
    if (vertice >= 0 && vertice < int(vertices3D->size()))
        cv::circle(imagen, cv::Point2i(vertices2D->at(vertice).x, vertices2D->at(vertice).y), RESIZE_FACTOR / 2.0, COLOR, RESIZE_FACTOR);
    // Graficar puntos
    cv::putText(imagen, std::to_string(static_cast<int>(vertice)), cv::Point(static_cast<int>(vertices2D->at(vertice).x),
        static_cast<int>(vertices2D->at(vertice).y)), cv::FONT_HERSHEY_DUPLEX, 0.55, COLOR_NEGRO, 1);

    return imagen;
}

cv::Mat facialmesh::graficar_pose_tracking_points(cv::Mat img)
{
    std::cout << int(pose_tracking_indxs->size()) << std::endl;
    cv::Mat imagen = img.clone();
    for (int k = 0; k < int(pose_tracking_indxs->size()); k++)
    {
        imagen = graficar_vertice(imagen, pose_tracking_indxs->at(k), COLOR_ROJO);
    }
    return imagen;
}

cv::Mat facialmesh::graficar_auv(cv::Mat img, int AUV)
{
    cv::Mat imagen = img.clone();
    if (AUV < int(animation_parameters.size())) {
        for (int k = 0; k < int(animation_parameters[AUV].matriz.size()); k++) {
            imagen = graficar_vertice(imagen, animation_parameters[AUV].matriz.at(k).i, COLOR_ROJO);
        }
        return imagen;
    }
    else return imagen;

}

cv::Mat facialmesh::graficar_shape(cv::Mat img, int SHAPE)
{
    cv::Mat imagen = img.clone();
    if (SHAPE < int(shape_parameters.size())) {
        for (int k = 0; k < int(shape_parameters[SHAPE].matriz.size()); k++) {
            imagen = graficar_vertice(imagen, shape_parameters[SHAPE].matriz.at(k).i, COLOR_VERDE);
        }
        return imagen;
    }
    else return imagen;
}
void facialmesh::set_keyframe_image(cv::Mat keyframe)
{
    initial_keyframe = keyframe.clone();
    // Si es RGB, la convierto a escala de grises
    //if (initial_keyframe.channels() == 3)
    //{
    //    cv::cvtColor(initial_keyframe, initial_keyframe, cv::COLOR_BGR2GRAY);
    //}

}
cv::Mat facialmesh::get_keyframe(void)
{
    return initial_keyframe;
}
void facialmesh::set_keyframe_model(void)
{
    actualizar_2D();
    *vertices3D_keyframe = *vertices3D;
    *vertices2D_keyframe = *vertices2D;
    for(int k = 0; k < int(animation_coef.size()); k++)
        animation_coef_k[k] = animation_coef[k];
}
cv::Mat facialmesh::render_keyframe(void)
{
    if (initial_keyframe.empty()) return cv::Mat(1,1,CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Mat keyframe_img_acomodada;
    if (initial_keyframe.channels() == 3) //KeyFrame en RGB
    {
        keyframe_img_acomodada = cv::Mat(initial_keyframe.rows, initial_keyframe.cols, CV_8UC3, cv::Scalar(128, 128, 128)); // Fondo gris
        //initial_keyframe.copyTo(keyframe_img_acomodada);
    }
    else //KeyFrame en escala de grises
    {
        keyframe_img_acomodada = cv::Mat(initial_keyframe.rows, initial_keyframe.cols, CV_8UC1, cv::Scalar(128)); // Fondo gris
    }


    // Mascara para el mapeo
    cv::Mat mascara(initial_keyframe.rows, initial_keyframe.cols, CV_8UC1, cv::Scalar(0));
    cv::Mat mascara_anim(initial_keyframe.rows, initial_keyframe.cols, CV_8UC1, cv::Scalar(0));

    // Para cada cara del modelo
    face face_vertexs;
    puntos2d v_keyframe(3);
    puntos2d v_image(3);
    vector<cv::Point2i> v_image_int(3);
    vector<cv::Mat> Affine_matrix(faces.size());

    for (int k = 0; k < int(faces.size()); k++)
    {
        // Vertices de la cara
        face_vertexs = faces.at(k);
        // Ubicacion de los puntos de esta cara en el keyframe
        // Luego pasar a los puntos en el keyframe anterior cuando se implemente el alg de RG
        v_keyframe.at(0) = vertices2D_keyframe->at((int)face_vertexs.a);
        v_keyframe.at(1) = vertices2D_keyframe->at((int)face_vertexs.b);
        v_keyframe.at(2) = vertices2D_keyframe->at((int)face_vertexs.c);

        // Ubicacion de los puntos en la nueva imagen
        v_image.at(0) = vertices2D->at((int)face_vertexs.a);
        v_image.at(1) = vertices2D->at((int)face_vertexs.b);
        v_image.at(2) = vertices2D->at((int)face_vertexs.c);

        v_image_int.at(0) = v_image.at(0);
        v_image_int.at(1) = v_image.at(1);
        v_image_int.at(2) = v_image.at(2);

        // Calculo de la transformacion de cada face
        Affine_matrix.at(k) = cv::getAffineTransform(v_image, v_keyframe);

        // Mascara para la cara
        cv::fillConvexPoly(mascara, v_image_int, cv::Scalar(k + 1));
        cv::fillConvexPoly(mascara_anim, v_image_int, cv::Scalar(255));
        //cv::fillConvexPoly(keyframe_img_acomodada, v_image_int, cv::Scalar(255));

        //std::cout << Affine_matrix.at(k) << std::endl;

        //cv::warpAffine(initial_keyframe, keyframe_img_acomodada, Affine_matrix.at(k), cv::Size(keyframe_img_acomodada.cols, keyframe_img_acomodada.rows));

    }

    // Realiza el mapeo
    int x_new, y_new, mask_value;
    cv::Scalar mask_color;
    for (int y = 0; y < initial_keyframe.rows; y++)
    {
        for (int x = 0; x < initial_keyframe.cols; x++)
        {
            mask_color = mascara.at<uchar>(y, x);
            mask_value = (int)mask_color.val[0] - 1;

            // Ubicacion del pixel en el keyframe acomodado
            if (mask_value >= 0)
            {

                x_new = cvRound(x * (Affine_matrix.at(mask_value)).at<double>(0, 0) +
                                y * (Affine_matrix.at(mask_value)).at<double>(0, 1) +
                                    (Affine_matrix.at(mask_value)).at<double>(0, 2));

                y_new = cvRound(x * (Affine_matrix.at(mask_value)).at<double>(1, 0) +
                                y * (Affine_matrix.at(mask_value)).at<double>(1, 1) +
                                    (Affine_matrix.at(mask_value)).at<double>(1, 2));

                //std::cout << " X:" << x << " X-new:" << x_new << " Y:" << y << " Y-new:" << y_new << std::endl;
                // Mapea
                if ((y_new >= 0) && (y_new < initial_keyframe.rows) && (x_new >= 0) && (x_new < initial_keyframe.cols))
                {
                    if (initial_keyframe.channels() > 1) // Si es RGB debo mapear de esta forma
                    {
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[0] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[0];
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[1] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[1];
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[2] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[2];
                    }
                    else // Si es escala de grises se mapea de esta forma
                    {
                        keyframe_img_acomodada.at<uchar>(y, x) = initial_keyframe.at<uchar>(y_new, x_new);
                    }

                }
            }
        }
    }
    return keyframe_img_acomodada;
}

cv::Mat facialmesh::render_keyframe_v2(cv::Mat img)
{
    if (img.empty()) return cv::Mat(1,1,CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Mat keyframe_img_acomodada = img.clone();

    // Mascara para el mapeo
    cv::Mat mascara(keyframe_img_acomodada.rows, keyframe_img_acomodada.cols, CV_8UC1, cv::Scalar(0));
    cv::Mat mascara_anim(keyframe_img_acomodada.rows, keyframe_img_acomodada.cols, CV_8UC1, cv::Scalar(0));

    // Para cada cara del modelo
    face face_vertexs;
    puntos2d v_keyframe(3);
    puntos2d v_image(3);
    vector<cv::Point2i> v_image_int(3);
    vector<cv::Mat> Affine_matrix(faces.size());

    for (int k = 0; k < int(faces.size()); k++)
    {
        // Vertices de la cara
        face_vertexs = faces.at(k);
        // Ubicacion de los puntos de esta cara en el keyframe
        // Luego pasar a los puntos en el keyframe anterior cuando se implemente el alg de RG
        v_keyframe.at(0) = vertices2D_keyframe->at((int)face_vertexs.a);
        v_keyframe.at(1) = vertices2D_keyframe->at((int)face_vertexs.b);
        v_keyframe.at(2) = vertices2D_keyframe->at((int)face_vertexs.c);

        // Ubicacion de los puntos en la nueva imagen
        v_image.at(0) = vertices2D->at((int)face_vertexs.a);
        v_image.at(1) = vertices2D->at((int)face_vertexs.b);
        v_image.at(2) = vertices2D->at((int)face_vertexs.c);

        v_image_int.at(0) = v_image.at(0);
        v_image_int.at(1) = v_image.at(1);
        v_image_int.at(2) = v_image.at(2);

        // Calculo de la transformacion de cada face
        Affine_matrix.at(k) = cv::getAffineTransform(v_image, v_keyframe);

        // Mascara para la cara
        cv::fillConvexPoly(mascara, v_image_int, cv::Scalar(k + 1));
        cv::fillConvexPoly(mascara_anim, v_image_int, cv::Scalar(255));
    }

    // Ubicacion de los puntos limitantes de la boca
    std::vector<face> face_boca = std::vector<face>(8);
    face_boca.at(0) = face(88,83,81);
    face_boca.at(1) = face(87,83,81);
    face_boca.at(2) = face(40,83,81);
    face_boca.at(3) = face(89,82,84);
    face_boca.at(4) = face(87,82,84);
    face_boca.at(5) = face(40,82,84);
    face_boca.at(6) = face(40,87,88);
    face_boca.at(7) = face(40,87,89);
    // Rellenar boca para q no se vean los dientes
    for (int k = 0; k < int(face_boca.size()); k++){
        // Ubicacion de los puntos en la nueva imagen
        v_image.at(0) = vertices2D->at((int)face_boca.at(k).a);
        v_image.at(1) = vertices2D->at((int)face_boca.at(k).b);
        v_image.at(2) = vertices2D->at((int)face_boca.at(k).c);

        v_image_int.at(0) = v_image.at(0);
        v_image_int.at(1) = v_image.at(1);
        v_image_int.at(2) = v_image.at(2);

        // Mascara para la cara
        cv::fillConvexPoly(mascara_anim, v_image_int, cv::Scalar(255));
    }
    keyframe_img_acomodada.setTo(cv::Scalar(0,0,0), mascara_anim);


    // Realiza el mapeo
    int x_new, y_new, mask_value;
    cv::Scalar mask_color;
    for (int y = 0; y < keyframe_img_acomodada.rows; y++)
    {
        for (int x = 0; x < keyframe_img_acomodada.cols; x++)
        {
            mask_color = mascara.at<uchar>(y, x);
            mask_value = (int)mask_color.val[0] - 1;

            // Ubicacion del pixel en el keyframe acomodado
            if (mask_value >= 0)
            {

                x_new = cvRound(x * (Affine_matrix.at(mask_value)).at<double>(0, 0) +
                                y * (Affine_matrix.at(mask_value)).at<double>(0, 1) +
                                    (Affine_matrix.at(mask_value)).at<double>(0, 2));

                y_new = cvRound(x * (Affine_matrix.at(mask_value)).at<double>(1, 0) +
                                y * (Affine_matrix.at(mask_value)).at<double>(1, 1) +
                                    (Affine_matrix.at(mask_value)).at<double>(1, 2));

                //std::cout << " X:" << x << " X-new:" << x_new << " Y:" << y << " Y-new:" << y_new << std::endl;
                // Mapea
                if ((y_new >= 0) && (y_new < initial_keyframe.rows) && (x_new >= 0) && (x_new < initial_keyframe.cols))
                {
                    if (initial_keyframe.channels() > 1) // Si es RGB debo mapear de esta forma
                    {
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[0] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[0];
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[1] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[1];
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[2] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[2];
                    }
                    else // Si es escala de grises se mapea de esta forma
                    {
                        keyframe_img_acomodada.at<uchar>(y, x) = initial_keyframe.at<uchar>(y_new, x_new);
                    }

                }
            }
        }
    }
    return keyframe_img_acomodada;
}

cv::Mat facialmesh::render_keyframe_v3(cv::Mat img)
{
    if (img.empty()) return cv::Mat(1, 1, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Mat keyframe_img_acomodada = img.clone();

    // Mascara para el mapeo
    cv::Mat mascara(keyframe_img_acomodada.rows, keyframe_img_acomodada.cols, CV_8UC1, cv::Scalar(0));
    cv::Mat mascara_anim(keyframe_img_acomodada.rows, keyframe_img_acomodada.cols, CV_8UC1, cv::Scalar(0));

    // Para cada cara del modelo
    face face_vertexs;
    puntos2d v_keyframe(3);
    puntos2d v_image(3);
    vector<cv::Point2i> v_image_int(3);
    vector<cv::Mat> Affine_matrix(faces.size());

    for (int k = 0; k < int(faces.size()); k++)
    {
        // Vertices de la cara
        face_vertexs = faces.at(k);
        // Ubicacion de los puntos de esta cara en el keyframe
        // Luego pasar a los puntos en el keyframe anterior cuando se implemente el alg de RG
        v_keyframe.at(0) = vertices2D_keyframe->at((int)face_vertexs.a);
        v_keyframe.at(1) = vertices2D_keyframe->at((int)face_vertexs.b);
        v_keyframe.at(2) = vertices2D_keyframe->at((int)face_vertexs.c);

        // Ubicacion de los puntos en la nueva imagen
        v_image.at(0) = vertices2D->at((int)face_vertexs.a);
        v_image.at(1) = vertices2D->at((int)face_vertexs.b);
        v_image.at(2) = vertices2D->at((int)face_vertexs.c);

        v_image_int.at(0) = v_image.at(0);
        v_image_int.at(1) = v_image.at(1);
        v_image_int.at(2) = v_image.at(2);

        // Calculo de la transformacion de cada face
        Affine_matrix.at(k) = cv::getAffineTransform(v_image, v_keyframe);

        // Mascara para la cara
        cv::fillConvexPoly(mascara, v_image_int, cv::Scalar(k + 1));
        cv::fillConvexPoly(mascara_anim, v_image_int, cv::Scalar(255));
    }


    // Realiza el mapeo
    int x_new, y_new, mask_value;
    cv::Scalar mask_color;
    for (int y = 0; y < keyframe_img_acomodada.rows; y++)
    {
        for (int x = 0; x < keyframe_img_acomodada.cols; x++)
        {
            mask_color = mascara.at<uchar>(y, x);
            mask_value = (int)mask_color.val[0] - 1;

            // Ubicacion del pixel en el keyframe acomodado
            if (mask_value >= 0)
            {

                x_new = cvRound(x * (Affine_matrix.at(mask_value)).at<double>(0, 0) +
                    y * (Affine_matrix.at(mask_value)).at<double>(0, 1) +
                    (Affine_matrix.at(mask_value)).at<double>(0, 2));

                y_new = cvRound(x * (Affine_matrix.at(mask_value)).at<double>(1, 0) +
                    y * (Affine_matrix.at(mask_value)).at<double>(1, 1) +
                    (Affine_matrix.at(mask_value)).at<double>(1, 2));

                //std::cout << " X:" << x << " X-new:" << x_new << " Y:" << y << " Y-new:" << y_new << std::endl;
                // Mapea
                if ((y_new >= 0) && (y_new < initial_keyframe.rows) && (x_new >= 0) && (x_new < initial_keyframe.cols))
                {
                    if (initial_keyframe.channels() > 1) // Si es RGB debo mapear de esta forma
                    {
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[0] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[0];
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[1] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[1];
                        keyframe_img_acomodada.at<cv::Vec3b>(y, x)[2] = initial_keyframe.at<cv::Vec3b>(y_new, x_new)[2];
                    }
                    else // Si es escala de grises se mapea de esta forma
                    {
                        keyframe_img_acomodada.at<uchar>(y, x) = initial_keyframe.at<uchar>(y_new, x_new);
                    }

                }
            }
        }
    }
    return keyframe_img_acomodada;
}



cv::Mat GetSquareImage(const cv::Mat& img, int target_width = 500)
{
    int width = img.cols,
        height = img.rows;

    cv::Mat square = cv::Mat::zeros(target_width, target_width, img.type());

    int max_dim = (width >= height) ? width : height;
    float scale = ((float)target_width) / max_dim;
    cv::Rect roi;
    if (width >= height)
    {
        roi.width = target_width;
        roi.x = 0;
        roi.height = height * scale;
        roi.y = (target_width - roi.height) / 2;
    }
    else
    {
        roi.y = 0;
        roi.height = target_width;
        roi.width = width * scale;
        roi.x = (target_width - roi.width) / 2;
    }

    cv::resize(img, square(roi), roi.size());

    return square;
}


cv::Mat GetFittedImage(const cv::Mat& img, int target_height = 500,int target_width = 500)
{
    int width = img.cols,
        height = img.rows;
    cv::Rect roi;
    cv::Mat square = cv::Mat::zeros(target_height, target_width, img.type());
    float rel = float(width) / float(height);
    float target_rel = float(target_width) / float(target_height);

    if (target_rel >= rel)
    {
        roi.y = 0;
        roi.height = target_height;
        roi.width = target_height * rel;
        roi.x = (target_width - roi.width) / 2;
    }
    else
    {
        roi.x = 0;
        roi.width = target_width;
        roi.height = (target_width / rel); //target_height * scale;
        roi.y = (target_height - roi.height) / 2;
    }
    cv::resize(img, square(roi), roi.size());
    return square;
}

void grayscale(cv::Mat& source, cv::Mat& destination) {

    cv::Mat planes[3], sx, sy;
    cv::Mat dif, highprecission;

    cv::split(source, planes);
    cv::medianBlur(planes[0], planes[0], 3);
    cv::medianBlur(planes[1], planes[1], 3);
    cv::medianBlur(planes[2], planes[2], 3);

    for (int l = 0; l < 3; l++) {
        cv::Sobel(planes[l], highprecission, CV_16S, 1, 0, 3, 2);
        cv::convertScaleAbs(highprecission, sx);
        cv::Sobel(planes[l], highprecission, CV_16S, 0, 1, 3, 2);
        cv::convertScaleAbs(highprecission, sy);
        if (l == 0) {
            destination = cv::max(sx, sy);
        }
        else {
            destination = cv::max(destination, cv::max(sx, sy));
        }
    }

    // TODO: probar si mejora aplicandole esto luego
    //cv::equalizeHist(image, image);

}


