#include "facetracking.h"

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

void facetracking::facelandmarks(cv::Mat img, cv::Rect face, bool &detect) {

    std::vector< std::vector<cv::Point2f> > face_landmarks;
    landmarks.clear();
    landmarks.reserve(68);
    detect = 0;
    std::vector<cv::Rect > face_vector;
    face_vector.push_back(face);

    // Run landmark detector
    bool success = facemark->fit(img, face_vector, face_landmarks);
    if (success && face_landmarks[0].size() == 68) {
            detect = 1;
            for (int k = 0; k < 68; k++)
                landmarks.push_back(face_landmarks[0][k]);
        }
}



cv::Mat facetracking::drawlandmarks(cv::Mat img) {
    cv::Mat image = img.clone();
    for (int k = 0; k < int(landmarks.size()); k++) {
        cv::circle(image, landmarks[k], 1, cv::Scalar((0), (0), (255), 0), 3);
    }
    return image;
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
        // Verifica si el punto fue encontrado (para utilizarlo en la estimacion de la pose)
        if (status.at(i) == 0)
            continue;
        puntos_nuevos.at(k) = curr_points.at(i);
        indx_OK.at(k) = i;
        k++;
    }
}

void facetracking::buscar_puntosV2(cv::Mat prev_img, cv::Mat curr_img, puntos2d prev_points, int level,
                                puntos2d *puntos_nuevos, cv::Size winsize, values* delta)
{
    std::vector<uchar> status = std::vector<uchar>();
    std::vector<float> error = std::vector<float>();
    puntos2d curr_points;
    //cv::calcOpticalFlowPyrLK(prev_img, curr_img, prev_points, curr_points, status, error, winsize, level);

    // Rodrigo - para probar
    cv::calcOpticalFlowPyrLK(
                prev_img,
                curr_img,
                prev_points,
                curr_points,
                status,
                error,
                winsize, //cv::Size(25, 25), con 35 funciona bien
                level,
                cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 60, 0.001),
                cv::OPTFLOW_LK_GET_MIN_EIGENVALS,
                1e-5
            );

    // Cuenta la cantidad de puntos OK
    puntos_nuevos->resize(status.size());
    delta->reserve(status.size());
    int puntos_ok = 0;
    for (int indx = 0; indx < int(status.size()); indx++)
    {
        if ((status.at(indx) == 1) && (error.at(indx) <5))
        {
            puntos_ok++;
            puntos_nuevos->at(indx) = curr_points.at(indx);
            delta->at(indx) = std::sqrt(pow((prev_points.at(indx).x - curr_points.at(indx).x),2) + pow((prev_points.at(indx).y - curr_points.at(indx).y), 2));
        }
        else
        {
            status.at(indx) = 0;
            curr_points.at(indx) = punto2d(0.0,0.0);
            delta->at(indx) = 0.0;
        }
    }
}


void facetracking::step(cv::Mat prev_img, cv::Mat img){

    // ######################################################################################################
    // ============================================================================> Defs y Pre-Procesamiento
    // ######################################################################################################
    cv::Mat prev_img_gs, img_gs; // img_hsv, mascara_boca;
    grayscale(prev_img, prev_img_gs);
    grayscale(img, img_gs);

    //cv::Mat rot_orig = candide->Rvector.clone();
    //cv::Mat tras_orig = candide->Tvector.clone();

    puntos2d puntos_prev(candide->vertices2D->size());
    for (int k = 0; k < int(candide->vertices2D->size()); k++)
        puntos_prev.at(k) = candide->vertices2D->at(k);

    // ######################################################################################################
    // ===============================================================================> Estimacion de la pose
    // ######################################################################################################

    // Puntos para tracking de pose en (t-1)
    puntos2d puntos_pose_tracking(candide->pose_tracking_indxs->size());
    puntos2d puntos_pose_tracking_k(candide->pose_tracking_indxs->size());
    for (int k = 0; k < int(candide->pose_tracking_indxs->size()); k++)
    {
        puntos_pose_tracking.at(k)   = candide->vertices2D->at(candide->pose_tracking_indxs->at(k));
        puntos_pose_tracking_k.at(k) = candide->vertices2D_keyframe->at(candide->pose_tracking_indxs->at(k));
    }

    // Optical Flow entre Frame(t) y KeyFrame Renderizado(t-1)
    puntos2d kf_of(candide->vertices2D->size());
    float kf_error = 0;
    cv::Mat rendered_keyframe = candide->render_keyframe();
    //cv::Mat rendered_keyframe = candide->render_keyframe_v2(prev_img);
    grayscale(rendered_keyframe, rendered_keyframe);
    estimar_pose(rendered_keyframe,img_gs, puntos_pose_tracking, &kf_of, &kf_error);
    cv::Mat kf_r = candide->Rvector.clone();
    cv::Mat kf_t = candide->Tvector.clone();
    // Vuelvo a poner Candide como estaba antes
    //candide->Rvector = rot_orig.clone();
    //candide->Tvector = tras_orig.clone();
    //candide->actualizar_2D();

    // Optical Flow entre Frame(t) y Frame(t-1)
    puntos2d ff_of(candide->vertices2D->size());
    float ff_error = 0;
    estimar_pose(prev_img_gs,img_gs, puntos_pose_tracking, &ff_of, &ff_error);
    cv::Mat ff_r = candide->Rvector.clone();
    cv::Mat ff_t = candide->Tvector.clone();
    // Vuelvo a poner Candide como estaba antes
    //candide->Rvector = rot_orig.clone();
    //candide->Tvector = tras_orig.clone();
    //candide->actualizar_2D();

    // Optical Flow entre Frame(t) y KeyFrame Original(t0)
    puntos2d rf_of(candide->vertices2D->size());
    float rf_error =0;
    cv::Mat original_keyframe = candide->get_keyframe();
    grayscale(original_keyframe, original_keyframe);
    estimar_pose(original_keyframe,img_gs, puntos_pose_tracking_k, &rf_of, &rf_error);
    cv::Mat rf_r = candide->Rvector.clone();
    cv::Mat rf_t = candide->Tvector.clone();



    // Me quedo con la estimacion de pose que menos difiera de los puntos estimados con Opt Flow
    if (ff_error < kf_error && ff_error < rf_error)
    {
        candide->Rvector = ff_r;
        candide->Tvector = ff_t;
    }
    else if (kf_error < ff_error && kf_error < rf_error)
    {
        candide->Rvector = kf_r;
        candide->Tvector = kf_t;
    }
    else
    {
        candide->Rvector = rf_r;
        candide->Tvector = rf_t;
    }

    // ######################################################################################################
    // ==========================================================================> Estimacion de la animacion
    // ######################################################################################################

    values anim_coefs;
    anim_coefs.resize(candide->animation_coef.size());
    for (int k = 0; k < int(candide->animation_coef.size()); k++)
    {
        anim_coefs.at(k) = candide->animation_coef.at(k);
    }

    bool detected = false;
    cv::Rect face = detectFace(img, detected);
    if (detected)
    {
        detected = false;
        punto2d aux;
        puntos2d last_landmarks;
        stringstream stream;

        last_landmarks.resize(landmarks.size());
        for (int k = 0; k < int(landmarks.size()); k++) {
            last_landmarks.at(k) = landmarks.at(k);
        }

        facelandmarks(img, face, detected);
        if (detected)
        {
            // Obtengo los deltas de los landmarks
            //delta_raw_landmarks.resize(landmarks.size());
            //delta_stb_landmarks.resize(stabilized_landmarks.size());
            delta_raw_stb.resize(stabilized_landmarks.size());
            //text_on_image.resize(stabilized_landmarks.size());

            for (int k = 0; k < int(stabilized_landmarks.size()); k++) {
                //aux = stabilized_landmarks.at(k).GetPoint();
                stabilized_landmarks.at(k).Update(landmarks.at(k));

                //delta_raw_landmarks.at(k) = std::sqrt(pow((last_landmarks.at(k).x - landmarks.at(k).x), 2) + pow((last_landmarks.at(k).y - landmarks.at(k).y), 2));
                //delta_stb_landmarks.at(k) = std::sqrt(pow((aux.x - stabilized_landmarks.at(k).GetPoint().x), 2) + pow((aux.y - stabilized_landmarks.at(k).GetPoint().y), 2));
                delta_raw_stb.at(k) = std::sqrt(pow((landmarks.at(k).x - stabilized_landmarks.at(k).GetPoint().x), 2) + pow((landmarks.at(k).y - stabilized_landmarks.at(k).GetPoint().y), 2));

                //stream << k << "    |    " << delta_raw_landmarks.at(k) << "    |    " << delta_stb_landmarks.at(k) << "    |    " << delta_raw_stb.at(k);
                //text_on_image.at(k) = stream.str() ;
                //stream.str(std::string());
            }


            // Opt. Flow / LandMarks - LevMarq entre Frame(t) y Frame(t-1) // ff_ac (=frame animation coefs vector)
            values ff_ac, ff_a_error;
            ff_ac.resize(candide->animation_coef.size());
            ff_a_error.resize(candide->animation_coef.size());
            estimar_animacion(prev_img_gs, img_gs, puntos_prev, anim_coefs, &ff_ac, &ff_a_error);

            for (int k = 0; k < int(candide->animation_coef.size()); k++)
            {
                candide->animation_coef.at(k) = ff_ac.at(k);
            }

            candide->actualizar_animation();
        }
    }
}

// ====================================================================================================
// ----------------------------------------------------------------------------------------------- POSE
// ====================================================================================================

void facetracking::estimar_pose(cv::Mat prev_img, cv::Mat img, puntos2d puntos_pose, puntos2d *puntos,  float *error)
{

    // =============================== Estimo P(t) ==================================
    // ___________________________OBTENGO OPTICAL FLOW LK____________________________
    puntos2d puntos_nuevos = puntos2d();
    values delta = values(puntos_pose.size());
    buscar_puntosV2(prev_img, img, puntos_pose, 3,  &puntos_nuevos, cv::Size(40,40), &delta); // Antes estaba en 20,20
    puntos3d actual_3D_ok = puntos3d(puntos_pose.size());
    puntos2d actual_2D_ok = puntos2d(puntos_pose.size());

    // Escribe los puntos 3D y 2D para la estimacion de la pose
    for (int k = 0; k < int(puntos_pose.size()); k++)
    {
        if(puntos_nuevos.at(k) != punto2d(0.0,0.0)){
            actual_3D_ok.at(k)= candide->vertices3D->at(candide->pose_tracking_indxs->at(k));
            actual_2D_ok.at(k) = puntos_nuevos.at(k);
        }
        else
        {
            actual_3D_ok.at(k)= candide->vertices3D->at(candide->pose_tracking_indxs->at(k));
            actual_2D_ok.at(k) = candide->vertices2D->at(candide->pose_tracking_indxs->at(k));
        }
    }

    // Estimo y actualizo la pose
    try
      {
        /* -------------------------------- SOLVEPNP ------------------------------- */
        *error = 1000.0;
        if(actual_2D_ok.size() >= 6)
        {
            cv::solvePnP(actual_3D_ok, actual_2D_ok, candide->camera_matrix, candide->distCoeffs, candide->Rvector, candide->Tvector);
            candide->actualizar_2D();
            *error = 0;
        }
        int n=0;
        *puntos = puntos2d(candide->pose_tracking_indxs->size());
        for (int k = 0; k < int(candide->pose_tracking_indxs->size()); k++){
           puntos->at(k) = candide->vertices2D->at(candide->pose_tracking_indxs->at(k));
           if(puntos_nuevos.at(k) != punto2d(0.0,0.0))
                *error += cv::norm(puntos->at(k) - puntos_nuevos.at(k));
                n++;
        }
        if(n>(int(candide->pose_tracking_indxs->size())/2) && *error!=0)
            *error= *error / int(candide->pose_tracking_indxs->size());
        else
            *error=1000.0;

      }
    catch (exception& e)
      {
        *error = 1000.0;
        cout << "Standard exception: " << e.what() << endl;
      }
}


// ====================================================================================================
// ------------------------------------------------------------------------------------------ ANIMACION
// ====================================================================================================

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
        fi[k] = norma;
    }
}

void facetracking::estimar_animacion(cv::Mat prev_img, cv::Mat img, puntos2d puntos_prev, values coef_prev, values *anim_coefs, values *au_error){

    puntos2d puntos_animation_tracking;
    std::vector<int> puntos_animation_tracking_indx;

    puntos_animation_tracking.reserve(int(candide->vertices2D->size()));
    puntos_animation_tracking_indx.reserve(int(candide->vertices2D->size()));
    std::vector<int> facemark_indx_candide = { 29,0,0,0,30,0,0,32,10,65,0,0,63,61,0,0,62,15,0,0,0,17,50,0,0,0,48,0,94,0,0,0,111,0,112,0,20,67,71,23,72,68,56,73,69,53,70,74,31,79,33,7,66,80,64,86,0,8,0,85,88,81,87,82,89,84,40,83 };

    // Selecciona los puntos para la estimacion de la animacion
    for(int n=0; n< int(candide->vertices2D->size()); n++)
    {
        if(candide->vertex_class.at(n) == CLASE_ANIMATION || candide->vertex_class.at(n) == CLASE_ANIMATION_CENTRAL )
        {
            puntos_animation_tracking_indx.push_back(n);
            puntos_animation_tracking.push_back(puntos_prev.at(n));
        }
    }

    puntos2d puntos_nuevos = puntos2d();
    vector<int> indx_OK = vector<int>();
    values delta = values(puntos_prev.size());

    buscar_puntosV2(prev_img, img, puntos_prev, 3,  &puntos_nuevos, cv::Size(40,40), &delta); // 20,20

    puntos2d actual_2D_ok = puntos2d(int(puntos_prev.size()));
    float max_delta = 1.0;

    // Escribe los puntos 2D para la estimacion de la animacion
    for (int k=0; k < int(puntos_animation_tracking_indx.size()); k++)
    {
        actual_2D_ok.at(k) = candide->vertices2D->at(puntos_animation_tracking_indx.at(k));

        if (puntos_nuevos.at(puntos_animation_tracking_indx.at(k)) != punto2d(0.0, 0.0))
        {
            actual_2D_ok.at(k) = puntos_nuevos.at(puntos_animation_tracking_indx.at(k));
        }

        for (int i = 0; i< int(facemark_indx_candide.size()); i++)
        {
            if (puntos_animation_tracking_indx.at(k) == facemark_indx_candide.at(i) &&
                facemark_indx_candide.at(i) != 0)
            {
                actual_2D_ok.at(k) = stabilized_landmarks.at(i).GetPoint();
                if (delta_raw_stb.at(i) > 10.0) //  if (delta.at(puntos_animation_tracking_indx.at(k)) < max_delta)
                {
                    //actual_2D_ok.at(k) =  landmarks.at(i);
                    actual_2D_ok.at(k).x = (landmarks.at(i).x * 0.4 + stabilized_landmarks.at(i).GetPoint().x * 0.6) ;
                    actual_2D_ok.at(k).y = (landmarks.at(i).y * 0.4 + stabilized_landmarks.at(i).GetPoint().y * 0.6) ;
                }
            }

            // ==========> Casos particulares
            // Correccion de cejas
            if (puntos_animation_tracking_indx.at(k) == 16 || puntos_animation_tracking_indx.at(k) == 18) {
                actual_2D_ok.at(k) = (stabilized_landmarks.at(18).GetPoint() + stabilized_landmarks.at(19).GetPoint()) / 2;
            }
            if (puntos_animation_tracking_indx.at(k) == 49 || puntos_animation_tracking_indx.at(k) == 51) {
                actual_2D_ok.at(k) = (stabilized_landmarks.at(24).GetPoint() + stabilized_landmarks.at(25).GetPoint()) / 2;
            }

            // Ojos
            if (puntos_animation_tracking_indx.at(k) == 21) {
                actual_2D_ok.at(k) = (stabilized_landmarks.at(37).GetPoint() + stabilized_landmarks.at(38).GetPoint()) / 2;
            }
            if (puntos_animation_tracking_indx.at(k) == 22) {
                actual_2D_ok.at(k) = (stabilized_landmarks.at(40).GetPoint() + stabilized_landmarks.at(41).GetPoint()) / 2;
            }
            if (puntos_animation_tracking_indx.at(k) == 54) {
                actual_2D_ok.at(k) = (stabilized_landmarks.at(43).GetPoint() + stabilized_landmarks.at(44).GetPoint()) / 2;
            }
            if (puntos_animation_tracking_indx.at(k) == 55) {
                actual_2D_ok.at(k) = (stabilized_landmarks.at(46).GetPoint() + stabilized_landmarks.at(47).GetPoint()) / 2;
            }

           if(candide->animation_coef.at(4) < -0.15 && candide->animation_coef.at(5) < -0.15 && candide->animation_coef.at(12) < 0.2){
               std::set<int> boca_indxs = {7,8,31,33,40,64,66,79,80,81,82,83,84,85,86,87,88,89};
               if (puntos_animation_tracking_indx.at(k) == facemark_indx_candide.at(i) &&
                   facemark_indx_candide.at(i) != 0 && boca_indxs.find(puntos_animation_tracking_indx.at(k)) != boca_indxs.end()){

                   //std::cout << "Land :" << candide->animation_coef.at(4) << " " << candide->animation_coef.at(5) << "  Distancia: " <<
                   //             sqrt(pow(abs(puntos_nuevos.at(puntos_animation_tracking_indx.at(k)).x - stabilized_landmarks.at(i).GetPoint().x),2) +
                   //             pow(abs(puntos_nuevos.at(puntos_animation_tracking_indx.at(k)).y - stabilized_landmarks.at(i).GetPoint().y),2)) << std::endl;

                   if(sqrt(pow(abs(puntos_nuevos.at(puntos_animation_tracking_indx.at(k)).x - stabilized_landmarks.at(i).GetPoint().x),2) +
                           pow(abs(puntos_nuevos.at(puntos_animation_tracking_indx.at(k)).y - stabilized_landmarks.at(i).GetPoint().y),2)) < 15){
                        actual_2D_ok.at(k) = puntos_nuevos.at(puntos_animation_tracking_indx.at(k));
                        stabilized_landmarks.at(i).Update(puntos_nuevos.at(puntos_animation_tracking_indx.at(k)));
                    }
                }
            }
        }


    }

    // Estimo la animacion
    try
      {
        /* -------------------------------- LEVMARQ ------------------------------- */
        ajustar_animacion_levmar(&actual_2D_ok, &puntos_animation_tracking_indx, candide);
        candide->actualizar_animation();
        for(int k=0; k < int(candide->animation_coef.size()); k++)
        {
            anim_coefs->at(k) = candide->animation_coef.at(k);
            au_error->at(k) = 0;
            int n = 0;
            for (int j=0; j< int(candide->animation_parameters.at(k).matriz.size()); j++)
            {
                if(puntos_nuevos.at(candide->animation_parameters.at(k).matriz.at(j).i) != punto2d(0.0,0.0))
                {
                     au_error->at(k) += cv::norm(candide->vertices2D->at(candide->animation_parameters.at(k).matriz.at(j).i) - puntos_nuevos.at(candide->animation_parameters.at(k).matriz.at(j).i));
                     n++;
                }
            }

            if(au_error->at(k) != 0)
                au_error->at(k)= au_error->at(k) / n;
            else
                au_error->at(k)= 1000.0;
        }
        // Vuelvo a poner la mascara como estaba antes
        for (int k = 0; k < int(candide->animation_coef.size()); k++)
            candide->animation_coef.at(k) = coef_prev.at(k);
        candide->actualizar_animation();
      }
    catch (exception& e)
      {
        cout << "Standard exception: " << e.what() << endl;
      }
}


void facetracking::ajustar_animacion_levmar(puntos2d *puntos2D_target, std::vector<int>* point_indxs,
                                facialmesh *modelo)
{
    alglib::real_1d_array x;
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

    // Limites de los coeficientes para cada AUV adaptado a candide3_v3
    //                              0      1     2     3     4     5     6     7     8     9     10    11    12    13     14    15    16    17   18   19   20   21   22   23   24
    alglib::real_1d_array bndl = "[ 0.0,  0.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0,  0.0,  -0.8,  -0.8, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ]";
    alglib::real_1d_array bndu = "[ 1.0,  1.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,  2.0,   1.5,   1.5,  2.0,  2.0, 1.5, 1.5, 1.5, 1.5, 0.0, 0.0, 2.0, 2.0 ]";

    alglib::minlmcreatev(datos_anim.modelo->animation_parameters.size(), point_indxs->size(), x, 1E-3, state);
    alglib::minlmsetbc(state, bndl, bndu);
    alglib::minlmsetcond(state, epsx, maxits);
    alglib::minlmoptimize(state, function_fvec_boca, NULL, (void*)&datos_anim);
    minlmresults(state, x, rep);

    for (int k=0; k < int(datos_anim.modelo->animation_parameters.size()); k++){
        datos_anim.modelo->set_animation_value(k,x[k]);
    }
}


