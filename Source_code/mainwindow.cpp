#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "facialmesh.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    std::cout << "Inicializando MainWindow" << std::endl;
    ui->setupUi(this);
    ui->stackedWidget->insertWidget(1,&kf_selection);
    ui->stackedWidget->insertWidget(2,&m_fitting);
    ui->stackedWidget->insertWidget(3,&tr_video);
    ui->stackedWidget->insertWidget(4,&results);
    ui->stackedWidget->insertWidget(5,&mesh);
    ui->stackedWidget->setCurrentIndex(0);
    prev_index = ui->stackedWidget->currentIndex();

    connect(&kf_selection, SIGNAL(keyframe_selected()), this, SLOT(move2modelfitting()));
    connect(&m_fitting, SIGNAL(keyframe_ready()), this, SLOT(move2trackingvideo()));
    connect(&tr_video, SIGNAL(show_results()),this, SLOT(move2results()));
    connect(&mesh, SIGNAL(go2init()), this, SLOT(move2init()));

    // ***** Evento para trackear el mouse
    ui->frame_title->installEventFilter(this);
    pressed = false;

    // ***** Sombras
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setColor(QColor(0, 0, 0, 100));
    ui->frame->setGraphicsEffect(shadow);
    QGraphicsDropShadowEffect *shadow_label = new QGraphicsDropShadowEffect();
    shadow_label->setBlurRadius(20);
    shadow_label->setXOffset(0);
    shadow_label->setYOffset(0);
    shadow_label->setColor(QColor(0, 0, 0, 100));
    ui->label->setGraphicsEffect(shadow_label);
    QGraphicsDropShadowEffect *shadow_button = new QGraphicsDropShadowEffect();
    shadow_button->setBlurRadius(80);
    shadow_button->setXOffset(15);
    shadow_button->setYOffset(15);
    shadow_button->setColor(QColor(0, 0, 0, 200));
    ui->boton_empezar->setGraphicsEffect(shadow_button);

    // ***** Cambio de tamano de ventana
    QSizeGrip *sizegrip = new QSizeGrip(ui->frame_grip);
    sizegrip->setStyleSheet("QSizeGrip { width: 10px; height: 10px; margin: 5px } QSizeGrip:hover { background-color: rgb(69, 77, 102) }");
    sizegrip->setToolTip("Cambiar Tamaño");

    // ***** Shortcuts
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SLOT(on_inicio_salir_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(on_boton_salir_clicked()));
    new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(on_boton_instrucciones_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_M), this, SLOT(on_boton_mascara_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V), this, SLOT(on_boton_about_clicked()));

    // ***** Tooltips atajos
    ui->boton_inicio->setToolTip("CTRL+R");
    ui->boton_salir->setToolTip("CTRL+Q");
    ui->boton_instrucciones->setToolTip("F1");
    ui->boton_about->setToolTip("CTRL+I");
    ui->boton_mascara->setToolTip("CTRL+M");

    std::cout << "MainWindow inicializado" << std::endl;
}

MainWindow::~MainWindow(){
    delete ui;  
}

// =========================================================================> CAMBIOS DE INDEX
void MainWindow::on_boton_empezar_clicked(){
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::move2modelfitting(){
    candide = new facialmesh(CANDIDE_MODEL);
    m_fitting.candide = candide;
    m_fitting.init();
    ui->stackedWidget->setCurrentIndex(2);
    m_fitting.kf_image = kf_selection.frame.clone();
    m_fitting.candide->set_keyframe_image(kf_selection.frame);
    m_fitting.candide->actualizar_2D();
    m_fitting.actualizar_posicion();
}

void MainWindow::move2trackingvideo(){
    ui->stackedWidget->setCurrentIndex(3);
    tr_video.tracker.candide =candide;
    tr_video.cap = kf_selection.cap; // TODO: pasar cap a puntero
    tr_video.kf_image = m_fitting.kf_image.clone();
    // ====> Opcion 1: Guardo el log en un archivo temporal para que no se almacene en la PC
    QTemporaryFile file;
    file.open();
    tr_video.logfile.set_log_filename(file.fileName().toStdString(),TEMPORARY_LOG);
    // ====> Opcion 2: Para guardar el archivo en el directorio del video.
    //tr_video.logfile.set_log_filename(kf_selection.filename, false);
    tr_video.logfile.set_total_frames(kf_selection.cap.get(cv::CAP_PROP_FRAME_COUNT) - kf_selection.cap.get(cv::CAP_PROP_POS_FRAMES));  
    kf_selection.cap.release();
    tr_video.procesarvideo();
}

void MainWindow::move2results(){
    ui->stackedWidget->setCurrentIndex(4);
    results.trackingdata.load_data(tr_video.logfile.filename);
    results.kf_image = tr_video.kf_image.clone();
    results.cap = tr_video.cap;
    tr_video.cap.release();

    //Paso los datos del paciente
    results.nombre = tr_video.nombre;
    results.apellido = tr_video.apellido;
    results.edad = tr_video.edad;
    results.dni = tr_video.dni;
    results.fecha = tr_video.fecha;
    results.filename = kf_selection.filename;

    results.graficar_distancias("ojo");
    results.graficar_distancias("ojoboca");
    results.grafica_dinamica_gestos();
    results.graficar_maximos();
    results.setValues();
}

void MainWindow::move2init(){
    ui->stackedWidget->setCurrentIndex(prev_index);
}


// =========================================================================> MOUSE EVENTS
void MainWindow::mousePressEvent(QMouseEvent *event){
    current = event->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(pressed)
        this->move(mapToParent(event->pos() - current));
}

bool MainWindow::eventFilter(QObject *object, QEvent *event){
    if (object == ui->frame_title && event->type() == QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent = (QMouseEvent*)event;
        if (pressed == false){
            current = mouseEvent->pos();
        }
        pressed = true;
        return true;
    }
    if (event->type() == QEvent::MouseButtonRelease){
        pressed = false;
        return true;
    }
    if (object == ui->frame_title && event->type() == QEvent::MouseButtonDblClick){
        on_btn_maximize_clicked();
    }
    else
        return false;
}


// =========================================================================> BOTONES BARRA
void MainWindow::on_btn_close_clicked(){
    this->close();
    qApp->quit();
    QApplication::quit();
}

void MainWindow::on_btn_minimize_clicked(){
    this->showMinimized();
}

void MainWindow::on_btn_maximize_clicked(){
    this->showMaximized();

    // IF NOT MAXIMIZED
    if(GLOBAL_STATE == 0){
        this->showMaximized();
        GLOBAL_STATE = 1;

   // IF MAXIMIZED REMOVE MARGINS AND BORDER RADIUS
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0, stop:0.198864 rgba(52, 57, 76, 255), stop:1 rgba(137, 150, 199, 255)); border-radius: 0px;");
    ui->centralwidget_layout->setContentsMargins(0,0,0,0);
    ui->btn_maximize->setToolTip("Restaurar");
    }

    else{
        this->showNormal();
        GLOBAL_STATE = 0;
        this->resize(this->width()+1, this->height()+1);      
        ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0, stop:0.198864 rgba(52, 57, 76, 255), stop:1 rgba(137, 150, 199, 255)); border-radius: 10px;");
        ui->centralwidget_layout->setContentsMargins(10,10,10,10);
        ui->btn_maximize->setToolTip("Maximizar");
    }
}


// =========================================================================> HAMB. MENU
void MainWindow::on_boton_toggle_clicked(){
    int width = ui->frame_left_menu->width();
    int extended = 170;
    int collapsed = 0;
    int width_expansion;

    if(width == collapsed){
        width_expansion = extended;
    }
    else{
        width_expansion = collapsed;
    }

    // Animacion
    QPropertyAnimation *animation = new QPropertyAnimation();
    animation->setTargetObject(ui->frame_left_menu);
    animation->setPropertyName("minimumWidth");
    animation->setDuration(400);
    animation->setStartValue(width);
    animation->setEndValue(width_expansion);
    animation->setEasingCurve(QEasingCurve::InOutQuart);
    animation->start();
}

void MainWindow::on_boton_mascara_clicked(){
    if (ui->stackedWidget->currentIndex() != 5){
        facialmesh *candide1 = new facialmesh(CANDIDE_MODEL);
        mesh.candide = candide1;
        mesh.init(); 
        prev_index = ui->stackedWidget->currentIndex();
        ui->stackedWidget->setCurrentIndex(5);
        mesh.kf_image = cv::Mat(563, 1000, CV_8UC3, cv::Scalar(255,255,255));
        mesh.candide->actualizar_2D();
        mesh.actualizar_posicion();
    }
    on_boton_toggle_clicked();
}

void MainWindow::on_boton_inicio_clicked(){
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindow::on_boton_about_clicked(){
    int ret = QMessageBox::information(this, tr("Face Tracking."),
                                   tr("Rosario, Argentina.\n""Versión: 1.0.0 \n"),
                                   QMessageBox::Ok);
    on_boton_toggle_clicked();
}

void MainWindow::on_boton_salir_clicked(){
    this->close();
    qApp->quit();
    QApplication::quit();
}

void MainWindow::on_boton_instrucciones_clicked(){
    QString destinationTemp = QDir::fromNativeSeparators(QDir::tempPath());
    if(!destinationTemp.endsWith('/'))
    destinationTemp.append('/');
    QString fileName = "instrucciones";
    if(QFile::exists(destinationTemp + fileName + ".pdf")){
    fileName  = "instrucciones %1";
    unsigned int i=0;
    for(;QFile::exists(destinationTemp + fileName.arg(i) + ".pdf");++i){}
    fileName  = fileName.arg(i);
    }
    QFile::copy(":/info/instrucciones.pdf",destinationTemp + fileName   + ".pdf");
    QDesktopServices::openUrl(QUrl::fromLocalFile(destinationTemp + fileName   + ".pdf"));



}


