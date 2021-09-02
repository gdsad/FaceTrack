#pragma once
#ifndef LOGFILE_H
#define LOGFILE_H

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include "facetracking.h"


typedef struct frame {
    int frame_number;
    puntos3d vertices ;
    puntos2d puntos;
    punto3d traslacion;
    punto3d rotacion;
    values animation_coef;
    frame(){
     frame_number = 0;
     vertices = puntos3d(113);
     puntos = puntos2d(113);
     animation_coef = values(30);
    }
    inline frame(int _frame_number, puntos3d _vertices, punto3d _traslacion,  punto3d _rotacion, values _animation_coef)
        : frame_number(_frame_number)
        , vertices(_vertices)
        , traslacion(_traslacion)
        , rotacion(_rotacion)
        , animation_coef(_animation_coef){};
} frame;

typedef vector<frame> frames;


class logfile {

public:
    // Constructor
    logfile();
    void set_log_filename(string log_filename, bool temp);
    void set_total_frames(std::size_t tframes);
    void start_log(facetracking tracker);
    void write_log(facetracking tracker, int frame);
    void end_log();

    string filename;
    ofstream myfile;
    std::size_t total_frames;

};


class datalog {

public:
    //Constructor
    datalog();

    //Metodos
    void load_data(string filename);
    float str2float(string s);
    void split_line(const string& s, char delim, vector<string>& elems);

    // Atributos
    values shape_coef;
    frames trackingdata;


};


#endif // LOGFILE_H
