#include "results.h"
#include "ui_results.h"

bool Results::eventFilter(QObject *object, QEvent *ev)
{
    if(ui->tab_graficas->currentIndex() == 0)
    {
        // Grafica Distancia Ojo - Boca
        if ((ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseMove) && object->objectName() == "widget_2")
        {
            QMouseEvent* mouseEvent = (QMouseEvent*)ev;
            auto const widgetPos = mouseEvent->localPos();
            auto const scenePos = ui->DistOjoBocaGraph->mapToScene(QPoint(static_cast<int>(widgetPos.x()), static_cast<int>(widgetPos.y())));
            auto const chartItemPos = GraphDistOjoBoca->mapFromScene(scenePos);
            auto const valueGivenSeries = GraphDistOjoBoca->mapToValue(chartItemPos);

            if (valueGivenSeries.x() >= trackingdata.trackingdata.at(0).frame_number && valueGivenSeries.x() <= cap.get(cv::CAP_PROP_FRAME_COUNT))
            {
                lineaVertical1->removePoints(0,2);
                lineaVertical1->append(valueGivenSeries.x(),90);
                lineaVertical1->append(valueGivenSeries.x(),-90);
                cv::Mat frame;
                cap.set(cv::CAP_PROP_POS_FRAMES,valueGivenSeries.x() - 1);
                cap.read(frame);
                QImage imgQ = MatToQImage(GetFittedImage(frame,241,411));
                ui->img_label_2->setPixmap(QPixmap::fromImage(imgQ));
                ui->tablaOjoBoca->item(0,0)->setText(QString::number(distOjoBocaL->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaOjoBoca->item(0,1)->setText(QString::number(distOjoBocaR->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->nro_frame_dist_ojoboca->setText("FRAME:" + QString::number(int(valueGivenSeries.x())).rightJustified(max_frame, QChar(0x2007)) + " | " + QString::number(int(valueGivenSeries.x())*(1.0/cap.get(cv::CAP_PROP_FPS)),'f',2).rightJustified(max_seconds, QChar(0x2007)) + "seg");
            }
        }
    }

    if(ui->tab_graficas->currentIndex() == 1)
    {
        // Grafica Distancia Parpados GraphDistOjoSupInf
        if ((ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseMove) && object->objectName() == "widget_3")
        {
            QMouseEvent* mouseEvent = (QMouseEvent*)ev;
            auto const widgetPos = mouseEvent->localPos();
            auto const scenePos = ui->DistOjoGraph->mapToScene(QPoint(static_cast<int>(widgetPos.x()), static_cast<int>(widgetPos.y())));
            auto const chartItemPos = GraphDistOjoSupInf->mapFromScene(scenePos);
            auto const valueGivenSeries = GraphDistOjoSupInf->mapToValue(chartItemPos);

            if (valueGivenSeries.x() >= trackingdata.trackingdata.at(0).frame_number && valueGivenSeries.x() <= cap.get(cv::CAP_PROP_FRAME_COUNT))
            {
                lineaVertical2->removePoints(0,2);
                lineaVertical2->append(valueGivenSeries.x(),90);
                lineaVertical2->append(valueGivenSeries.x(),-90);
                cv::Mat frame;
                cap.set(cv::CAP_PROP_POS_FRAMES,valueGivenSeries.x() - 1);
                cap.read(frame);
                QImage imgQ = MatToQImage(GetFittedImage(frame,241,411));
                ui->img_label_3->setPixmap(QPixmap::fromImage(imgQ));                
                ui->tablaOjoSupInf->item(0,0)->setText(QString::number(distOjoSupInfL->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaOjoSupInf->item(0,1)->setText(QString::number(distOjoSupInfR->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->nro_frame_dist_ojo->setText("FRAME:" + QString::number(int(valueGivenSeries.x())).rightJustified(max_frame, QChar(0x2007)) + " | " + QString::number(int(valueGivenSeries.x())*(1.0/cap.get(cv::CAP_PROP_FPS)),'f',2).rightJustified(max_seconds, QChar(0x2007)) + "seg");
            }
        }
    }

    if(ui->tab_graficas->currentIndex() == 2)
    {
        // Grafica Gestos
        if ((ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseMove) && object->objectName() == "widget")
        {
            QMouseEvent* mouseEvent = (QMouseEvent*)ev;
            auto const widgetPos = mouseEvent->localPos();
            auto const scenePos = ui->GestosGraph->mapToScene(QPoint(static_cast<int>(widgetPos.x()), static_cast<int>(widgetPos.y())));
            auto const chartItemPos = GraphGestos->mapFromScene(scenePos);
            auto const valueGivenSeries = GraphGestos->mapToValue(chartItemPos);

            if (valueGivenSeries.x() >= trackingdata.trackingdata.at(0).frame_number && valueGivenSeries.x() <= cap.get(cv::CAP_PROP_FRAME_COUNT))
            {
                lineaVertical->removePoints(0,2);
                lineaVertical->append(valueGivenSeries.x(),90);
                lineaVertical->append(valueGivenSeries.x(),-90);
                cv::Mat frame;
                cap.set(cv::CAP_PROP_POS_FRAMES,valueGivenSeries.x() - 1);
                cap.read(frame);
                QImage imgQ = MatToQImage(GetFittedImage(frame,241,411));
                ui->img_label->setPixmap(QPixmap::fromImage(imgQ));

                ui->tablaGestos->item(0,0)->setText(QString::number(gestos.at(2)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(0,1)->setText(QString::number(gestos.at(3)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(1,0)->setText(QString::number(gestos.at(8)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number ).y(),'f',3));
                ui->tablaGestos->item(1,1)->setText(QString::number(gestos.at(9)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(2,0)->setText(QString::number(gestos.at(10)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(2,1)->setText(QString::number(gestos.at(11)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(3,0)->setText(QString::number(gestos.at(4)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(3,1)->setText(QString::number(gestos.at(5)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(4,0)->setText(QString::number(gestos.at(0)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(4,1)->setText(QString::number(gestos.at(1)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(5,0)->setText(QString::number(gestos.at(6)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->tablaGestos->item(5,1)->setText(QString::number(gestos.at(7)->at(valueGivenSeries.x() - trackingdata.trackingdata.at(0).frame_number).y(),'f',3));
                ui->nro_frame->setText("FRAME:" + QString::number(int(valueGivenSeries.x())).rightJustified(max_frame, QChar(0x2007)) + " | " + QString::number(int(valueGivenSeries.x())*(1.0/cap.get(cv::CAP_PROP_FPS)),'f',2).rightJustified(max_seconds, QChar(0x2007)) + "seg");
            }
        }
    }
      return false;
}


Results::Results(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Results)
{
    std::cout << "Inicializando Results..." << std::endl;
    ui->setupUi(this);

    connect(ui->BoxElevarCejasL,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxElevarCejasR,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxFruncirCenioL,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxFruncirCenioR,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxSonrreirL,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxSonrreirR,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxBesoL,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxBesoR,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxCerrarOjoL,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxCerrarOjoR,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxCerrarOjoFueL,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));
    connect(ui->BoxCerrarOjoFueR,SIGNAL(stateChanged(int)),this,SLOT(update_gestos(int)));

    // Scroll para modificar umbrales de los gestos
    connect(ui->scroll_umbral_beso_L, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_beso_R, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_sonreir_L, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_sonreir_R, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_cerrar_ojo_L, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_cerrar_ojo_R, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_elevar_ceja_L, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_elevar_ceja_R, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_fruncir_ceno_L, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_fruncir_ceno_R, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_cerrar_ojo_fuerte_L, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));
    connect(ui->scroll_umbral_cerrar_ojo_fuerte_R, SIGNAL(valueChanged(int)), this, SLOT(grafica_dinamica_gestos()));


    // Seteo nombre de los gestos
    std::vector<QString> names = {"Elevar ceja izquierda", "Elevar ceja derecha", "Fruncir ceño izquierdo", "Fruncir ceño derecho", "Sonreir lado izquierdo", "Sonreir lado derecho",
                                  "Beso lado izquierdo", "Beso lado derecho", "Cerrar ojo izquiedo", "Cerrar ojo derecho", "Cerrar ojo izquiedo fuerte", "Cerrar Ojo derecho fuerte"};
    gestos.resize(12);
    for(int k = 0; k < int(gestos.size()); k ++){
        gestos.at(k) = new QSplineSeries();
        gestos.at(k)->setName(names.at(k));
        qDebug() << "Gesto " << k << ": " << gestos.at(k)->name() ;
    }

    // Seteo rango de scroll para el umbral del gesto
    ui->scroll_umbral_elevar_ceja_L->setRange(0,40);
    ui->scroll_umbral_elevar_ceja_R->setRange(0,40);
    ui->scroll_umbral_fruncir_ceno_L->setRange(0,40);
    ui->scroll_umbral_fruncir_ceno_R->setRange(0,40);
    ui->scroll_umbral_sonreir_L->setRange(0,40);
    ui->scroll_umbral_sonreir_R->setRange(0,40);
    ui->scroll_umbral_beso_L->setRange(0,40);
    ui->scroll_umbral_beso_R->setRange(0,40);
    ui->scroll_umbral_cerrar_ojo_L->setRange(0,40);
    ui->scroll_umbral_cerrar_ojo_R->setRange(0,40);
    ui->scroll_umbral_cerrar_ojo_fuerte_L->setRange(0,40);
    ui->scroll_umbral_cerrar_ojo_fuerte_R->setRange(0,40);

    // Le doy valor inicial nulo al umbral
    ui->scroll_umbral_elevar_ceja_L->setValue(0);
    ui->scroll_umbral_elevar_ceja_R->setValue(0);
    ui->scroll_umbral_fruncir_ceno_L->setValue(0);
    ui->scroll_umbral_fruncir_ceno_R->setValue(0);
    ui->scroll_umbral_sonreir_L->setValue(0);
    ui->scroll_umbral_sonreir_R->setValue(0);
    ui->scroll_umbral_beso_L->setValue(0);
    ui->scroll_umbral_beso_R->setValue(0);
    ui->scroll_umbral_cerrar_ojo_L->setValue(0);
    ui->scroll_umbral_cerrar_ojo_R->setValue(0);
    ui->scroll_umbral_cerrar_ojo_fuerte_L->setValue(0);
    ui->scroll_umbral_cerrar_ojo_fuerte_R->setValue(0);

    // Valor inicial tabla con valores de cada gesto
    ui->tablaGestos->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tablaGestos->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tablaGestos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablaGestos->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablaGestos->setSortingEnabled(false);
    ui->tablaGestos->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tablaGestos->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tablaGestos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablaGestos->setItem(0,0,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(0,1,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(1,0,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(1,1,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(2,0,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(2,1,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(3,0,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(3,1,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(4,0,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(4,1,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(5,0,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaGestos->setItem(5,1,new QTableWidgetItem(QString::number(0,'f',3)));   
    ui->tablaGestos->item(0,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(0,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(1,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(1,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(2,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(2,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(3,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(3,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(4,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(4,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(5,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaGestos->item(5,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    ui->tablaOjoBoca->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tablaOjoBoca->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tablaOjoBoca->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablaOjoBoca->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablaOjoBoca->setSortingEnabled(false);
    ui->tablaOjoBoca->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tablaOjoBoca->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tablaOjoBoca->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablaOjoBoca->setItem(0,0,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaOjoBoca->setItem(0,1,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaOjoBoca->item(0,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaOjoBoca->item(0,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    ui->tablaOjoSupInf->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tablaOjoSupInf->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tablaOjoSupInf->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablaOjoSupInf->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablaOjoSupInf->setSortingEnabled(false);
    ui->tablaOjoSupInf->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tablaOjoSupInf->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tablaOjoSupInf->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablaOjoSupInf->setItem(0,0,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaOjoSupInf->setItem(0,1,new QTableWidgetItem(QString::number(0,'f',3)));
    ui->tablaOjoSupInf->item(0,0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaOjoSupInf->item(0,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Configuracion para capturar el mouse en la grafica de gestos
    ui->widget->installEventFilter(this);
    ui->GestosGraph->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->widget_2->installEventFilter(this);
    ui->DistOjoBocaGraph->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->widget_3->installEventFilter(this);
    ui->DistOjoGraph->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Linea que indica la coordenada en X donde esta el mouse en la grafica
    lineaVertical->setName("");
    lineaVertical->append(0,90);
    lineaVertical->append(0,-90);

    lineaVertical1->setName("");
    lineaVertical1->append(0,90);
    lineaVertical1->append(0,-90);

    lineaVertical2->setName("");
    lineaVertical2->append(0,90);
    lineaVertical2->append(0,-90);

    QPen linea;
    linea.setWidth(1);
    linea.setColor(QColor(255,0,0,250));

    lineaVertical->setPen(linea);
    lineaVertical1->setPen(linea);
    lineaVertical2->setPen(linea);

    GraphGestos->addSeries(lineaVertical);
    GraphDistOjoBoca->addSeries(lineaVertical1);
    GraphDistOjoSupInf->addSeries(lineaVertical2);

    // Inicializo los maximos de todos los gestos
    maxGestos.resize(12);
    for(int k=0;k<12;k++)
        maxGestos.at(k)=-INT_MAX;

    ui->tab_graficas->setCurrentIndex(0);

    // Sombras
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setXOffset(5);
    shadow->setYOffset(5);
    shadow->setColor(QColor(0, 0, 0, 130));
    ui->tab_graficas->setGraphicsEffect(shadow);

    QGraphicsDropShadowEffect *shadow_PDF = new QGraphicsDropShadowEffect();
    shadow_PDF->setBlurRadius(40);
    shadow_PDF->setXOffset(15);
    shadow_PDF->setYOffset(15);
    shadow_PDF->setColor(QColor(0, 0, 0, 130));
    ui->PDF->setGraphicsEffect(shadow_PDF);

    std::cout << "Results inicializado" << std::endl;
}

Results::~Results()
{
    delete ui;
}

void Results::graficar_distancias(string medida)
{
    QString title;
    vector <float> sumDist;
    maxDist.resize(4);
    minDist.resize(4);
    promDist.resize(4);
    sumDist.resize(4);

    float x1,x2,y1,y2,dist,distRef;

    if(medida=="ojoboca"){
        for(int k=0;k<2;k++){
            maxDist.at(k)=-100.0;
            minDist.at(k)=100;
            sumDist.at(k)=0;
        }

        //==============> Distancia Ojo-Boca Lado Izquierdo
        distOjoBocaL->setName("Lado Izquierdo");
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
           distRef=sqrt(((trackingdata.trackingdata.at(k).vertices.at(23).x-trackingdata.trackingdata.at(k).vertices.at(56).x)*(trackingdata.trackingdata.at(k).vertices.at(23).x-trackingdata.trackingdata.at(k).vertices.at(56).x))+((trackingdata.trackingdata.at(k).vertices.at(23).y-trackingdata.trackingdata.at(k).vertices.at(56).y)*(trackingdata.trackingdata.at(k).vertices.at(23).y-trackingdata.trackingdata.at(k).vertices.at(56).y)));

           x1=trackingdata.trackingdata.at(k).vertices.at(53).x;
           x2=trackingdata.trackingdata.at(k).vertices.at(64).x;
           y1=trackingdata.trackingdata.at(k).vertices.at(53).y;
           y2=trackingdata.trackingdata.at(k).vertices.at(64).y;

           dist=sqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2)))/distRef; //Divido la distancia por la distancia entre los ojos para normalizar
           distOjoBocaL->append(trackingdata.trackingdata.at(k).frame_number,dist);
           sumDist.at(1)=sumDist.at(1)+dist;
           if(dist>maxDist.at(1))
           {
               maxDist.at(1)=dist;
           }
           if(dist<minDist.at(1))
           {
               minDist.at(1)=dist;
           }
           if(k==int(trackingdata.trackingdata.size())-1)
           {
               promDist.at(1)=sumDist.at(1)/int(trackingdata.trackingdata.size());
           }
        }
        GraphDistOjoBoca->addSeries(distOjoBocaL);


        //==============> Distancias Ojo-Boca Lado Derecho
        distOjoBocaR->setName("Lado Derecho");
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
           distRef=sqrt(((trackingdata.trackingdata.at(k).vertices.at(23).x-trackingdata.trackingdata.at(k).vertices.at(56).x)*(trackingdata.trackingdata.at(k).vertices.at(23).x-trackingdata.trackingdata.at(k).vertices.at(56).x))+((trackingdata.trackingdata.at(k).vertices.at(23).y-trackingdata.trackingdata.at(k).vertices.at(56).y)*(trackingdata.trackingdata.at(k).vertices.at(23).y-trackingdata.trackingdata.at(k).vertices.at(56).y)));

           x1=trackingdata.trackingdata.at(k).vertices.at(20).x;
           x2=trackingdata.trackingdata.at(k).vertices.at(31).x;
           y1=trackingdata.trackingdata.at(k).vertices.at(20).y;
           y2=trackingdata.trackingdata.at(k).vertices.at(31).y;

           dist=sqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2)))/distRef; //Divido la distancia por la distancia entre los ojos para normalizar
           distOjoBocaR->append(trackingdata.trackingdata.at(k).frame_number,dist);
           sumDist.at(0)=sumDist.at(0)+dist;
           if(dist>maxDist.at(0))
           {
               maxDist.at(0)=dist;
           }
           if(dist<minDist.at(0))
           {
               minDist.at(0)=dist;
           }
           if(k==int(trackingdata.trackingdata.size())-1)
           {
               promDist.at(0)=sumDist.at(0)/int(trackingdata.trackingdata.size());
           }
        }
        GraphDistOjoBoca->addSeries(distOjoBocaR);  //Asigno la variable spline al objeto del grafico


    }else if (medida=="ojo") {
        for(int k=2;k<4;k++){
            maxDist.at(k)=-100.0;
            minDist.at(k)=100;
            sumDist.at(k)=0;
        }

        //==============> Distancia Parpado Sup-Inf Lado Izquierdo
        distOjoSupInfL->setName("Lado Izquierdo");
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
           distRef=sqrt(((trackingdata.trackingdata.at(k).vertices.at(23).x-trackingdata.trackingdata.at(k).vertices.at(56).x)*(trackingdata.trackingdata.at(k).vertices.at(23).x-trackingdata.trackingdata.at(k).vertices.at(56).x))+((trackingdata.trackingdata.at(k).vertices.at(23).y-trackingdata.trackingdata.at(k).vertices.at(56).y)*(trackingdata.trackingdata.at(k).vertices.at(23).y-trackingdata.trackingdata.at(k).vertices.at(56).y)));

           x1=trackingdata.trackingdata.at(k).vertices.at(54).x;
           x2=trackingdata.trackingdata.at(k).vertices.at(55).x;
           y1=trackingdata.trackingdata.at(k).vertices.at(54).y;
           y2=trackingdata.trackingdata.at(k).vertices.at(55).y;

           dist=sqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2)))/distRef; //Divido la distancia por la distancia entre los ojos para normalizar
           distOjoSupInfL->append(trackingdata.trackingdata.at(k).frame_number,dist);
           sumDist.at(3)=sumDist.at(3)+dist;
           if(dist>maxDist.at(3))
           {
               maxDist.at(3)=dist;
           }
           if(dist<minDist.at(3))
           {
               minDist.at(3)=dist;
           }
           if(k==int(trackingdata.trackingdata.size())-1)
           {
               promDist.at(3)=sumDist.at(3)/int(trackingdata.trackingdata.size());
           }
        }
        GraphDistOjoSupInf->addSeries(distOjoSupInfL);

        //==============> Distancia Parpado Sup-Inf Lado Derecho
        distOjoSupInfR->setName("Lado Derecho");
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
           distRef=sqrt(((trackingdata.trackingdata.at(k).vertices.at(23).x-trackingdata.trackingdata.at(k).vertices.at(56).x)*(trackingdata.trackingdata.at(k).vertices.at(23).x-trackingdata.trackingdata.at(k).vertices.at(56).x))+((trackingdata.trackingdata.at(k).vertices.at(23).y-trackingdata.trackingdata.at(k).vertices.at(56).y)*(trackingdata.trackingdata.at(k).vertices.at(23).y-trackingdata.trackingdata.at(k).vertices.at(56).y)));

           x1=trackingdata.trackingdata.at(k).vertices.at(21).x;
           x2=trackingdata.trackingdata.at(k).vertices.at(22).x;
           y1=trackingdata.trackingdata.at(k).vertices.at(21).y;
           y2=trackingdata.trackingdata.at(k).vertices.at(22).y;

           dist=sqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2)))/distRef; //Divido la distancia por la distancia entre los ojos para normalizar
           distOjoSupInfR->append(trackingdata.trackingdata.at(k).frame_number,dist);
           sumDist.at(2)=sumDist.at(2)+dist;
           if(dist>maxDist.at(2))
           {
               maxDist.at(2)=dist;
           }
           if(dist<minDist.at(2))
           {
               minDist.at(2)=dist;
           }
           if(k==int(trackingdata.trackingdata.size())-1)
           {
               promDist.at(2)=sumDist.at(2)/int(trackingdata.trackingdata.size());
           }
        }
        GraphDistOjoSupInf->addSeries(distOjoSupInfR);
    }

    if(medida=="ojoboca"){
        QFont letraTitulo("Arial",16);
        GraphDistOjoBoca->setTitleFont(letraTitulo);

        GraphDistOjoBoca->legend()->setVisible(true);
        GraphDistOjoBoca->legend()->setAlignment(Qt::AlignTop);
        GraphDistOjoBoca->legend()->setBackgroundVisible(false);

        GraphDistOjoBoca->createDefaultAxes();
        GraphDistOjoBoca->axes(Qt::Vertical).first()->setTitleText("Distancia");
        GraphDistOjoBoca->axes(Qt::Horizontal).first()->setTitleText("Cuadro del video");
        GraphDistOjoBoca->axes(Qt::Vertical).first()->setRange(min(minDist.at(0), minDist.at(1)),max(maxDist.at(0), maxDist.at(1)));
        GraphDistOjoBoca->axes(Qt::Horizontal).first()->setRange(trackingdata.trackingdata.at(0).frame_number ,cap.get(cv::CAP_PROP_FRAME_COUNT));
        GraphDistOjoBoca->legend()->markers(lineaVertical1)[0]->setVisible(false);
        title="DISTANCIA OJO - BOCA";
        GraphDistOjoBoca->setTitle(title);
        ui->DistOjoBocaGraph->setChart(GraphDistOjoBoca);
    }
    if(medida=="ojo"){
        //DISEÑO DE GRÁFICAS
        QFont letraTitulo("Arial",16);
        GraphDistOjoSupInf->setTitleFont(letraTitulo);

        GraphDistOjoSupInf->legend()->setVisible(true);
        GraphDistOjoSupInf->legend()->setAlignment(Qt::AlignTop);
        GraphDistOjoSupInf->legend()->setBackgroundVisible(false);

        GraphDistOjoSupInf->createDefaultAxes();
        GraphDistOjoSupInf->axes(Qt::Vertical).first()->setTitleText("Distancia");
        GraphDistOjoSupInf->axes(Qt::Horizontal).first()->setTitleText("Cuadro del video");
        GraphDistOjoSupInf->axes(Qt::Vertical).first()->setRange(min(minDist.at(2), minDist.at(3)),max(maxDist.at(2), maxDist.at(3)));
        GraphDistOjoSupInf->axes(Qt::Horizontal).first()->setRange(trackingdata.trackingdata.at(0).frame_number ,cap.get(cv::CAP_PROP_FRAME_COUNT));
        GraphDistOjoSupInf->legend()->markers(lineaVertical2)[0]->setVisible(false);
        title="DISTANCIA PÁRPADO SUPERIOR - INFERIOR";
        GraphDistOjoSupInf->setTitle(title);
        ui->DistOjoGraph->setChart(GraphDistOjoSupInf);
    }


}

void Results::grafica_dinamica_gestos()
{
    QPen linea;
    linea.setWidth(2);

    if(ui->scroll_umbral_elevar_ceja_L->value()/20.0 != umbral_elevar_ceja_L){
        umbral_elevar_ceja_L = ui->scroll_umbral_elevar_ceja_L->value()/20.0;
        ui->scroll_umbral_elevar_ceja_L->setToolTip(QString::number(umbral_elevar_ceja_L));
        linea.setColor(QColor(200, 120, 255, 250));
        gestos.at(0)->clear();
        gestos.at(0)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = trackingdata.trackingdata.at(k).animation_coef.at(14)+trackingdata.trackingdata.at(k).animation_coef.at(16);

            if( valor >= umbral_elevar_ceja_L){
                gestos.at(0)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(0)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor > maxGestos.at(0)){
                maxGestos.at(0)= valor;
            }
        }
    }

    if(ui->scroll_umbral_elevar_ceja_R->value()/20.0 != umbral_elevar_ceja_R){
        umbral_elevar_ceja_R = ui->scroll_umbral_elevar_ceja_R->value()/20.0;
        ui->scroll_umbral_elevar_ceja_R->setToolTip(QString::number(umbral_elevar_ceja_R));
        linea.setColor(QColor(100, 100, 100, 250));
        gestos.at(1)->clear();
        gestos.at(1)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = trackingdata.trackingdata.at(k).animation_coef.at(13)+trackingdata.trackingdata.at(k).animation_coef.at(15);
            if( valor >= umbral_elevar_ceja_R){
                gestos.at(1)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(1)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor >maxGestos.at(1)){
                maxGestos.at(1)= valor;
            }
        }
    }

    if(ui->scroll_umbral_fruncir_ceno_L->value()/20.0 != umbral_fruncir_ceno_L){
        umbral_fruncir_ceno_L = ui->scroll_umbral_fruncir_ceno_L->value()/20.0;
        ui->scroll_umbral_fruncir_ceno_L->setToolTip(QString::number(umbral_fruncir_ceno_L));
        linea.setColor(QColor(150, 150, 0, 250));
        gestos.at(2)->clear();
        gestos.at(2)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {      
            float valor = -trackingdata.trackingdata.at(k).animation_coef.at(14) - trackingdata.trackingdata.at(k).animation_coef.at(16) + trackingdata.trackingdata.at(k).animation_coef.at(18);
            if( valor >= umbral_fruncir_ceno_L){
                gestos.at(2)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(2)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }            
            if(valor >maxGestos.at(2)){
                maxGestos.at(2)= valor;
            }
        }
    }

    if(ui->scroll_umbral_fruncir_ceno_R->value()/20.0 != umbral_fruncir_ceno_R){
        umbral_fruncir_ceno_R = ui->scroll_umbral_fruncir_ceno_R->value()/20.0;
        ui->scroll_umbral_fruncir_ceno_R->setToolTip(QString::number(umbral_fruncir_ceno_R));
        linea.setColor(QColor(23, 236,210, 250));
        gestos.at(3)->clear();
        gestos.at(3)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = -trackingdata.trackingdata.at(k).animation_coef.at(13) - trackingdata.trackingdata.at(k).animation_coef.at(15) + trackingdata.trackingdata.at(k).animation_coef.at(17);

            if( valor >= umbral_fruncir_ceno_R){
                gestos.at(3)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(3)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }            
            if(valor >maxGestos.at(3)){
                maxGestos.at(3)= valor;
            }
        }
    }

    if(ui->scroll_umbral_sonreir_L->value()/20.0 != umbral_sonreir_L){
        umbral_sonreir_L = ui->scroll_umbral_sonreir_L->value()/20.0;
        ui->scroll_umbral_sonreir_L->setToolTip(QString::number(umbral_sonreir_L));
        linea.setColor(QColor(250, 100, 0, 250));
        gestos.at(4)->clear();
        gestos.at(4)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = (trackingdata.trackingdata.at(k).animation_coef.at(3) +
                           trackingdata.trackingdata.at(k).animation_coef.at(5) -
                           trackingdata.trackingdata.at(k).animation_coef.at(7)) /3;
            if( valor >= umbral_sonreir_L){
                gestos.at(4)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(4)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor >maxGestos.at(4)){
                maxGestos.at(4)= valor;
            }
        }
    }

    if(ui->scroll_umbral_sonreir_R->value()/20.0 != umbral_sonreir_R){
        umbral_sonreir_R = ui->scroll_umbral_sonreir_R->value()/20.0;
        ui->scroll_umbral_sonreir_R->setToolTip(QString::number(umbral_sonreir_R));
        linea.setColor(QColor(0, 0, 0, 250));
        gestos.at(5)->clear();
        gestos.at(5)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = (trackingdata.trackingdata.at(k).animation_coef.at(2) +
                           trackingdata.trackingdata.at(k).animation_coef.at(4) -
                           trackingdata.trackingdata.at(k).animation_coef.at(6)) /3;

            if(valor > umbral_sonreir_R){
                gestos.at(5)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(5)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor > maxGestos.at(5)){
                maxGestos.at(5) = valor;
            }
        }
    }

    if(ui->scroll_umbral_beso_L->value()/20.0 != umbral_beso_L){
        umbral_beso_L = ui->scroll_umbral_beso_L->value()/20.0;
        ui->scroll_umbral_beso_L->setToolTip(QString::number(umbral_beso_L));
        linea.setColor(QColor(220,0,0,250));
        gestos.at(6)->clear();
        gestos.at(6)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = -trackingdata.trackingdata.at(k).animation_coef.at(5);
            if(valor > umbral_beso_L){
                gestos.at(6)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(6)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor > maxGestos.at(6)){
                maxGestos.at(6) = valor;
            }
        }
    }

    if(ui->scroll_umbral_beso_R->value()/20.0 != umbral_beso_R){
        umbral_beso_R = ui->scroll_umbral_beso_R->value()/20.0;
        ui->scroll_umbral_beso_R->setToolTip(QString::number(umbral_beso_R));
        linea.setColor(QColor(0, 0, 150, 250));
        gestos.at(7)->clear();
        gestos.at(7)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = -trackingdata.trackingdata.at(k).animation_coef.at(4);
            if(valor > umbral_beso_R){
                gestos.at(7)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(7)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor > maxGestos.at(7)){
                maxGestos.at(7) = valor;
            }
        }
    }

    if(ui->scroll_umbral_cerrar_ojo_L->value()/20.0 != umbral_cerrar_ojo_L){
        umbral_cerrar_ojo_L = ui->scroll_umbral_cerrar_ojo_L->value()/20.0;
        ui->scroll_umbral_cerrar_ojo_L->setToolTip(QString::number(umbral_cerrar_ojo_L));
        linea.setColor(QColor(120, 0, 250, 250));
        gestos.at(8)->clear();
        gestos.at(8)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = trackingdata.trackingdata.at(k).animation_coef.at(20);
            if(valor > umbral_cerrar_ojo_L){
                gestos.at(8)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(8)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor > maxGestos.at(8)){
                maxGestos.at(8) = valor;
            }
        }
    }

    if(ui->scroll_umbral_cerrar_ojo_R->value()/20.0 != umbral_cerrar_ojo_R){
        umbral_cerrar_ojo_R = ui->scroll_umbral_cerrar_ojo_R->value()/20.0;
        ui->scroll_umbral_cerrar_ojo_R->setToolTip(QString::number(umbral_cerrar_ojo_R));
        linea.setColor(QColor(0, 180, 0, 250));
        gestos.at(9)->clear();
        gestos.at(9)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = trackingdata.trackingdata.at(k).animation_coef.at(19);
            if(valor > umbral_cerrar_ojo_R){
                gestos.at(9)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(9)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor > maxGestos.at(9)){
                maxGestos.at(9) = valor;
            }
        }
    }

    if(ui->scroll_umbral_cerrar_ojo_fuerte_L->value()/20.0 != umbral_cerrar_ojo_fuerte_L){
         umbral_cerrar_ojo_fuerte_L = ui->scroll_umbral_cerrar_ojo_fuerte_L->value()/20.0;
         ui->scroll_umbral_cerrar_ojo_fuerte_L->setToolTip(QString::number(umbral_cerrar_ojo_fuerte_L));
         linea.setColor(QColor(0, 120, 120, 250));
         gestos.at(10)->clear();
         gestos.at(10)->setPen(linea);
         for(int k=0;k<int(trackingdata.trackingdata.size());k++)
         {
             float valor = trackingdata.trackingdata.at(k).animation_coef.at(20) * 0.6 +
                     (-trackingdata.trackingdata.at(k).animation_coef.at(14) -
                       trackingdata.trackingdata.at(k).animation_coef.at(16) +
                       trackingdata.trackingdata.at(k).animation_coef.at(18)) * 0.4 ;
             if(valor > umbral_cerrar_ojo_fuerte_L){
                 gestos.at(10)->append(trackingdata.trackingdata.at(k).frame_number,valor);
             }
             else{
                 gestos.at(10)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
             }
             if(valor > maxGestos.at(10)){
                 maxGestos.at(10) = valor;
             }
         }
    }

   if(ui->scroll_umbral_cerrar_ojo_fuerte_R->value()/20.0 != umbral_cerrar_ojo_fuerte_R){
        umbral_cerrar_ojo_fuerte_R = ui->scroll_umbral_cerrar_ojo_fuerte_R->value()/20.0;
        ui->scroll_umbral_cerrar_ojo_fuerte_R->setToolTip(QString::number(umbral_cerrar_ojo_fuerte_R));
        linea.setColor(QColor(250, 0, 250, 250));
        gestos.at(11)->clear();
        gestos.at(11)->setPen(linea);
        for(int k=0;k<int(trackingdata.trackingdata.size());k++)
        {
            float valor = trackingdata.trackingdata.at(k).animation_coef.at(19) * 0.6 +
                          (-trackingdata.trackingdata.at(k).animation_coef.at(13) -
                            trackingdata.trackingdata.at(k).animation_coef.at(15) +
                            trackingdata.trackingdata.at(k).animation_coef.at(17)) * 0.4 ;
            if(valor > umbral_cerrar_ojo_fuerte_R){
                gestos.at(11)->append(trackingdata.trackingdata.at(k).frame_number,valor);
            }
            else{
                gestos.at(11)->append(trackingdata.trackingdata.at(k).frame_number,0.0);
            }
            if(valor > maxGestos.at(11)){
                maxGestos.at(11) = valor;
            }
        }
   }

   GraphGestos->legend()->setVisible(true);
   GraphGestos->legend()->setAlignment(Qt::AlignTop);
   GraphGestos->legend()->setBackgroundVisible(false);
   GraphGestos->legend()->markers(lineaVertical)[0]->setVisible(false);
}

void Results::update_gestos(int status){
        std::vector<string> box = {"BoxElevarCejasL", "BoxElevarCejasR", "BoxFruncirCenioL", "BoxFruncirCenioR", "BoxSonrreirL",
                         "BoxSonrreirR" , "BoxBesoL", "BoxBesoR", "BoxCerrarOjoL", "BoxCerrarOjoR", "BoxCerrarOjoFueL" , "BoxCerrarOjoFueR"};
        std::vector<int> row = {4,4,0,0,3,3,5,5,1,1,2,2}, column = {0,1,0,1,0,1,0,1,0,1,0,1};
        QBrush brush;
        brush.setColor(QColor(255,255,0,100));
        brush.setStyle(Qt::SolidPattern);

        bool checked = (status > 0);
        for(int k =0 ; k < int(box.size()) ; k++){
            if(box.at(k) == sender()->objectName().toUtf8().constData()){
                if(checked){
                    GraphGestos->addSeries(gestos.at(k));
                    ui->tablaGestos->item(row.at(k),column.at(k))->setBackground(brush);
                }
                else {
                    GraphGestos->removeSeries(gestos.at(k));
                    ui->tablaGestos->item(row.at(k),column.at(k))->setBackground(Qt::NoBrush);
                }
            }
        }

        QFont letraTitulo("Arial",16);
        GraphGestos->setTitleFont(letraTitulo);
        GraphGestos->createDefaultAxes();
        GraphGestos->axes(Qt::Vertical).first()->setTitleText("Activación de Gestos");
        GraphGestos->axes(Qt::Horizontal).first()->setTitleText("Cuadro del video");
        GraphGestos->axes(Qt::Vertical).first()->setRange(0,2);
        GraphGestos->axes(Qt::Horizontal).first()->setRange(trackingdata.trackingdata.at(0).frame_number ,cap.get(cv::CAP_PROP_FRAME_COUNT));
        GraphGestos->setTitle("GESTOS");
        ui->GestosGraph->setChart(GraphGestos);
}

void Results::graficar_maximos()
{
    QBarSet *derecho = new QBarSet("Lado derecho");
    QBarSet *izquierdo = new QBarSet("Lado izquierdo");

    *derecho << maxGestos.at(1)<< maxGestos.at(3) << maxGestos.at(5)<< maxGestos.at(7)<< maxGestos.at(9)<< maxGestos.at(11);
    *izquierdo << maxGestos.at(0)<< maxGestos.at(2)<< maxGestos.at(4)<< maxGestos.at(6)<< maxGestos.at(8)<< maxGestos.at(10);

    maximos->append(izquierdo);
    maximos->append(derecho);

    QFont letraTitulo("Arial",16);
    maximosGestos->setTitleFont(letraTitulo);
    maximosGestos->addSeries(maximos);
    maximosGestos->setTitle("NIVELES MÁXIMOS DE GESTOS");
    maximosGestos->setAnimationOptions(QChart::SeriesAnimations);

    QStringList gestos;
    gestos << "ElevarCejas" << "FruncirCeño" << "Sonreir" << "Beso" << "CerrarOjos" << "CerrarOjosFuerte";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(gestos);
    axisX->setTitleText("Niveles maximos");
    maximosGestos->addAxis(axisX,Qt::AlignBottom);
    maximos->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0,2);
    axisY->setTitleText("Gestos");

    maximosGestos->addAxis(axisY, Qt::AlignLeft);
    maximos->attachAxis(axisY);

    maximosGestos->legend()->setVisible(true);
    maximosGestos->legend()->setAlignment(Qt::AlignBottom);

    ui->GraphMaximos->setChart(maximosGestos);

    ui->tablaMaximos->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tablaMaximos->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tablaMaximos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablaMaximos->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablaMaximos->setSortingEnabled(false);
    ui->tablaMaximos->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tablaMaximos->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tablaMaximos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablaMaximos->setItem(0,0,new QTableWidgetItem(QString("Elevar Cejas")));
    ui->tablaMaximos->setItem(0,1,new QTableWidgetItem(QString::number(maxGestos.at(0),'f',3)));
    ui->tablaMaximos->setItem(0,2,new QTableWidgetItem(QString::number(maxGestos.at(1),'f',3)));
    ui->tablaMaximos->setItem(0,3,new QTableWidgetItem(QString::number(abs(maxGestos.at(0) - maxGestos.at(1)),'f',3)));
    ui->tablaMaximos->setItem(1,0,new QTableWidgetItem(QString("Fruncir Ceño")));
    ui->tablaMaximos->setItem(1,1,new QTableWidgetItem(QString::number(maxGestos.at(2),'f',3)));
    ui->tablaMaximos->setItem(1,2,new QTableWidgetItem(QString::number(maxGestos.at(3),'f',3)));
    ui->tablaMaximos->setItem(1,3,new QTableWidgetItem(QString::number(abs(maxGestos.at(2) - maxGestos.at(3)),'f',3)));
    ui->tablaMaximos->setItem(2,0,new QTableWidgetItem(QString("Sonreir")));
    ui->tablaMaximos->setItem(2,1,new QTableWidgetItem(QString::number(maxGestos.at(4),'f',3)));
    ui->tablaMaximos->setItem(2,2,new QTableWidgetItem(QString::number(maxGestos.at(5),'f',3)));
    ui->tablaMaximos->setItem(2,3,new QTableWidgetItem(QString::number(abs(maxGestos.at(4) - maxGestos.at(5)),'f',3)));
    ui->tablaMaximos->setItem(3,0,new QTableWidgetItem(QString("Beso")));
    ui->tablaMaximos->setItem(3,1,new QTableWidgetItem(QString::number(maxGestos.at(6),'f',3)));
    ui->tablaMaximos->setItem(3,2,new QTableWidgetItem(QString::number(maxGestos.at(7),'f',3)));
    ui->tablaMaximos->setItem(3,3,new QTableWidgetItem(QString::number(abs(maxGestos.at(6) - maxGestos.at(7)),'f',3)));
    ui->tablaMaximos->setItem(4,0,new QTableWidgetItem(QString("Cerrar Ojos")));
    ui->tablaMaximos->setItem(4,1,new QTableWidgetItem(QString::number(maxGestos.at(8),'f',3)));
    ui->tablaMaximos->setItem(4,2,new QTableWidgetItem(QString::number(maxGestos.at(9),'f',3)));
    ui->tablaMaximos->setItem(4,3,new QTableWidgetItem(QString::number(abs(maxGestos.at(8) - maxGestos.at(9)),'f',3)));
    ui->tablaMaximos->setItem(5,0,new QTableWidgetItem(QString("Cerrar Ojos Fuerte")));
    ui->tablaMaximos->setItem(5,1,new QTableWidgetItem(QString::number(maxGestos.at(10),'f',3)));
    ui->tablaMaximos->setItem(5,2,new QTableWidgetItem(QString::number(maxGestos.at(11),'f',3)));
    ui->tablaMaximos->setItem(5,3,new QTableWidgetItem(QString::number(abs(maxGestos.at(10) - maxGestos.at(11)),'f',3)));
    ui->tablaMaximos->item(0,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(0,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(0,3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(1,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(1,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(1,3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(2,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(2,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(2,3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(3,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(3,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(3,3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(4,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(4,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(4,3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(5,1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(5,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tablaMaximos->item(5,3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void Results::setValues(){
    // Relleno campos de datos del paciente con los valores ya existentes
    ui->nombre->setText(nombre);
    ui->apellido->setText(apellido);
    ui->dni->setText(dni);
    ui->edad->setText(edad);
    ui->fecha->setText(fecha);

    // Seteo como imagen inicial previsualizada el keyframe seleccionado
    cv::Mat frame;
    cap.set(cv::CAP_PROP_POS_FRAMES,trackingdata.trackingdata.at(0).frame_number - 1);
    cap.read(frame);
    QImage imgQ = MatToQImage(GetFittedImage(frame,241,411));
    ui->img_label->setPixmap(QPixmap::fromImage(imgQ));
    ui->img_label_2->setPixmap(QPixmap::fromImage(imgQ));
    ui->img_label_3->setPixmap(QPixmap::fromImage(imgQ));

    // Obtengo el numero de digitos de la cantidad maxima de frames y segundos para darle un ancho fijo
    // al texto de abajo a la derecha
    max_frame =  int(cap.get(cv::CAP_PROP_FRAME_COUNT));
    int digits = 0;
    if (max_frame < 0) digits = 1; // remove this line if '-' counts as a digit
    while (max_frame) {
        max_frame /= 10;
        digits++;
    }
    max_frame = digits;

    max_seconds =  int(cap.get(cv::CAP_PROP_FRAME_COUNT)/cap.get(cv::CAP_PROP_FPS));
    digits = 0;
    if (max_seconds < 0) digits = 1; // remove this line if '-' counts as a digit
    while (max_seconds) {
        max_seconds /= 10;
        digits++;
    }
    max_seconds = digits;

    ui->nro_frame_dist_ojo->setText("FRAME:" + QString::number(int(0)).rightJustified(max_frame, QChar(0x2007)) + " | " + QString::number(int(0)*(1.0/cap.get(cv::CAP_PROP_FPS)),'f',2).rightJustified(max_seconds, QChar(0x2007)) + "seg");
    ui->nro_frame_dist_ojoboca->setText("FRAME:" + QString::number(int(0)).rightJustified(max_frame, QChar(0x2007)) + " | " + QString::number(int(0)*(1.0/cap.get(cv::CAP_PROP_FPS)),'f',2).rightJustified(max_seconds, QChar(0x2007)) + "seg");
    ui->nro_frame->setText("FRAME:" + QString::number(int(0)).rightJustified(max_frame, QChar(0x2007)) + " | " + QString::number(int(0)*(1.0/cap.get(cv::CAP_PROP_FPS)),'f',2).rightJustified(max_seconds, QChar(0x2007)) + "seg");

}

void Results::on_PDF_clicked()
{
    std::size_t lenght;
    std::string nombre_video = filename;
    std::string nombre_PDF = filename;
    lenght = nombre_PDF.length();

    std::size_t found = nombre_video.find_last_of("/");
    if (found <= lenght && found > 0)
        nombre_video = nombre_video.substr(found+1,lenght);

    found = nombre_PDF.find_last_of(".");
    if (found <= lenght && found > 0)
        nombre_PDF = nombre_PDF.substr(0,found);
    nombre_PDF += ".pdf";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportar Informe"),QString::fromStdString(nombre_PDF), tr("PDF (*.pdf)"));

    if(fileName == NULL)
        return;
    std::cout << "Se guardara el archivo de Shape: " << fileName.toUtf8().constData() << std::endl;

    std::string pdf_filename = fileName.toUtf8().constData();

    QPdfWriter pdf(QString::fromStdString(pdf_filename));
    QPainter painter(&pdf);

    nombre = ui->nombre->text().toUtf8().constData(),
    apellido = ui->apellido->text().toUtf8().constData(),
    edad = ui->edad->text().toUtf8().constData(),
    dni = ui->dni->text().toUtf8().constData(),
    fecha = ui->fecha->text().toUtf8().constData();
    QString comentarios = ui->comentarios->text().toUtf8().constData();

    QBrush brush;
    brush.setColor(Qt::lightGray);
    brush.setStyle(Qt::SolidPattern);


    QPen lapiz;
    int y_recuadro = 150, w_recuadro = 9250, x_recuadro = 150;
    lapiz.setWidth(30);
    painter.setPen(lapiz);

    // ===============> Título
    int h_titulo = 350;
    QFont font = painter.font();
    font.setPixelSize(190);
    font.setBold("true");
    painter.setFont(font);
    painter.drawRect(QRect(x_recuadro,y_recuadro,w_recuadro,h_titulo));
    painter.drawText(x_recuadro,y_recuadro,w_recuadro, h_titulo, Qt::AlignCenter , "RESUMEN ANÁLISIS FACIAL");


    // ===============> Datos del paciente
    int h_renglon = 250, img_size = 1600;

    // Titulo Datos del Paciente
    painter.setBrush(brush);
    painter.drawRect(QRect(x_recuadro,y_recuadro + h_titulo, w_recuadro - img_size - 10,h_titulo));
    painter.setBrush(Qt::NoBrush);
    painter.drawText(x_recuadro,y_recuadro + h_titulo,w_recuadro - img_size, h_titulo, Qt::AlignCenter , "DATOS PERSONALES");

    lapiz.setWidth(20);
    painter.setPen(lapiz);
    font.setPixelSize(120);
    font.setBold(0);
    painter.setFont(font);


    // Nombre y Apellido
    painter.drawRect(QRect(x_recuadro, y_recuadro + 2*h_titulo, w_recuadro - img_size, h_renglon));
    painter.drawText(x_recuadro, y_recuadro + 2*h_titulo, w_recuadro - img_size, h_renglon, Qt::AlignVCenter , " NOMBRE: " + nombre.toUpper() + " " + apellido.toUpper());

    // DNI y Edad
    painter.drawRect(QRect(x_recuadro, y_recuadro + 2*h_titulo + h_renglon,(w_recuadro - img_size)/2, h_renglon));
    painter.drawText(x_recuadro, y_recuadro +2* h_titulo + h_renglon,(w_recuadro - img_size)/2, h_renglon, Qt::AlignVCenter , " DNI: " + dni.toUpper());

    painter.drawRect(QRect((w_recuadro - img_size)/2 + x_recuadro, y_recuadro + 2*h_titulo + h_renglon,(w_recuadro - img_size)/2, h_renglon));
    painter.drawText((w_recuadro - img_size)/2 + x_recuadro, y_recuadro + 2*h_titulo + h_renglon, (w_recuadro - img_size)/2, h_renglon, Qt::AlignVCenter , " EDAD: " + edad.toUpper() + " AÑOS");

    // Fecha
    painter.drawRect(QRect(x_recuadro, y_recuadro + 2*h_titulo + 2*h_renglon, w_recuadro - img_size, h_renglon));
    painter.drawText(x_recuadro, y_recuadro + 2*h_titulo + 2*h_renglon, w_recuadro - img_size, h_renglon, Qt::AlignVCenter , " FECHA: " + fecha.toUpper());

    // Nombre del Video
    painter.drawRect(QRect(x_recuadro, y_recuadro + 2*h_titulo + 3*h_renglon, w_recuadro - img_size, h_renglon));
    painter.drawText(x_recuadro, y_recuadro + 2*h_titulo + 3*h_renglon, w_recuadro - img_size, h_renglon, Qt::AlignVCenter , " NOMBRE DEL VIDEO: " + QString::fromStdString(nombre_video));

    // Comentarios
    painter.drawRect(QRect(x_recuadro, y_recuadro + 2*h_titulo + 4*h_renglon, w_recuadro - img_size, h_renglon));
    painter.drawText(x_recuadro, y_recuadro + 2*h_titulo + 4*h_renglon, w_recuadro - img_size, h_renglon, Qt::AlignVCenter , " COMENTARIOS: " + comentarios);

    // Foto
    lapiz.setWidth(30);
    painter.setPen(lapiz);
    painter.drawRect(QRect(x_recuadro + w_recuadro - img_size, y_recuadro + h_titulo, img_size, img_size)); // Foto

    // Linea Gruesa
    painter.drawRect(QRect(x_recuadro, y_recuadro + h_titulo, w_recuadro, img_size));


    // ===============> Grafica Distancia Ojo Boca
    int y_ojo_boca = y_recuadro + h_titulo + img_size + 2*h_renglon;

    // Titulo Datos del Paciente
    lapiz.setWidth(30);
    painter.setPen(lapiz);
    font.setPixelSize(190);
    font.setBold("true");
    painter.setFont(font);


    painter.setBrush(brush);
    painter.drawRect(QRect(x_recuadro, y_ojo_boca, w_recuadro, h_titulo));
    painter.setBrush(Qt::NoBrush);
    painter.drawText(x_recuadro, y_ojo_boca, w_recuadro, h_titulo, Qt::AlignCenter , "DISTANCIA OJO - BOCA");

    //  Tabla Maxima, Minima y Promedio
    lapiz.setWidth(20);
    painter.setPen(lapiz);
    font.setPixelSize(120);
    painter.setFont(font);

    painter.drawRect(QRect(x_recuadro + w_recuadro/5, y_ojo_boca + h_titulo, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + w_recuadro/5, y_ojo_boca + h_titulo, 2*w_recuadro/5, h_renglon, Qt::AlignCenter , "IZQUIERDA");

    painter.drawRect(QRect(x_recuadro + 3*w_recuadro/5, y_ojo_boca + h_titulo, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + 3*w_recuadro/5, y_ojo_boca + h_titulo, 2*w_recuadro/5, h_renglon, Qt::AlignCenter , "DERECHA");

    painter.drawRect(QRect(x_recuadro,y_ojo_boca + h_titulo + h_renglon, w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro,y_ojo_boca + h_titulo + h_renglon, w_recuadro/5, h_renglon, Qt::AlignVCenter , "  MÁXIMA");

    painter.drawRect(QRect(x_recuadro,y_ojo_boca + h_titulo + 2*h_renglon, w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro,y_ojo_boca + h_titulo + 2*h_renglon, w_recuadro/5, h_renglon, Qt::AlignVCenter , "  MÍNIMA");

    painter.drawRect(QRect(x_recuadro,y_ojo_boca + h_titulo + 3*h_renglon, w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro,y_ojo_boca + h_titulo + 3*h_renglon, w_recuadro/5, h_renglon, Qt::AlignVCenter , "  PROMEDIO");

    font.setBold(0);
    painter.setFont(font);

    // Maximos
    painter.drawRect(QRect(x_recuadro + w_recuadro/5, y_ojo_boca + h_titulo + h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + w_recuadro/5, y_ojo_boca + h_titulo + h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxDist.at(1),'f',3) + " "); // MAXIMO IZQUIERDA

    painter.drawRect(QRect(x_recuadro + 3*w_recuadro/5, y_ojo_boca + h_titulo + h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + 3*w_recuadro/5, y_ojo_boca + h_titulo + h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxDist.at(0),'f',3) + " "); // MAXIMO DERECHA

    // Minimos
    painter.drawRect(QRect(x_recuadro + w_recuadro/5, y_ojo_boca + h_titulo + 2*h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + w_recuadro/5, y_ojo_boca + h_titulo + 2*h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(minDist.at(1),'f',3) + " "); // MIN IZQUIERDA

    painter.drawRect(QRect(x_recuadro + 3*w_recuadro/5, y_ojo_boca + h_titulo + 2*h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + 3*w_recuadro/5, y_ojo_boca + h_titulo + 2*h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(minDist.at(0),'f',3) + " "); // MIN DERECHA

    // Promedios
    painter.drawRect(QRect(x_recuadro + w_recuadro/5, y_ojo_boca + h_titulo + 3*h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + w_recuadro/5, y_ojo_boca + h_titulo +3*h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(promDist.at(1),'f',3) + " "); // PROM IZQUIERDA

    painter.drawRect(QRect(x_recuadro + 3*w_recuadro/5, y_ojo_boca + h_titulo + 3*h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + 3*w_recuadro/5, y_ojo_boca + h_titulo +  3*h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(promDist.at(0),'f',3) + " "); // PROM DERECHA

    // Linea Gruesa
    lapiz.setWidth(30);
    painter.setPen(lapiz);
    painter.drawRect(QRect(x_recuadro, y_ojo_boca, w_recuadro, h_titulo + 4*h_renglon));


    // ===============> Grafica Distancia Parpado Superior - Inferior
    int y_ojo_sup_inf = y_ojo_boca + h_titulo +  6*h_renglon;

    // Titulo Datos del Paciente
    lapiz.setWidth(30);
    painter.setPen(lapiz);
    font.setPixelSize(190);
    font.setBold("true");
    painter.setFont(font);

    painter.setBrush(brush);
    painter.drawRect(QRect(x_recuadro, y_ojo_sup_inf, w_recuadro, h_titulo));
    painter.setBrush(Qt::NoBrush);
    painter.drawText(x_recuadro, y_ojo_sup_inf, w_recuadro, h_titulo, Qt::AlignCenter , "DISTANCIA PÁRPADO SUPERIOR - INFERIOR");

    //  Tabla Maxima, Minima y Promedio
    lapiz.setWidth(20);
    painter.setPen(lapiz);
    font.setPixelSize(120);
    painter.setFont(font);

    painter.drawRect(QRect(x_recuadro + w_recuadro/5, y_ojo_sup_inf + h_titulo, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + w_recuadro/5, y_ojo_sup_inf + h_titulo, 2*w_recuadro/5, h_renglon, Qt::AlignCenter , "IZQUIERDA");

    painter.drawRect(QRect(x_recuadro + 3*w_recuadro/5, y_ojo_sup_inf + h_titulo, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + 3*w_recuadro/5, y_ojo_sup_inf + h_titulo, 2*w_recuadro/5, h_renglon, Qt::AlignCenter , "DERECHA");

    painter.drawRect(QRect(x_recuadro,y_ojo_sup_inf + h_titulo + h_renglon, w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro,y_ojo_sup_inf + h_titulo + h_renglon, w_recuadro/5, h_renglon, Qt::AlignVCenter , "  MÁXIMA");

    painter.drawRect(QRect(x_recuadro,y_ojo_sup_inf + h_titulo + 2*h_renglon, w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro,y_ojo_sup_inf + h_titulo + 2*h_renglon, w_recuadro/5, h_renglon, Qt::AlignVCenter , "  MÍNIMA");

    painter.drawRect(QRect(x_recuadro,y_ojo_sup_inf + h_titulo + 3*h_renglon, w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro,y_ojo_sup_inf + h_titulo + 3*h_renglon, w_recuadro/5, h_renglon, Qt::AlignVCenter , "  PROMEDIO");

    font.setBold(0);
    painter.setFont(font);

    // Maximos
    painter.drawRect(QRect(x_recuadro + w_recuadro/5, y_ojo_sup_inf + h_titulo + h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + w_recuadro/5, y_ojo_sup_inf + h_titulo + h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxDist.at(3),'f',3) + " "); // MAXIMO IZQUIERDA

    painter.drawRect(QRect(x_recuadro + 3*w_recuadro/5, y_ojo_sup_inf + h_titulo + h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + 3*w_recuadro/5, y_ojo_sup_inf + h_titulo + h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxDist.at(2),'f',3) + " "); // MAXIMO DERECHA

    // Minimos
    painter.drawRect(QRect(x_recuadro + w_recuadro/5, y_ojo_sup_inf + h_titulo + 2*h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + w_recuadro/5, y_ojo_sup_inf + h_titulo + 2*h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(minDist.at(3),'f',3) + " "); // MIN IZQUIERDA

    painter.drawRect(QRect(x_recuadro + 3*w_recuadro/5, y_ojo_sup_inf + h_titulo + 2*h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + 3*w_recuadro/5, y_ojo_sup_inf + h_titulo + 2*h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(minDist.at(2),'f',3) + " "); // MIN DERECHA

    // Promedios
    painter.drawRect(QRect(x_recuadro + w_recuadro/5, y_ojo_sup_inf + h_titulo + 3*h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + w_recuadro/5, y_ojo_sup_inf + h_titulo +3*h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(promDist.at(3),'f',3) + " "); // PROM IZQUIERDA

    painter.drawRect(QRect(x_recuadro + 3*w_recuadro/5, y_ojo_sup_inf + h_titulo + 3*h_renglon, 2*w_recuadro/5, h_renglon));
    painter.drawText(x_recuadro + 3*w_recuadro/5, y_ojo_sup_inf + h_titulo +  3*h_renglon, 2*w_recuadro/5, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(promDist.at(2),'f',3) + " "); // PROM DERECHA

    // Linea Gruesa
    lapiz.setWidth(30);
    painter.setPen(lapiz);
    painter.drawRect(QRect(x_recuadro, y_ojo_sup_inf, w_recuadro, h_titulo + 4*h_renglon));


    // ===============> Grafica Gestos
    int y_gestos = y_ojo_sup_inf + h_titulo +  6*h_renglon;

    // Titulo Datos del Paciente
    lapiz.setWidth(30);
    painter.setPen(lapiz);
    font.setPixelSize(190);
    font.setBold("true");
    painter.setFont(font);


    painter.setBrush(brush);
    painter.drawRect(QRect(x_recuadro, y_gestos, w_recuadro, h_titulo));
    painter.setBrush(Qt::NoBrush);
    painter.drawText(x_recuadro, y_gestos, w_recuadro, h_titulo, Qt::AlignCenter , "GESTOS");

    //  Tabla Maxima, Minima y Promedio
    lapiz.setWidth(20);
    painter.setPen(lapiz);
    font.setPixelSize(120);
    painter.setFont(font);

    painter.drawRect(QRect(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo,2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo,2 * w_recuadro/9, h_renglon, Qt::AlignCenter , "MÁXIMO IZQUIERDA");

    painter.drawRect(QRect(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo,2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo, 2 * w_recuadro/9, h_renglon, Qt::AlignCenter , "MÁXIMO DERECHA");

    painter.drawRect(QRect(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo,2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo, 2 * w_recuadro/9, h_renglon, Qt::AlignCenter , "DIFERENCIA");

    painter.drawRect(QRect(x_recuadro,y_gestos + h_titulo + h_renglon, w_recuadro/3, h_renglon));
    painter.drawText(x_recuadro,y_gestos + h_titulo + h_renglon, w_recuadro/3, h_renglon, Qt::AlignVCenter , "  FRUNCIR CEÑO");

    painter.drawRect(QRect(x_recuadro,y_gestos + h_titulo + 2*h_renglon, w_recuadro/3, h_renglon));
    painter.drawText(x_recuadro,y_gestos + h_titulo + 2*h_renglon, w_recuadro/3, h_renglon, Qt::AlignVCenter , "  CERRAR OJO");

    painter.drawRect(QRect(x_recuadro,y_gestos + h_titulo + 3*h_renglon, w_recuadro/3, h_renglon));
    painter.drawText(x_recuadro,y_gestos + h_titulo + 3*h_renglon, w_recuadro/3, h_renglon, Qt::AlignVCenter , "  CERRAR OJO FUERTE");

    painter.drawRect(QRect(x_recuadro,y_gestos + h_titulo + 4*h_renglon, w_recuadro/3, h_renglon));
    painter.drawText(x_recuadro,y_gestos + h_titulo + 4*h_renglon, w_recuadro/3, h_renglon, Qt::AlignVCenter , "  SONREIR");

    painter.drawRect(QRect(x_recuadro,y_gestos + h_titulo + 5*h_renglon, w_recuadro/3, h_renglon));
    painter.drawText(x_recuadro,y_gestos + h_titulo + 5*h_renglon, w_recuadro/3, h_renglon, Qt::AlignVCenter , "  ELEVAR CEJA");

    painter.drawRect(QRect(x_recuadro,y_gestos + h_titulo + 6*h_renglon, w_recuadro/3, h_renglon));
    painter.drawText(x_recuadro,y_gestos + h_titulo + 6*h_renglon, w_recuadro/3, h_renglon, Qt::AlignVCenter , "  BESO");

    font.setBold(0);
    painter.setFont(font);

    // Maxima Izq
    painter.drawRect(QRect(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(2),'f',3) + " "); // Fruncir ceno

    painter.drawRect(QRect(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 2*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 2* h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(8),'f',3) + " "); // Cerrar ojo

    painter.drawRect(QRect(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 3*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 3*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(10),'f',3) + " "); // Cerrar ojo fuerte

    painter.drawRect(QRect(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 4*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 4*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(4),'f',3) + " "); // Sonreir

    painter.drawRect(QRect(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 5*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 5*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(0),'f',3) + " "); // Elevar Ceja

    painter.drawRect(QRect(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 6*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 3 * w_recuadro/9, y_gestos + h_titulo + 6*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(6),'f',3) + " "); // Beso


    // Maxima Der
    painter.drawRect(QRect(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(3),'f',3) + " "); // Fruncir ceno

    painter.drawRect(QRect(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 2*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 2* h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(9),'f',3) + " "); // Cerrar ojo

    painter.drawRect(QRect(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 3*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 3*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(11),'f',3) + " "); // Cerrar ojo fuerte

    painter.drawRect(QRect(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 4*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 4*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(5),'f',3) + " "); // Sonreir

    painter.drawRect(QRect(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 5*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 5*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(1),'f',3) + " "); // Elevar Ceja

    painter.drawRect(QRect(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 6*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 5 * w_recuadro/9, y_gestos + h_titulo + 6*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(maxGestos.at(7),'f',3) + " "); // Beso


    // Diferencia
    painter.drawRect(QRect(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(abs(maxGestos.at(2) - maxGestos.at(3)),'f',3) + " "); // Fruncir ceno

    painter.drawRect(QRect(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 2*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 2* h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(abs(maxGestos.at(8) - maxGestos.at(9)),'f',3) + " "); // Cerrar ojo

    painter.drawRect(QRect(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 3*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 3*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(abs(maxGestos.at(10) - maxGestos.at(11)),'f',3) + " "); // Cerrar ojo fuerte

    painter.drawRect(QRect(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 4*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 4*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(abs(maxGestos.at(4) - maxGestos.at(5)),'f',3) + " "); // Sonreir

    painter.drawRect(QRect(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 5*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 5*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(abs(maxGestos.at(0) - maxGestos.at(1)),'f',3) + " "); // Elevar Ceja

    painter.drawRect(QRect(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 6*h_renglon, 2 * w_recuadro/9, h_renglon));
    painter.drawText(x_recuadro + 7 * w_recuadro/9, y_gestos + h_titulo + 6*h_renglon, 2 * w_recuadro/9, h_renglon, Qt::AlignRight | Qt::AlignVCenter , QString::number(abs(maxGestos.at(6) - maxGestos.at(7)),'f',3) + " "); // Beso


    // ===============> Recuadro externo
    lapiz.setWidth(30);
    painter.setPen(lapiz);
    int h_recuadro = y_gestos + h_titulo + 6*h_renglon + h_renglon - y_recuadro;
    painter.drawRect(QRect(x_recuadro,y_recuadro,w_recuadro,h_recuadro));


    //  ===============> Foto paciente
    cv::Mat foto;
    cv::CascadeClassifier HC_face;
    HC_face.load("haarcascade_frontalface_alt2.xml");
    cv::Mat gray, small_img;
        int maxArea = 0;
        int indx = 0;
        bool face_detected = 0;
        // Preproceso la imagen
        if (kf_image.channels() == 3) cv::cvtColor(kf_image, gray, cv::COLOR_BGR2GRAY);
        else gray = kf_image.clone();
        cv::resize(gray, small_img, cv::Size(kf_image.cols * RESIZE_F, kf_image.rows * RESIZE_F) , 0, 0, cv::INTER_LINEAR);
        cv::equalizeHist(small_img, small_img);
        std::vector<cv::Rect> faces;
        cv::Rect roi;
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
            if(cvRound(faces[indx].x * RESIZE_F)-0.2*cvRound(faces[indx].width * RESIZE_F)>0)   faces[indx].x=cvRound(faces[indx].x * RESIZE_F)-0.2*cvRound(faces[indx].width * RESIZE_F);
            else faces[indx].x=cvRound(faces[indx].x * RESIZE_F);
            if(1.4*cvRound(faces[indx].width * RESIZE_F)+faces[indx].x<kf_image.cols) faces[indx].width=1.4*cvRound(faces[indx].width * RESIZE_F);
            else faces[indx].width=cvRound(faces[indx].width * RESIZE_F);
            faces[indx].y = 0;
            faces[indx].height = kf_image.rows;
            roi = faces[indx];
        }
    if (face_detected) foto=kf_image(roi);
    else foto=kf_image;
    int width = foto.cols,height = foto.rows;
    cv::Rect rect;
    cv::Mat square = cv::Mat(img_size, img_size, CV_8UC3, cv::Scalar(255,255,255));
    float rel = float(width) / float(height);
    float target_rel = float(img_size) / float(img_size);
    if (target_rel >= rel)
    {
        rect.y = 0;
        rect.height = img_size;
        rect.width = img_size * rel;
        rect.x = (img_size - rect.width) / 2;
    }
    else
    {
        rect.x = 0;
        rect.width = img_size;
        rect.height = (img_size / rel); //target_height * scale;
        rect.y = (img_size - rect.height) / 2;
    }
    cv::resize(foto, square(rect), rect.size());
    QImage imgQ3 = MatToQImage(square);

    painter.drawImage(QRect(x_recuadro + w_recuadro - img_size + 30, y_recuadro + h_titulo + 30 ,img_size - 60,img_size - 60),imgQ3);


    painter.end();
    bool r = QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(pdf_filename)));

}


void Results::on_exp_gestos_clicked()
{
    qDebug() << "Gestos";
    std::size_t lenght;
    std::string nombre_csv = filename;
    lenght = nombre_csv.length();


    std::size_t found = nombre_csv.find_last_of(".");
    if (found <= lenght && found > 0)
        nombre_csv = nombre_csv.substr(0,found);
    nombre_csv += "_gestos.csv";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportar CSV Gestos"),QString::fromStdString(nombre_csv), tr("CSV (*.csv)"));

    if(fileName == NULL)
        return;
    // Almaceno el valor de los umbrales
    int scroll_umbral_elevar_ceja_L = ui->scroll_umbral_elevar_ceja_L->value();
    int scroll_umbral_elevar_ceja_R = ui->scroll_umbral_elevar_ceja_R->value();
    int scroll_umbral_fruncir_ceno_L = ui->scroll_umbral_fruncir_ceno_L->value();
    int scroll_umbral_fruncir_ceno_R = ui->scroll_umbral_fruncir_ceno_R->value();
    int scroll_umbral_sonreir_L = ui->scroll_umbral_sonreir_L->value();
    int scroll_umbral_sonreir_R = ui->scroll_umbral_sonreir_R->value();
    int scroll_umbral_beso_L = ui->scroll_umbral_beso_L->value();
    int scroll_umbral_beso_R = ui->scroll_umbral_beso_R->value();
    int scroll_umbral_cerrar_ojo_L = ui->scroll_umbral_cerrar_ojo_L->value();
    int scroll_umbral_cerrar_ojo_R = ui->scroll_umbral_cerrar_ojo_R->value();
    int scroll_umbral_cerrar_ojo_fuerte_L = ui->scroll_umbral_cerrar_ojo_fuerte_L->value();
    int scroll_umbral_cerrar_ojo_fuerte_R = ui->scroll_umbral_cerrar_ojo_fuerte_R->value();

    // Pongo los umbrales a 0 para graficar
    ui->scroll_umbral_elevar_ceja_L->setValue(0);
    ui->scroll_umbral_elevar_ceja_R->setValue(0);
    ui->scroll_umbral_fruncir_ceno_L->setValue(0);
    ui->scroll_umbral_fruncir_ceno_R->setValue(0);
    ui->scroll_umbral_sonreir_L->setValue(0);
    ui->scroll_umbral_sonreir_R->setValue(0);
    ui->scroll_umbral_beso_L->setValue(0);
    ui->scroll_umbral_beso_R->setValue(0);
    ui->scroll_umbral_cerrar_ojo_L->setValue(0);
    ui->scroll_umbral_cerrar_ojo_R->setValue(0);
    ui->scroll_umbral_cerrar_ojo_fuerte_L->setValue(0);
    ui->scroll_umbral_cerrar_ojo_fuerte_R->setValue(0);

    // Obtengo las graficas actualizadas
    grafica_dinamica_gestos();

    // Genero CSV
    ofstream myfile;
    myfile.open (fileName.toStdString());
    myfile << "timestamp,elevar_ceja_L,elevar_ceja_R,fruncir_ceno_L,fruncir_ceno_R,sonreir_L,sonreir_R,beso_L,beso_R,cerrar_ojo_L,cerrar_ojo_R,cerrar_ojo_fuerte_L,cerrar_ojo_fuerte_L,dist_ojo_boca_L,dist_ojo_boca_R,dist_ojo_sup_inf_L,dist_ojo_sup_inf_R" << std::endl;
    for(int k=0 ; k < int(trackingdata.trackingdata.size()); k++){
        float timestamp = trackingdata.trackingdata.at(k).frame_number*(1.0/cap.get(cv::CAP_PROP_FPS));

        myfile << std::setprecision(5) << timestamp << "," << gestos.at(0)->at(k).y() << "," << gestos.at(1)->at(k).y() << "," << gestos.at(2)->at(k).y() << ","
                                    << gestos.at(3)->at(k).y() << "," << gestos.at(4)->at(k).y() << "," << gestos.at(5)->at(k).y() << ","
                                    << gestos.at(6)->at(k).y() << "," << gestos.at(7)->at(k).y() << "," << gestos.at(8)->at(k).y() << ","
                                    << gestos.at(9)->at(k).y() << "," << gestos.at(10)->at(k).y() << "," << gestos.at(11)->at(k).y() << ","
                                    << distOjoBocaL->at(k).y() << "," << distOjoBocaR->at(k).y() << ","
                                    << distOjoSupInfL->at(k).y() << "," << distOjoSupInfR->at(k).y() << std::endl;
    }
    myfile.close();

    // Vuelvo a poner las graficas con los umbrales que tenian antes
    ui->scroll_umbral_elevar_ceja_L->setValue(scroll_umbral_elevar_ceja_L);
    ui->scroll_umbral_elevar_ceja_R->setValue(scroll_umbral_elevar_ceja_R);
    ui->scroll_umbral_fruncir_ceno_L->setValue(scroll_umbral_fruncir_ceno_L);
    ui->scroll_umbral_fruncir_ceno_R->setValue(scroll_umbral_fruncir_ceno_R);
    ui->scroll_umbral_sonreir_L->setValue(scroll_umbral_sonreir_L);
    ui->scroll_umbral_sonreir_R->setValue(scroll_umbral_sonreir_R);
    ui->scroll_umbral_beso_L->setValue(scroll_umbral_beso_L);
    ui->scroll_umbral_beso_R->setValue(scroll_umbral_beso_R);
    ui->scroll_umbral_cerrar_ojo_L->setValue(scroll_umbral_cerrar_ojo_L);
    ui->scroll_umbral_cerrar_ojo_R->setValue(scroll_umbral_cerrar_ojo_R);
    ui->scroll_umbral_cerrar_ojo_fuerte_L->setValue(scroll_umbral_cerrar_ojo_fuerte_L);
    ui->scroll_umbral_cerrar_ojo_fuerte_R->setValue(scroll_umbral_cerrar_ojo_fuerte_R);

    // Actualizo las graficas
    grafica_dinamica_gestos();

}

void Results::on_exp_2d_clicked()
{
    qDebug() << "2D";
    std::size_t lenght;
    std::string nombre_csv = filename;
    lenght = nombre_csv.length();

    std::size_t found = nombre_csv.find_last_of(".");
    if (found <= lenght && found > 0)
        nombre_csv = nombre_csv.substr(0,found);
    nombre_csv += "_2D.csv";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportar CSV Gestos"),QString::fromStdString(nombre_csv), tr("CSV (*.csv)"));

    if(fileName == NULL)
        return;

    // Genero CSV
    ofstream myfile;
    std::vector<std::string> ejes = {"x", "y"};
    myfile.open (fileName.toStdString());

    // Encabezado
    myfile << "timestamp,";
    for (int k=0; k < int(trackingdata.trackingdata.at(0).puntos.size()) ; k++){
        for(int i=0; i < int(ejes.size()) ; i++){
            myfile << "P" << std::setfill('0') << std::setw(3) << k << "_" << ejes.at(i) << ",";
        }
    }
    myfile << std::endl;


    // Rescalo los puntos para obtener las coordenadas en el video original
    int target_width =TARGET_WIDTH ,
        target_height = TARGET_HEIGHT,
        width = cap.get(cv::CAP_PROP_FRAME_WIDTH),
        height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    float rel = float(width) / float(height);
    float target_rel = float(target_width) / float(target_height);
    float rel_height, rel_width, diff_x, diff_y;

    if (target_rel >= rel)
    {
        rel_height = target_height;
        rel_width = target_height * rel;
        diff_x = (target_width - rel_width) / 2;
        diff_y = 0;
    }
    else
    {
        rel_width = target_width;
        rel_height = (target_width / rel);
        diff_y = (target_height - rel_height) / 2;
        diff_x = 0;
    }

    /* ==========> Ver puntos en video original
     * Esta seccion del codigo permite ver directamente los puntos trackeados en el video original.
    cv::Mat frame;
    cap.set(cv::CAP_PROP_POS_FRAMES, trackingdata.trackingdata.at(0).frame_number);
    int indx = 0;
    while(cap.read(frame)){

        for (int k =0; k < int(trackingdata.trackingdata.at(0).puntos.size()); k++){
            cv::circle(frame, cv::Point2i(int(float(trackingdata.trackingdata.at(indx).puntos.at(k).x - diff_x) *(width/rel_width)) ,
                                          int(float(trackingdata.trackingdata.at(indx).puntos.at(k).y - diff_y) * (height/rel_height))),
                                          1, cv::Scalar(0,0,255), 2);
        }
        indx++;
        cv::imshow("Frame",frame);
        cv::waitKey(int(1.0/30.0 * 1000));
    }
    */


    // Datos
    for (int k=0; k < int(trackingdata.trackingdata.size()) ; k++){
        float timestamp = trackingdata.trackingdata.at(k).frame_number*(1.0/cap.get(cv::CAP_PROP_FPS));
        myfile << std::setprecision(3) << timestamp << "," << std::setprecision(5);
        for (int j=0; j < int(trackingdata.trackingdata.at(k).puntos.size()) ; j++){
            myfile  << float(trackingdata.trackingdata.at(k).puntos.at(j).x - diff_x) * (width/rel_width) << ","
                    << float(trackingdata.trackingdata.at(k).puntos.at(j).y - diff_y) * (height/rel_height) << ",";
        }
        myfile << std::endl;
    }
    myfile.close();
    qDebug() << "CSV 2D guardado";
}

void Results::on_exp_3d_clicked()
{
    qDebug() << "3D";
    std::size_t lenght;
    std::string nombre_csv = filename;
    lenght = nombre_csv.length();

    std::size_t found = nombre_csv.find_last_of(".");
    if (found <= lenght && found > 0)
        nombre_csv = nombre_csv.substr(0,found);
    nombre_csv += "_3D.csv";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exportar CSV Gestos"),QString::fromStdString(nombre_csv), tr("CSV (*.csv)"));

    if(fileName == NULL)
        return;
    // Genero CSV
    ofstream myfile;
    std::vector<std::string> ejes = {"x", "y", "z"};
    myfile.open (fileName.toStdString());

    // Encabezado
    myfile << std::setprecision(2) << "timestamp,";
    for (int k=0; k < int(trackingdata.trackingdata.at(0).vertices.size()) ; k++){
        for(int i=0; i < int(ejes.size()) ; i++){
            myfile << "P" << std::setfill('0') << std::setw(3) << k << "_" << ejes.at(i) << ",";
        }
    }
    myfile << "rot_x,rot_y,rot_z,tr_x,tr_y,tr_z,";
    for(int k=0; k < int(trackingdata.trackingdata.at(0).animation_coef.size()); k++){
        myfile << "AUV_" << std::setfill('0') << std::setw(3) << k << ",";
    }
    for(int k=0; k < int(trackingdata.shape_coef.size()-1); k++){
        myfile << "SUV_" << std::setfill('0') << std::setw(3) << k << ",";
    }
    myfile << "SUV_" << std::setfill('0') << std::setw(3) << int(trackingdata.shape_coef.size()-1) << std::endl;


    // Datos
    for (int k=0; k < int(trackingdata.trackingdata.size()) ; k++){

        float timestamp = trackingdata.trackingdata.at(k).frame_number*(1.0/cap.get(cv::CAP_PROP_FPS));
        myfile << std::setprecision(3) << timestamp << "," << std::setprecision(5);
        for (int j=0; j < int(trackingdata.trackingdata.at(k).vertices.size()) ; j++){
            myfile  << trackingdata.trackingdata.at(k).vertices.at(j).x << ","
                    << trackingdata.trackingdata.at(k).vertices.at(j).y << ","
                    << trackingdata.trackingdata.at(k).vertices.at(j).z << ",";
        }
        myfile << trackingdata.trackingdata.at(k).rotacion.y << "," << trackingdata.trackingdata.at(k).rotacion.x << "," << trackingdata.trackingdata.at(k).rotacion.z << ","
               << trackingdata.trackingdata.at(k).traslacion.x << "," << trackingdata.trackingdata.at(k).traslacion.y << "," << trackingdata.trackingdata.at(k).traslacion.z << ",";

        for(int j=0; j < int(trackingdata.trackingdata.at(k).animation_coef.size()); j++){
            //qDebug() << j << ": "  << trackingdata.trackingdata.at(k).animation_coef.at(j);
            myfile << trackingdata.trackingdata.at(k).animation_coef.at(j) << ",";
            //std::cout << trackingdata.trackingdata.at(k).animation_coef.at(j) << ", ";
        }
        //std::cout << std::endl;
        for(int j=0; j < int(trackingdata.shape_coef.size()-1); j++){
            myfile << trackingdata.shape_coef.at(j) << ",";
        }
        myfile << trackingdata.shape_coef.at(int(trackingdata.shape_coef.size()-1)) << std::endl ;

    }
    myfile.close();
    qDebug() << "CSV 3D guardado";
}
