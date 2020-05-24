#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusbar->showMessage("Start Program",4000);

    //Menu
    QMenu *pFileMenu;
    //Open Image menu
    pFileMenu = menuBar()->addMenu(tr("Open"));// first Menu is "Open"
    QAction *pSlot1 = new QAction(tr("Open Image"),this);//create new menu
    pSlot1->setStatusTip(tr("Open Image Raw or Format"));//set status
    connect(pSlot1, SIGNAL(triggered()), this, SLOT(open_Action()));//set Action
    pFileMenu->addAction(pSlot1);//Add action to menubar

    //Open Multiple Image menu
    pFileMenu = menuBar()->addMenu(tr("Open Multi image"));// first Menu is "Open"
    QAction *pSlot3 = new QAction(tr("Open Multi Image"),this);//create new menu
    pSlot3->setStatusTip(tr("Open Image Raw or Format"));//set status
    connect(pSlot3, SIGNAL(triggered()), this, SLOT(open_Multiple_Action()));//set Action
    pFileMenu->addAction(pSlot3);//Add action to menubar


    //Cylindrical Projection menu
    //Menu
    pFileMenu = menuBar()->addMenu(tr("Projection"));// first Menu is "Open"
    QAction *pSlot2 = new QAction(tr("Project Image"),this);//create new menu
    pSlot2->setStatusTip(tr("Project image on cylinder space"));//set status
    connect(pSlot2, SIGNAL(triggered()), this, SLOT(project_Action()));//set Action
    pFileMenu->addAction(pSlot2);//Add action to menubar

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::open_Multiple_Action(){//Image open
    QFileDialog dlg2;
    QStringList filenameList = dlg2.getOpenFileNames(this, "Load Image", "", "Image Files (*.png *.jpg *.bmp *.raw)");

    if(!filenameList.isEmpty()){
        for(int i =0; i <filenameList.length(); i++){
            QString str = filenameList.at(i);
            image[i].load(str);
        }
        QPixmap temp;
        temp = QPixmap::fromImage(image[0]);
        const int h = ui->label_Display->height();
        const int w = ui->label_Display->width();
        imgPixmap1 = QPixmap::fromImage(image[0]);
        image1 = image[0];
        ui->label_Display->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));//image display with keeping Original Ratio
    }

}

void MainWindow::open_Action()//Image open
{
    qDebug() << "Open Image Action";
    ui->statusbar->showMessage("show file Dialog",4000);
    QFileDialog dlg;
    QString filePath = dlg.getOpenFileName(this, "Load Image", "", "Image Files (*.png *.jpg *.bmp *.raw)");
    QString fileName = filePath.section("/", -1);

    //Read JPG
   /* QByteArray arr1;
    uchar* data1;
    int size = arr1.size();
    data1 = (uchar *)malloc(size);
    memcpy(data1, reinterpret_cast<uchar *>(arr1.data()), size);
    QPixmap res;
    res.loadFromData(data1, size, "JPG");
    QImage* temp;
    temp = new QImage(data1, img.width(), img.height(), QImage::Format_ARGB32);
    res = QPixmap::fromImage(*temp);
    ui->label_ResultDisplay->setPixmap(res);
*/

    qDebug()<<"Format Image is Selected";
    image1.load(filePath);//QImage load from path
    imgPixmap1 = QPixmap::fromImage(image1);

    //Read Image to Qbyte array
    QFile file(filePath);
    QByteArray arr1;
    if(file.open(QFile::ReadOnly)){
      arr1 = file.readAll();
      qDebug() << "Image read in QByteArray";
    }



    //********************Data Converting Code Sample******************************
   // QImage imgTemp(imgPixmap1.toImage().convertToFormat(QImage::Format_ARGB32));// QPixmap to QImage
   // QImage* readImage = new QImage((const unsigned char*)arr1.data(), image1.width(), image1.height(), QImage::Format_ARGB32); // QByteArray to QImage
   //*********************************************************************************

    QImage* set = new QImage(image1.width(), image1.height(), QImage::Format_ARGB32);
    for(int i = 0; i < image1.width(); ++i){
        for( int j= 0; j <image1.height(); ++j){
           set->setPixel(i,j, image1.pixel(i, j));
        }
    }
    QPixmap temp;
     temp = QPixmap::fromImage(*set);
    const int h = ui->label_Display->height();
    const int w = ui->label_Display->width();

    ui->label_ResultDisplay->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));
    ui->label_Display->setPixmap(imgPixmap1.scaled(w, h, Qt::KeepAspectRatio));//image display with keeping Original Ratio

}


void MainWindow::project_Action()
{
    int width = image1.width();
    int height = image1.height();
    double f = ui->focal_length->value();
    qDebug() << "focal: " << f;
    int factor = 0; // scale factor(radius of the cylinder)

    prj1 = new QImage(width, height, QImage::Format_ARGB32);

    //Forward Warping
    for(int i = 0; i < width; ++i){
           for(int j = 0; j < height; ++j){
                //int theta = atan2((double)(i-(width/2)), f);
                double a = f*(atan2((double)(i-(width/2)), f))+(width/2);
                double b = f*(j-height/2)/sqrt(pow(i-width/2,2)+pow(f,2)) + height/2;

                prj1->setPixel(a,b, image1.pixel(i,j));

           }
     }
    prj1->save("D:\\projection.jpg","JPG");
    //Draw Result
     QPixmap temp;
     temp = QPixmap::fromImage(*prj1);//get result pixmap
     int w =  ui->label_ResultDisplay->width();//get display width
     int h = ui->label_ResultDisplay->height();//get display height
     ui->label_ResultDisplay->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));
}

void MainWindow::Myinterpolation(){//Inverse warping & Bilinear interpolation
    int width = image1.width();
    int height = image1.height();
    double f = ui->focal_length->value();

    //Interpolation
    QImage* interImg = new QImage(width, height, QImage::Format_ARGB32);
    for(int i = 0; i < width; ++i){
            for(int j = 0; j < height; ++j){
               //inverse Warping test
                double test = (i- (width/2))/f;
                double a = f*(tan(test))+ (width/2);

                double test2 = pow(a-(width/2),2) + pow(f,2);
                double root = sqrt(test2);
                double b = (j-height/2)* root/f + height/2;
                if(b <0 || a < 0 || a>image1.width() || b > image1.height()){
                    //qDebug() << a << b;
                    continue;
                }

                QColor inter_Color = Bilinear(a, b);// Call Bilinear interpolation function
                //interImg->setPixelColor(a,b, inter_Color);
                interImg->setPixelColor(i,j, inter_Color);
            }
     }
    int w =  ui->label_ResultDisplay->width();//get display width
    int h = ui->label_ResultDisplay->height();//get display height

    //Draw Result
     QPixmap interResult;
     interImg->save("D:\\inter.jpg","JPG");
     interResult = QPixmap::fromImage(*interImg);//get result pixmap
     w =  ui->label_ResultDisplay->width();//get display width
     h = ui->label_ResultDisplay->height();//get display height
     ui->label_ResultDisplay->setPixmap(interResult.scaled(w, h, Qt::KeepAspectRatio));
}










QColor MainWindow::Bilinear(double x, double y){
    /*QRgb a1 = image1.pixel(x, y);
    QRgb a2 = image1.pixel(x+1, y);
    QRgb a3 = image1.pixel(x, y+1);
    QRgb a4 = image1.pixel(x+1, y+1);
    QRgb res;
    res =  qRgb(qRed(a1)+qRed(a2),qGreen(a1),qBlue(a1));*/

  //  QVector<QRgb> v = image1.colorTable();// get color table
    int xb = (int)x;//integer only
    int yb = (int)y;

    if(yb> image1.height()-2 || xb>image1.width()-2){//if Out of range
        QColor res = image1.pixelColor(xb, yb);//return padding value
        return res;
    }
    QColor a1 = image1.pixelColor(xb, yb);
    QColor a2 = image1.pixelColor(xb+1, yb);
    QColor a3 = image1.pixelColor(xb, yb+1);
    QColor a4 = image1.pixelColor(xb+1, yb+1);

    double xr = x-xb;
    double yr = y-yb;
    //x half pel
    QColor step1((a2.red()-a1.red())*xr + a1.red(),(a2.green()-a1.green())*xr + a1.green(), (a2.blue()-a1.blue())*xr + a1.blue());
    //x (y+1) half pel
    QColor step2((a4.red()-a3.red())*xr + a3.red(),(a4.green()-a3.green())*xr + a3.green(), (a4.blue()-a3.blue())*xr + a3.blue());
    //Quater pel
    QColor res((step2.red()-step1.red())*yr + step1.red(),(step2.green()-step1.green())*yr + step1.green(),(step2.blue()-step1.blue())*yr + step1.blue());

    return res;
}
void MainWindow::on_focal_length_valueChanged(int value)
{
    ui->label_focal_value->setText(QString::number(value));

}

void MainWindow::on_ProjectionButton_clicked()
{
    project_Action();
}

void MainWindow::on_InterpolationButton_clicked()
{
    Myinterpolation();
}

void MainWindow::on_pushButton_2_clicked()//Previous image setting
{
    if(currentImage ==0){
        currentImage = 5;
    }else{
        currentImage = currentImage -1;
    }
    image1 = image[currentImage];
    QPixmap temp;
    temp = QPixmap::fromImage(image1);//get result pixmap
    int w =  ui->label_Display->width();//get display width
    int h = ui->label_Display->height();//get display height
    ui->label_Display->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));
}

void MainWindow::on_pushButton_clicked()//Next image button
{
    if(currentImage ==5){
        currentImage = 0;
    }else{
        currentImage = currentImage +1;
    }
    image1 = image[currentImage];
    QPixmap temp;
    temp = QPixmap::fromImage(image1);//get result pixmap
    int w =  ui->label_Display->width();//get display width
    int h = ui->label_Display->height();//get display height
    ui->label_Display->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));

}
