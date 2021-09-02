#include "logfile.h"

logfile::logfile(){
}

void logfile::set_log_filename(string log_filename, bool temp){
    std::size_t lenght;
    lenght = log_filename.length();
    filename = log_filename;
    if(temp == false){
    lenght = filename.length();
    std::size_t found = filename.find_last_of(".");
    if (found <= lenght && found > 0)
        filename = filename.substr(0,found);
    filename += ".log";
    }
    myfile.open (filename);
}


void logfile::set_total_frames(std::size_t tframes){
    total_frames = tframes;
}

void logfile::start_log(facetracking tracker){
    // TODO: Poner hora en la que se genera el archivo
    myfile << "\n# CANT FRAME:" << std::endl;
    myfile << total_frames << std::endl;
    myfile << "\n# CANT ANIMATION:" << std::endl;
    myfile << tracker.candide->animation_parameters.size() << std::endl;
    myfile << "\n# CANT VERTEX:" << std::endl;
    myfile << tracker.candide->vertices3D->size() << std::endl;
    myfile << "\n# CANT PUNTOS:" << std::endl;
    myfile << tracker.candide->vertices2D->size() << std::endl;
    myfile << "\n# CANT SHAPE:" << endl;
    myfile << tracker.candide->shape_parameters.size() << std::endl;

    myfile << "\n# SHAPE:" << endl;
    for(int k=0; k < int(tracker.candide->shape_parameters.size()); k++){
        myfile << tracker.candide->shape_coef[k] << std::endl;
    }
}

void logfile::write_log(facetracking tracker, int frame){
    myfile << "\n@ FRAME: " << frame << std::endl;
    myfile << "\n# VERTEX:" << std::endl;
    for(int k=0; k < int(tracker.candide->vertices3D->size()); k++){ // Despues dejar solamente los vertices que nos sirven para las mediciones
        myfile << "" << tracker.candide->vertices3D->at(k).x << " " << tracker.candide->vertices3D->at(k).y << " " << tracker.candide->vertices3D->at(k).z << " " << std::endl;
    }
    myfile << "\n# PUNTOS:" << std::endl;
    for(int k=0; k < int(tracker.candide->vertices2D->size()); k++){ // Despues dejar solamente los vertices que nos sirven para las mediciones
        myfile << "" << tracker.candide->vertices2D->at(k).x << " " << tracker.candide->vertices2D->at(k).y << " " <<  std::endl;
    }
    myfile << "\n# POSE:" << std::endl;
    myfile << tracker.candide->Tvector.at<double>(0,0) << " " << tracker.candide->Tvector.at<double>(0,1) << " " << tracker.candide->Tvector.at<double>(0,2)<<std::endl;
    myfile << tracker.candide->Rvector.at<double>(0,0) << " " << tracker.candide->Rvector.at<double>(0,1) << " " << tracker.candide->Rvector.at<double>(0,2)<<std::endl;
    myfile << "\n# ANIMATION:" << std::endl;
    for(int k=0; k < int(tracker.candide->animation_parameters.size()); k++){
        myfile << tracker.candide->animation_coef.at(k) << std::endl;
    }
}

void logfile::end_log(){
    myfile << "\n# END" << endl;
    myfile.close();
}

datalog::datalog(){
}

void datalog::load_data(string filename){

    ifstream file;
    string line;
    vector<string> linedata_tmp;
    std::size_t cant_frames, cant_animation, cant_vertex, cant_puntos, cant_shape;

    file.open(filename);
    if (!file.is_open()) {
        std::printf("\nNo se puedo abrir el archivo");
        return;
    }
    std::cout << "----> Se abrio log " << filename << std::endl;
    std::getline(file, line);

    while (!(string::npos != line.find("# END")))
    {

        if (string::npos != line.find("# CANT FRAME:")) {
            std::getline(file, line);
            cant_frames = atoi(line.c_str());
            trackingdata.reserve(cant_frames);
        }

        if (string::npos != line.find("# CANT ANIMATION:")) {
            std::getline(file, line);
            cant_animation = atoi(line.c_str());
        }

        if (string::npos != line.find("# CANT VERTEX:")) {
            std::getline(file, line);
            cant_vertex = atoi(line.c_str());
        }

        if (string::npos != line.find("# CANT PUNTOS:")) {
            std::getline(file, line);
            cant_puntos = atoi(line.c_str());
        }

        if (string::npos != line.find("# CANT SHAPE:")) {
            std::getline(file, line);
            cant_shape = atoi(line.c_str());
            shape_coef.reserve(cant_shape);
        }

        if (string::npos != line.find("# SHAPE:")){
            for (int k = 0; k < int(cant_shape); k++) {
                std::getline(file, line);
                shape_coef.push_back(str2float(line.c_str()));
            }
        }

        if (string::npos != line.find("@ FRAME:")){
            std::size_t found = line.find_last_of(":");
            frame frame;
            frame.frame_number = str2float(line.substr(found+1).c_str());
            frame.animation_coef = values(cant_animation);
            frame.vertices = puntos3d(cant_vertex);
            frame.puntos = puntos2d(cant_puntos);

            for(int n=0; n< 4; n++){

                while (string::npos == line.find("# ")) std::getline(file, line);

                if (string::npos != line.find("# ANIMATION:")){

                    for (int k = 0; k < int(cant_animation); k++) {
                        std::getline(file, line);
                        frame.animation_coef.at(k) =str2float(line.c_str());
                    }
                }

                if (string::npos != line.find("# POSE:")){
                    std::getline(file, line);
                    linedata_tmp.clear();
                    split_line(line, ' ', linedata_tmp);
                    frame.traslacion.x = str2float(linedata_tmp.at(0));
                    frame.traslacion.y = str2float(linedata_tmp.at(1));
                    frame.traslacion.z = str2float(linedata_tmp.at(2));

                    std::getline(file, line);
                    linedata_tmp.clear();
                    split_line(line, ' ', linedata_tmp);
                    frame.rotacion.x = str2float(linedata_tmp.at(0));
                    frame.rotacion.y = str2float(linedata_tmp.at(1));
                    frame.rotacion.z = str2float(linedata_tmp.at(2));

                }

                if (string::npos != line.find("# VERTEX:")){
                    for (int k = 0; k < int(cant_vertex); k++) {
                        punto3d vertex_temp;
                        std::getline(file, line);
                        linedata_tmp.clear();
                        split_line(line, ' ', linedata_tmp);
                        vertex_temp.x = str2float(linedata_tmp.at(0));
                        vertex_temp.y = str2float(linedata_tmp.at(1));
                        vertex_temp.z = str2float(linedata_tmp.at(2));
                        frame.vertices.at(k) = vertex_temp;
                    }
                }

                if (string::npos != line.find("# PUNTOS:")){
                    for (int k = 0; k < int(cant_puntos); k++) {
                        punto2d punto_temp;
                        std::getline(file, line);
                        linedata_tmp.clear();
                        split_line(line, ' ', linedata_tmp);
                        punto_temp.x = str2float(linedata_tmp.at(0));
                        punto_temp.y = str2float(linedata_tmp.at(1));
                        frame.puntos.at(k) = punto_temp;
                    }
                }

            }
            trackingdata.push_back(frame);
        }
       std::getline(file,line);

    }
    std::cout << "Log cargado con exito " << std::endl;
    file.close();
    if(file.is_open()){
        std::cout << "El archivo sigue abierto" << std::endl;
    }
}


float datalog::str2float(string s)
{
    size_t found = s.find(","); // indx donde esta la coma
    if (found != std::string::npos)
        s[found] = '.'; // Change ',' to '.'

    return std::atof(s.c_str());
}

void datalog::split_line(const string& s, char delim, vector<string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (!item.empty())
            elems.push_back(item);
    }
}
