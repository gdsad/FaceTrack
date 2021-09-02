#include "facetracking2.h"

facetracking::facetracking(){
    HC_face.load("haarcascade_frontalface_alt2.xml");
    facemark = cv::face::FacemarkLBF::create();
    facemark->loadModel("lbfmodel.yaml");
    termcrit = cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03);
    winSize = cv::Size(20, 20);
}

cv::Rect facetracking::detectFace(cv::Mat img, bool& face_detected) {

    cv::Mat gray, small_img;
    int maxArea = 0;
    int indx = 0;

    // Preproceso la imagen
    if (img.channels() == 3) cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else gray = img.clone();
    cv::resize(gray, small_img, cv::Size(img.cols * RESIZE_F, img.rows * RESIZE_F) , 0, 0, cv::INTER_LINEAR);
    cv::equalizeHist(small_img, small_img);
    std::vector<cv::Rect> faces;
    HC_face.detectMultiScale(small_img, faces, 1.1 , 3, cv::CASCADE_FIND_BIGGEST_OBJECT, cv::Size(50,50));

    //Me quedo con la face de mayor area
    if (faces.size() > 0) {

        face_detected = 1;
        for (int i = 0; i< int(faces.size()); i++) {
            if (faces[i].width * faces[i].height > maxArea) {
                maxArea = faces[i].width * faces[i].height;
                indx = i;
            }
        }
        faces[indx].width = cvRound(faces[indx].width * RESIZE_F);
        faces[indx].height = cvRound(faces[indx].height * RESIZE_F);
        faces[indx].x = cvRound(faces[indx].x * RESIZE_F);
        faces[indx].y = cvRound(faces[indx].y * RESIZE_F);

        return faces[indx];
    }
    face_detected = 0;
    return cv::Rect(1,1,1,1);
}

void facetracking::facelandmarks(cv::Mat img, cv::Rect face, bool &detect, int type) {

    //TODO: crear estructura de datos que devuelva el punto 2d y tambien el nro de vertice del modelo 3D
    //      agregar un factor de escala para el procesamiento de la imagen de entrada

    std::vector< std::vector<cv::Point2f> > face_landmarks;
    landmarks.clear();
    landmarks.reserve(68);
    detect = 0;
    std::vector<cv::Rect > face_vector;
    face_vector.push_back(face);

    // Cargo los indices de los pose tracking vertex
    std::vector<int> *landmarks_pose_indxs = new std::vector<int>{ 27,28,30,31,35,0,8,16,39,42 };

    // Cargo los indices de los animation tracking vertex
    //std::vector<int> *landmarks_animation_indxs = new std::vector<int>{0,4,7, 8,9,12,13,16,17,18,21,22,25,26,28,32,34,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55, 57,59,60,61,63,64,65,67};

    // Cargo los indices de todos los landmarks que se corresponden con un vertice de Candide
    std::vector<int> *landmarks_indxs = new std::vector<int>{0,4,7, 8,9,12,13,16,17,18,21,22,25,26,28,32,34,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55, 57,59,60,61,63,64,65,67};



    // Run landmark detector
    bool success = facemark->fit(img, face_vector, face_landmarks);
    if (success && face_landmarks[0].size() == 68) {
            detect = 1;
            if (type == LANDMARKS_POSE){
            //Para la pose - Funciona bien para la pose. Lo comente para probar todos los landmarks
            for(int k = 0 ; k < int(landmarks_pose_indxs->size()) ; k++)
                landmarks.push_back(face_landmarks[0][landmarks_pose_indxs->at(k)]);
            }
            else if (type == LANDMARKS_ANIMATION){
                std::cout << "Falta implementarlo" << std::endl;
            }
            else if (type == LANDMARKS_ALL){
            //Para todos los landmarks
            for(int k = 0 ; k < int(landmarks_indxs->size()) ; k++)
                landmarks.push_back(face_landmarks[0][landmarks_indxs->at(k)]);
            }
         /* Landmark 27 --> Nariz arriba(Vertice 37)
            Landmark 28 --> Nariz medio(Vertice 94)
            Landmark 30 --> Nariz abajo(Vertice 5) o 38
            Landmark 31 --> Nariz izq(Vertice 111)
            Landmark 35 --> Nariz der(Vertice 112)
            Landmark 0 --> Costado izq(Vertice 29)
            Landmark 8 --> Pera(Vertice 10)
            Landmark 16 --> Costado der(Vertice 62)
            Landmark 39 --> Ojo izq(Vertice 23)
            Landmark 42 --> Ojo der(Vertice 56)
            Landmark 54 --> boca derech(Vertice 56)
            Landmark 48 --> boca izq(Vertice 64)
            Landmark 57 --> boca abajo(Vertice 8)
            Landmark 51 --> boca arriba(Vertice 7)
            //landmarks.push_back(face_landmarks[0][29]); -- no se corresponde a un vertice del modelo */
        }
}

void facetracking::drawlandmarks(cv::Mat img) {
    for (int k = 0; k < int(landmarks.size()); k++) {
        cv::circle(img, landmarks[k], 1, cv::Scalar((0), (0), (255), 0), 3);
    }
}

void facetracking::estimar_pose_landmarks(cv::Mat img)
{
    posevertex.clear();
    posevertex.reserve(candide->pose_tracking_indxs->size());
    for(int k = 0 ; k < int(candide->pose_tracking_indxs->size()); k++)
        posevertex.push_back(candide->vertices3D->at(candide->pose_tracking_indxs->at(k)));
    bool facedetected = 0, landmarksdetected = 0;
    cv::Rect face_roi = detectFace(img, facedetected);
    if (facedetected) {
        facelandmarks(img, face_roi, landmarksdetected, LANDMARKS_POSE);
        if (landmarksdetected) {
            /* --------------------------------- LEVMAR --------------------------------
            cv::Mat Rvector = candide->Rvector.clone();
            cv::Mat Tvector = candide->Tvector.clone();
            ajustar_pose_levmar(&posevertex, &landmarks,&Rvector,&Tvector);
            candide->Rvector = Rvector.clone();
            candide->Tvector = Tvector.clone();
            candide->actualizar_2D();
            Rvector.release();
            Tvector.release(); */

            /* -------------------------------- SOLVEPNP ------------------------------- */
            cv::solvePnP(posevertex, landmarks, candide->camera_matrix, candide->distCoeffs, candide->Rvector, candide->Tvector);
            candide->actualizar_2D();
        }
    }
}

void facetracking::estimar_animacion_landmarks(cv::Mat img, puntos2d &puntos)
{
    bool facedetected = 0, landmarksdetected = 0;
    cv::Rect face_roi = detectFace(img, facedetected);
    if (facedetected) {
        facelandmarks(img, face_roi, landmarksdetected, LANDMARKS_ALL);
        if (landmarksdetected) {
            // --------------------------------- LEVMAR --------------------------------
            ajustar_animacion_levmar(&landmarks, candide->candide_lankmarks_indxs, candide);
            candide->actualizar_2D();
            puntos = landmarks;
        }
    }
}








void facetracking::optflow(cv::Mat prev_img, cv::Mat img, std::vector<cv::Point2f> prev_points,
                           std::vector<cv::Point2f>* points, std::vector<uchar>* status, std::vector<float>* error) {
    cv::Mat image = img.clone(), prev_image = prev_img.clone();
    if (image.channels() == 3) cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    if (prev_image.channels() == 3)  cv::cvtColor(prev_image, prev_image, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(image,image);
    cv::equalizeHist(prev_image, prev_image);

    cv::calcOpticalFlowPyrLK(prev_image, image, prev_points, *points, *status, *error, winSize,
        3, termcrit, 0, 0.001);

}



void facetracking::buscar_puntos(cv::Mat &prev_img, cv::Mat &curr_img, puntos2d &prev_points, int level,
                                puntos2d &puntos_nuevos, vector<int> &indx_OK, cv::Size winsize)
{
    std::vector<uchar> status = std::vector<uchar>();
    std::vector<float> error = std::vector<float>();
    puntos2d curr_points;
    cv::calcOpticalFlowPyrLK(prev_img, curr_img, prev_points, curr_points, status, error, winsize, level);
    // Cuenta la cantidad de puntos OK
    int puntos_ok = 0;
    for (int indx = 0; indx < int(status.size()); indx++)
    {
        if ((status.at(indx) == 1) && (error.at(indx) <5))
            puntos_ok++;
        else
            status.at(indx) = 0;
    }
    indx_OK.resize(puntos_ok);
    puntos_nuevos.resize(puntos_ok);
    // Escribe los puntos ok y sus indices
    int i, k;
    for (i = k = 0; i < int(prev_points.size()); i++)
    {
        // Verifica si el punto fue encontrado (para utilizarlo en la estimaci�n de la pose)
        if (status.at(i) == 0)
            continue;
        puntos_nuevos.at(k) = curr_points.at(i);
        indx_OK.at(k) = i;
        k++;
    }
}

void facetracking::estimar_pose_lk_kf(cv::Mat prev_img, cv::Mat img){
    puntos2d *puntos_prev(candide->vertices2D); // En lo de Lucas es candide.vertices2D_noanimation, que en nuestra libreria no esta implementado todavia
    puntos2d puntos_pose_tracking(candide->pose_tracking_indxs->size());
    puntos2d puntos_pose_tracking_k(candide->pose_tracking_indxs->size());
    posevertex.clear();
    posevertex.reserve(candide->pose_tracking_indxs->size());
    // Selecciona los puntos para la estimacion de la pose
    // =============================== Cargo P(t-1) ==================================
    for (int k = 0; k < int(candide->pose_tracking_indxs->size()); k++) // Actualmente pose_tracking_indxs no tiene nada en nuestra libreria
    {
        puntos_pose_tracking.at(k) = candide->vertices2D->at(candide->pose_tracking_indxs->at(k));
        puntos_pose_tracking_k.at(k) = candide->vertices2D_keyframe->at(candide->pose_tracking_indxs->at(k));
        posevertex.push_back(candide->vertices3D->at(candide->pose_tracking_indxs->at(k)));
    }
    // =============================== Estimo P(t) ==================================
    // ___________________________OBTENGO OPTICAL FLOW LK____________________________

    // Opt Flow LK entre Frame(t) y Frame(t-1)
    puntos2d puntos_nuevos = puntos2d();
    vector<int> indx_OK = vector<int>();
    buscar_puntos(prev_img, img, puntos_pose_tracking, 3,  puntos_nuevos, indx_OK);


    // Opt Flow LK entre Frame(t) y KeyFrame -- vers 1
    puntos2d puntos_nuevosK1 = puntos2d();
    vector<int> indx_OKK1 = vector<int>();
    cv::Mat keyframe1 = candide->get_keyframe();
    buscar_puntos(keyframe1, img, puntos_pose_tracking_k, 3, puntos_nuevosK1, indx_OKK1); //aca Lucas usa el Keyframe original

    // Opt Flow LK entre Frame(t) y KeyFrame -- vers 2
    puntos2d puntos_nuevosK2 = puntos2d();
    vector<int> indx_OKK2 = vector<int>();
    cv::Mat keyframe2 = candide->render_keyframe();
    buscar_puntos(keyframe2, img, puntos_pose_tracking, 3, puntos_nuevosK2, indx_OKK2); //aca Lucas usa el Keyframe original

    // Busca los puntos OK
    int puntos_ok = int(indx_OK.size()) + int(indx_OKK1.size()) + int(indx_OKK2.size());
    std::cout << "Puntos ok: " << puntos_ok << std::endl;
    puntos3d actual_3D_ok = puntos3d(puntos_ok);
    puntos2d actual_2D_ok = puntos2d(puntos_ok);
    // Escribe los puntos 3D y 2D para la estimaciosn de la pose
    int k = 0, i= 0, j = 0;
    for (k = 0; k < int(indx_OK.size()); k++)
    {
        int indice = candide->pose_tracking_indxs->at(indx_OK.at(k));
        actual_3D_ok.at(k) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k) = puntos_nuevos.at(k);
    }
    for (i = 0; i < int(indx_OKK1.size()); i++) // checkear este for aca no deberia ser i < indx_OKK.size() ?
    {
        int indice = candide->pose_tracking_indxs->at(indx_OKK1.at(i));
        actual_3D_ok.at(k + i) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k + i) = puntos_nuevosK1.at(i);
    }
    for (j = 0; j < int(indx_OKK2.size()); j++) // checkear este for aca no deberia ser i < indx_OKK.size() ?
    {

        int indice = candide->pose_tracking_indxs->at(indx_OKK2.at(j));
        actual_3D_ok.at(k + i + j) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k + i + j) = puntos_nuevosK2.at(j);
    }
    std::cout << "K: " << k << "/ I: " << i << "/ J: " << j << std::endl;

    // Landmarks
    bool facedetected = 0, landmarksdetected = 0;
    cv::Rect face_roi = detectFace(img, facedetected);
    if (facedetected) {
        facelandmarks(img, face_roi, landmarksdetected, LANDMARKS_POSE);
        if (landmarksdetected) {
            actual_2D_ok.reserve(10);
            actual_3D_ok.reserve(10);
            for(int l = 0 ; l < int(posevertex.size()) ; l++){
                actual_3D_ok.push_back(posevertex.at(l));
                actual_2D_ok.push_back(landmarks.at(l));
            }
        }
    }

    // Estimo y actualizo la pose
    if (puntos_ok > 5)
    {
        cv::solvePnP(actual_3D_ok, actual_2D_ok, candide->camera_matrix, candide->distCoeffs, candide->Rvector, candide->Tvector);
        candide->actualizar_2D();
    }
    // else resetear modelo?
}









// LUCAS ESTIMACION DE POSE DE LA MASCARA


static double tukey_M_Estimator(double x, double c)
{
    // Regla
    //y = (c^2/6)*(1-(1-(x/c).^2).^3); % Si |x| <= c
    if (abs(x) <= c)
    {
        return (c * c / 6.0) * (1 - pow((1 - (x / c) * (x / c)), 3));
    }
    else //y( abs(x) > c ) = c^2/6; % Si |x| > c
    {
        return c * c / 6.0;
    }
}

// ====================================================================================================
// ----------------------------------------------------------------------------------------------- POSE
// ====================================================================================================
void facetracking::estimar_pose_lk(cv::Mat prev_img, cv::Mat img, puntos2d &puntos)
{
    // Convierto imagenes de entrada a blanco y negro ?

    puntos2d *puntos_prev(candide->vertices2D_noanimation); // En lo de Lucas es candide.vertices2D_noanimation, que en nuestra libreria no esta implementado todavia
    puntos2d puntos_pose_tracking(candide->pose_tracking_indxs->size());
    puntos2d puntos_pose_tracking_k(candide->pose_tracking_indxs->size());
    // Selecciona los puntos para la estimacion de la pose
    // =============================== Cargo P(t-1) ==================================
    for (int k = 0; k < int(candide->pose_tracking_indxs->size()); k++) // Actualmente pose_tracking_indxs no tiene nada en nuestra libreria
    {
        puntos_pose_tracking.at(k) = candide->vertices2D->at(candide->pose_tracking_indxs->at(k));
        puntos_pose_tracking_k.at(k) = candide->vertices2D_keyframe->at(candide->pose_tracking_indxs->at(k));
    }
    // =============================== Estimo P(t) ==================================
    // ___________________________OBTENGO OPTICAL FLOW LK____________________________

    // Opt Flow LK entre Frame(t) y Frame(t-1)
    puntos2d puntos_nuevos = puntos2d();
    vector<int> indx_OK = vector<int>();
    buscar_puntos(prev_img, img, puntos_pose_tracking, 3,  puntos_nuevos, indx_OK);

    // Opt Flow LK entre Frame(t) y KeyFrame -- vers 1
    puntos2d puntos_nuevosK1 = puntos2d();
    vector<int> indx_OKK1 = vector<int>();
    cv::Mat keyframe1 = candide->get_keyframe();
    buscar_puntos(keyframe1, img, puntos_pose_tracking_k, 3, puntos_nuevosK1, indx_OKK1); //aca Lucas usa el Keyframe original

    // Opt Flow LK entre Frame(t) y KeyFrame -- vers 2
    puntos2d puntos_nuevosK2 = puntos2d();
    vector<int> indx_OKK2 = vector<int>();
    cv::Mat keyframe2 = candide->render_keyframe();
    buscar_puntos(keyframe2, img, puntos_pose_tracking, 3, puntos_nuevosK2, indx_OKK2); //aca Lucas usa el Keyframe original

    // Busca los puntos OK
    int puntos_ok = int(indx_OK.size()) + int(indx_OKK1.size()) + int(indx_OKK2.size());
    //std::cout << "Puntos ok: " << puntos_ok << std::endl;
    puntos3d actual_3D_ok = puntos3d(puntos_ok);
    puntos2d actual_2D_ok = puntos2d(puntos_ok);
    // Escribe los puntos 3D y 2D para la estimaciosn de la pose
    int k = 0, i= 0, j = 0;
    for (k = 0; k < int(indx_OK.size()); k++)
    {
        int indice = candide->pose_tracking_indxs->at(indx_OK.at(k));
        actual_3D_ok.at(k) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k) = puntos_nuevos.at(k);
    }
    for (i = 0; i < int(indx_OKK1.size()); i++) // checkear este for aca no deberia ser i < indx_OKK.size() ?
    {
        int indice = candide->pose_tracking_indxs->at(indx_OKK1.at(i));
        actual_3D_ok.at(k + i) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k + i) = puntos_nuevosK1.at(i);
    }
    for (j = 0; j < int(indx_OKK2.size()); j++) // checkear este for aca no deberia ser i < indx_OKK.size() ?
    {

        int indice = candide->pose_tracking_indxs->at(indx_OKK2.at(j));
        actual_3D_ok.at(k + i + j) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k + i + j) = puntos_nuevosK2.at(j);
    }
    //std::cout << "K: " << k << "/ I: " << i << "/ J: " << j << std::endl;


    // Estimo y actualizo la pose
    if (puntos_ok > 5)
    {
        /* --------------------------------- LEVMAR --------------------------------
        cv::Mat Rvector = candide->Rvector.clone();
        cv::Mat Tvector = candide->Tvector.clone();
        ajustar_pose_levmar(&actual_3D_ok, &actual_2D_ok,&Rvector,&Tvector);
        candide->rotartrasladar(Rvector.at<double>(0,0), Rvector.at<double>(0,2), Rvector.at<double>(0,1),
                                Tvector.at<double>(0,0), -Tvector.at<double>(0,1), Tvector.at<double>(0,2)) ;
        candide->actualizar_2D();
        Rvector.release();
        Tvector.release();
        */

        /* -------------------------------- SOLVEPNP ------------------------------- */
        cv::solvePnP(actual_3D_ok, actual_2D_ok, candide->camera_matrix, candide->distCoeffs, candide->Rvector, candide->Tvector);
        candide->actualizar_2D();
        puntos2d puntos_anim = puntos2d();

        // Estimo animacion con LK
        estimar_animacion_lk(prev_img, img, puntos_prev, puntos_anim);

        // Estimo animacion con Landmarks
        //estimar_animacion_landmarks(img, puntos_anim);

        // Puntos de Pose
        //puntos = actual_2D_ok;

        // Puntos de animacion
        puntos = puntos_anim;

        // Todos los puntos - No funca
        //puntos = actual_2D_ok;
        //for(int k = 0 ; int(puntos_anim.size()); k++) puntos.push_back(puntos_anim.at(k));
    }
    // else resetear modelo?
}




struct datos_estimacion_pose{
    std::vector<cv::Point3f>* puntos3D;
    std::vector<cv::Point2f>* puntos2D_proy;
    std::vector<cv::Point2f>* puntos2D_target;
    cv::Mat* Rvector;
    cv::Mat* Tvector;
    cv::Mat* distCoeffs;
    cv::Mat* camera_matrix;
};

void function_fvec_pose(const alglib::real_1d_array &x, alglib::real_1d_array &fi, void *ptr)
{
    datos_estimacion_pose *pose_data = (datos_estimacion_pose*)ptr;
    // Rotacion
    pose_data->Rvector->at<double>(0,0) = x[0];
    pose_data->Rvector->at<double>(0,1) = x[1];
    pose_data->Rvector->at<double>(0,2) = x[2];
    // Traslacion
    pose_data->Tvector->at<double>(0,0) = x[3];
    pose_data->Tvector->at<double>(0,1) = x[4];
    pose_data->Tvector->at<double>(0,2) = x[5];

    // C�lculo de la Proyecci�n
    cv::projectPoints(*pose_data->puntos3D, *pose_data->Rvector, *pose_data->Tvector,
                      *pose_data->camera_matrix, *pose_data->distCoeffs, *pose_data->puntos2D_proy );
    // Funci�n a minimizar
    double norma;
    for (int k = 0; k < int(pose_data->puntos3D->size()); k++)
    {
        norma = cv::norm(pose_data->puntos2D_proy->at(k) - pose_data->puntos2D_target->at(k));
        fi[k] = tukey_M_Estimator(norma, 4);
    }
}


void facetracking::ajustar_pose_levmar(puntos3d* opoints, puntos2d* ipoints, cv::Mat *Rvector, cv::Mat *Tvector){

    alglib::real_1d_array x = "[0,0,0,0,0,0]";

    std::vector<cv::Point2f> *puntos2D_proy = new std::vector<cv::Point2f>();
    datos_estimacion_pose *datos_pose = new datos_estimacion_pose();
    datos_pose->Rvector = Rvector;
    datos_pose->Tvector = Tvector;
    datos_pose->distCoeffs = &candide->distCoeffs;
    datos_pose->camera_matrix = &candide->camera_matrix;
    datos_pose->puntos2D_proy = puntos2D_proy;
    datos_pose->puntos2D_target = ipoints;
    datos_pose->puntos3D = opoints;

    // Rotacion
    x[0] = Rvector->at<double>(0,0);
    x[1] = Rvector->at<double>(0,1);
    x[2] = Rvector->at<double>(0,2);
    // Traslacion
    x[3] = Tvector->at<double>(0,0);
    x[4] = Tvector->at<double>(0,1);
    x[5] = Tvector->at<double>(0,2);

    //double epsg = 1E-8; //gradiente
    //double epsf = 1E-8; //funcion
    double epsx = 1E-9; //parametros
    alglib::ae_int_t maxits = 150;
    alglib::minlmstate state;
    alglib::minlmreport rep;

    alglib::minlmcreatev(6, datos_pose->puntos3D->size(), x, 1E-3, state);
    alglib::minlmsetcond(state, epsx, maxits);
    alglib::minlmoptimize(state, function_fvec_pose, NULL, (void*)datos_pose);
    minlmresults(state, x, rep);

    // Rotacion
    Rvector->at<double>(0,0) = x[0];
    Rvector->at<double>(0,1) = x[1];
    Rvector->at<double>(0,2) = x[2];
    // Traslacion
    Tvector->at<double>(0,0) = x[3];
    Tvector->at<double>(0,1) = x[4];
    Tvector->at<double>(0,2) = x[5];

    free(datos_pose);
    free(puntos2D_proy);
}


// ====================================================================================================
// ------------------------------------------------------------------------------------------ ANIMACION
// ====================================================================================================
void facetracking::estimar_animacion_lk(cv::Mat prev_img, cv::Mat img, puntos2d *puntos_prev, puntos2d &puntos)
{
    cv::Mat keyframe = candide->render_keyframe(); //renderkeyframe con la estimacion de pose P(t)

    // Tracking de la expresion facial
    puntos2d puntos_animation_tracking;
    puntos2d puntos_animation_trackingK;
    std::vector<int> puntos_animation_tracking_indx;

    puntos_animation_tracking.reserve(113);
    puntos_animation_trackingK.reserve(113);
    puntos_animation_tracking_indx.reserve(int(candide->vertices2D->size()));

    // Selecciona los puntos para la estimacion de la animacion
    for(int n=0; n< int(candide->vertices2D->size()); n++){
        if(candide->vertex_class.at(n) == CLASE_ANIMATION || candide->vertex_class.at(n) == CLASE_ANIMATION_CENTRAL ){
            puntos_animation_tracking_indx.push_back(n);
            puntos_animation_tracking.push_back(puntos_prev->at(n));
            puntos_animation_trackingK.push_back(candide->vertices2D->at(n));
        }
    }

    // Opt Flow LK entre Frame(t) y Frame(t-1)
    puntos2d puntos_nuevos = puntos2d();
    vector<int> indx_OK = vector<int>();
    buscar_puntos(prev_img, img, puntos_animation_tracking, 3,  puntos_nuevos, indx_OK, cv::Size(50,50));

    // Opt Flow LK entre Frame(t) y KeyFrame -- vers 1
    puntos2d puntos_nuevosK1 = puntos2d();
    vector<int> indx_OKK1 = vector<int>();
    cv::Mat keyframe1 = candide->get_keyframe();
    buscar_puntos(keyframe1, img, puntos_animation_trackingK, 3, puntos_nuevosK1, indx_OKK1, cv::Size(50,50)); //aca Lucas usa el Keyframe original

    // Opt Flow LK entre Frame(t) y KeyFrame -- vers 2
    puntos2d puntos_nuevosK2 = puntos2d();
    vector<int> indx_OKK2 = vector<int>();
    //cv::Mat keyframe2 = candide->render_keyframe();
    cv::Mat keyframe2 = candide->get_keyframe();
    buscar_puntos(keyframe2, img, puntos_animation_tracking, 3, puntos_nuevosK2, indx_OKK2, cv::Size(50,50)); //aca Lucas usa el Keyframe original

    // Busca los puntos OK
    int puntos_ok = int(indx_OK.size()) + int(indx_OKK1.size()) + int(indx_OKK2.size());
    //std::cout << "Puntos ok: " << puntos_ok << std::endl;
    puntos3d actual_3D_ok = puntos3d(puntos_ok);
    puntos2d actual_2D_ok = puntos2d(puntos_ok);
    // Escribe los puntos 3D y 2D para la estimaciosn de la pose
    int k = 0, i= 0, j = 0;
    for (k = 0; k < int(indx_OK.size()); k++)
    {
        int indice = puntos_animation_tracking_indx.at(indx_OK.at(k));
        actual_3D_ok.at(k) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k) = puntos_nuevos.at(k);
    }
    for (i = 0; i < int(indx_OKK1.size()); i++) // checkear este for aca no deberia ser i < indx_OKK.size() ?
    {
        int indice = puntos_animation_tracking_indx.at(indx_OKK1.at(i));
        actual_3D_ok.at(k + i) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k + i) = puntos_nuevosK1.at(i);
    }
    for (j = 0; j < int(indx_OKK2.size()); j++) // checkear este for aca no deberia ser i < indx_OKK.size() ?
    {

        int indice = puntos_animation_tracking_indx.at(indx_OKK2.at(j));
        actual_3D_ok.at(k + i + j) = candide->vertices3D->at(indice);
        actual_2D_ok.at(k + i + j) = puntos_nuevosK2.at(j);
    }
    //std::cout << "K: " << k << "/ I: " << i << "/ J: " << j << std::endl;


    // Estimo y actualizo la pose
    //_estimador->estimar_bocav2(&puntos_nuevos, &puntos_nuevosK, &indx_OK, &indx_OKK,   modelo->animation_parameters.busq_boca_indx, modelo);
    //modelo->Update_Animation_Parameters();
    if (puntos_ok > 5)
    {
        // --------------------------------- LEVMAR --------------------------------
        ajustar_animacion_levmar(&actual_2D_ok, &puntos_animation_tracking_indx, candide);
        candide->actualizar_2D();
        // -------------------------------- SOLVEPNP -------------------------------
        //cv::solvePnP(actual_3D_ok, actual_2D_ok, candide->camera_matrix, candide->distCoeffs, candide->Rvector, candide->Tvector);
        //candide->actualizar_2D();
        puntos = actual_2D_ok;
    }
    // else resetear modelo?
}


struct datos_estimacion_anim{
    std::vector<cv::Point2f>* puntos2D_target;
    std::vector<int>* point_indxs;
    facialmesh* modelo;
};

void function_fvec_boca(const alglib::real_1d_array &x, alglib::real_1d_array &fi, void *ptr)
{
    datos_estimacion_anim *anim_data = (datos_estimacion_anim*)ptr;
    for (int k=0; k < int(anim_data->modelo->animation_parameters.size()); k++){
         anim_data->modelo->animation_coef.at(k) = x[k];
    }
    anim_data->modelo->actualizar_animation();

    // Funcion a minimizar
    double norma;
    for (int k = 0; k < int(anim_data->point_indxs->size()); k++)
    {
        norma = cv::norm(anim_data->modelo->vertices2D->at(anim_data->point_indxs->at(k)) - anim_data->puntos2D_target->at(k));
        fi[k] = tukey_M_Estimator(norma, 4);
    }
}


void facetracking::ajustar_animacion_levmar(puntos2d *puntos2D_target, std::vector<int>* point_indxs,
                                facialmesh *modelo)
{
    alglib::real_1d_array x; // = "[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]";
    datos_estimacion_anim datos_anim = datos_estimacion_anim();
    datos_anim.modelo = modelo;
    datos_anim.puntos2D_target = puntos2D_target;
    datos_anim.point_indxs = point_indxs;

    x.setlength(datos_anim.modelo->animation_parameters.size());
    for (int k=0; k < int(datos_anim.modelo->animation_parameters.size()); k++){
          x[k] = datos_anim.modelo->animation_coef.at(k);
    }

    double epsx = 1E-10;
    alglib::ae_int_t maxits = 100;
    alglib::minlmstate state;
    alglib::minlmreport rep;

    //alglib::real_1d_array bndl = "[ 0, -1.8, +0.05, -0.8]";
    //alglib::real_1d_array bndu = "[+1, +0.5,  +1.5, +0.8]";
    //alglib::real_1d_array bndl = "[ -2.0, -2.0, -2.0, -2.0]";
    //alglib::real_1d_array bndu = "[+2.0, +2.0,  +2.0, +2.0]";

    alglib::minlmcreatev(datos_anim.modelo->animation_parameters.size(), point_indxs->size(), x, 1E-3, state);
    //alglib::minlmsetbc(state, bndl, bndu);
    alglib::minlmsetcond(state, epsx, maxits);
    alglib::minlmoptimize(state, function_fvec_boca, NULL, (void*)&datos_anim);
    minlmresults(state, x, rep);
    //
    for (int k=0; k < int(datos_anim.modelo->animation_parameters.size()); k++){
        datos_anim.modelo->set_animation_value(k,x[k]);
    }
    //free(datos_boca);
}
