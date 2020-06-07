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
        sourcePixmap = QPixmap::fromImage(image[0]);
        sourceImage = image[0];
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
    sourceImage.load(filePath);//QImage load from path
    sourcePixmap = QPixmap::fromImage(sourceImage);

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

    QImage* set = new QImage(sourceImage.width(), sourceImage.height(), QImage::Format_ARGB32);
    for(int i = 0; i < sourceImage.width(); ++i){
        for( int j= 0; j <sourceImage.height(); ++j){
           set->setPixel(i,j, sourceImage.pixel(i, j));
        }
    }
    const int h = ui->label_Display->height();
    const int w = ui->label_Display->width();
    resultImage = *set;
    resultPixmap= QPixmap::fromImage(*set);//get result pixmap
    ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio));
    ui->label_Display->setPixmap(sourcePixmap.scaled(w, h, Qt::KeepAspectRatio));//image display with keeping Original Ratio

}


void MainWindow::project_Action()
{
    int width = sourceImage.width();
    int height = sourceImage.height();
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

                prj1->setPixel(a,b, sourceImage.pixel(i,j));

           }
     }
     prj1->save("D:\\projection.jpg","JPG");
    //Draw Result
     resultImage = *prj1;
     resultPixmap= QPixmap::fromImage(*prj1);//get result pixmap
     int w =  ui->label_ResultDisplay->width();//get display width
     int h = ui->label_ResultDisplay->height();//get display height
     ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio));
}

void MainWindow::Myinterpolation(){//Inverse warping & Bilinear interpolation
    int width = sourceImage.width();
    int height = sourceImage.height();
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
                if(b <0 || a < 0 || a>sourceImage.width() || b > sourceImage.height()){
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
     interImg->save("D:\\inter.jpg","JPG");
     resultImage = *interImg;
     resultPixmap = QPixmap::fromImage(*interImg);//get result pixmap
     w =  ui->label_ResultDisplay->width();//get display width
     h = ui->label_ResultDisplay->height();//get display height
     ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio));
}


//Interpolation함수!!!!!
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

    if(yb> sourceImage.height()-2 || xb>sourceImage.width()-2){//if Out of range
        QColor res = sourceImage.pixelColor(xb, yb);//return padding value
        return res;
    }
    QColor a1 = sourceImage.pixelColor(xb, yb);
    QColor a2 = sourceImage.pixelColor(xb+1, yb);
    QColor a3 = sourceImage.pixelColor(xb, yb+1);
    QColor a4 = sourceImage.pixelColor(xb+1, yb+1);

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
//RGB Convolution함수!!!!!!!!!!!!
QRgb convolute(const QList<int> &kernel, const QImage &image, int x, int y){
  int kernelsize = sqrt(kernel.size());
  qreal total = 0;
  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;
  for(int r = -kernelsize/2 ; r<=kernelsize/2; ++r){
    for(int c = -kernelsize/2; c<=kernelsize/2; ++c){
      int kerVal = kernel.at((kernelsize/2+r)*kernelsize+(kernelsize/2+c));
      total+=kerVal;
      red += qRed(image.pixel(x+c, y+r))*kerVal;
      green += qGreen(image.pixel(x+c, y+r))*kerVal;
      blue += qBlue(image.pixel(x+c, y+r))*kerVal;
    }
  }
  if(total==0)
      return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));
  return qRgb(qBound(0, qRound(red/total), 255), qBound(0, qRound(green/total), 255), qBound(0, qRound(blue/total), 255));
}
QRgb convoluteGauss(const QList<float> &kernel, const QImage &image, int x, int y){
  int kernelsize = sqrt(kernel.size());
  qreal total = 0;
  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;
  for(int r = -kernelsize/2 ; r<=kernelsize/2; ++r){
    for(int c = -kernelsize/2; c<=kernelsize/2; ++c){
      float kerVal = kernel.at((kernelsize/2+r)*kernelsize+(kernelsize/2+c))/256;
      total+=kerVal;
      red += qRed(image.pixel(x+c, y+r))*kerVal;
      green += qGreen(image.pixel(x+c, y+r))*kerVal;
      blue += qBlue(image.pixel(x+c, y+r))*kerVal;
    }
  }
  if(total==0)
      return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));
  return qRgb(qBound(0, qRound(red/total), 255), qBound(0, qRound(green/total), 255), qBound(0, qRound(blue/total), 255));
}
QRgb pixelMultiply(const QImage &image,const QImage &image2, int x, int y){
  qreal total = 0;
  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;

  red = qRed(image.pixel(x, y)) * qRed(image2.pixel(x, y));
  green = qGreen(image.pixel(x, y)) * qRed(image2.pixel(x, y));
  blue = qBlue(image.pixel(x, y)) * qRed(image2.pixel(x, y));

  if(total==0)
      return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//(Rounding (0~255)
  return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//Rounding(0~255)
}
QRgb MainWindow::pixelHarrisMeasure(const QImage &lx2,const QImage &ly2,const QImage &lxy, int x, int y){

  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;
  qreal k = 0.03;

  red = qRed(lx2.pixel(x, y)) * qRed(ly2.pixel(x, y)) - qRed(lxy.pixel(x, y))*qRed(lxy.pixel(x, y))
          - k *(qRed(lx2.pixel(x,y)) +qRed(ly2.pixel(x,y))*(qRed(lx2.pixel(x,y)) +qRed(ly2.pixel(x,y))));
  green = qGreen(lx2.pixel(x, y)) * qGreen(ly2.pixel(x, y)) - qGreen(lxy.pixel(x, y))*qGreen(lxy.pixel(x, y))
          - k *(qGreen(lx2.pixel(x,y)) +qGreen(ly2.pixel(x,y))*(qGreen(lx2.pixel(x,y)) +qGreen(ly2.pixel(x,y))));
  blue = qBlue(lx2.pixel(x, y)) * qBlue(ly2.pixel(x, y)) - qBlue(lxy.pixel(x, y))*qBlue(lxy.pixel(x, y))
          - k *(qBlue(lx2.pixel(x,y)) +qBlue(ly2.pixel(x,y))*(qBlue(lx2.pixel(x,y)) +qBlue(ly2.pixel(x,y))));



  if(red > Threshold && green > Threshold && blue > Threshold){
   return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//Rounding(0~255)
  }else{
      return qRgb(0, 0, 0);
  }

//  return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//Rounding(0~255)
}


//Using Harris corner algorithm
void MainWindow::on_detectButton_clicked()//Detect Keypoint
{
    QList<int> sharpen, blur, laplace, sobeldx, sobeldy;

    QList<float> Gauss;
    sobeldx << -1 << 0 << 1
          << -2 << 0 << 2
          << -1 << 0 << 1;

    sobeldy << 1 << 2 << 1
            << 0 << 0 << 0
            << -1 << -2 << -1;

    Gauss << 1 << 4 << 6 << 4 << 1
          << 4 << 16<< 24 << 16 << 4
          << 6 << 24 << 36 << 24 << 6
          << 4 << 16<< 24<< 16<< 4
          <<1 << 4<< 6<< 4<< 1;

    sharpen << 0 <<  0 <<  0 <<  0 << 0
        << 0 <<  0 << -1 <<  0 << 0
        << 0 << -1 <<  5 << -1 << 0
        << 0 <<  0 << -1 <<  0 << 0
        << 0 <<  0 <<  0 <<  0 << 0;
    blur    << 0 <<  0 <<  1 <<  0 << 0
        << 0 <<  1 <<  3 <<  1 << 0
        << 1 <<  3 <<  7 <<  3 << 1
        << 0 <<  1 <<  3 <<  1 << 0
        << 0 <<  0 <<  1 <<  0 << 0;

    laplace << -1 << -1 <<  -1 << -1 << -1
        << -1 << -1 <<  -1 << -1 << -1
        << -1 << -1 <<  24 << -1 << -1
        << -1 << -1 <<  -1 << -1 << -1
        << -1 << -1 <<  -1 << -1 << -1;


    //Pre. Blur first
    QImage tempBlur = resultImage;//Horizontal Edge
    for(int r=2;r<resultImage.height()-2;r++){
        for(int c=2;c<resultImage.width()-2;c++){
          tempBlur.setPixel(c, r, convolute(blur, resultImage, c, r));
        }
     }

    //1. Get Edge Image
    QImage lx = resultImage;//Horizontal Edge
    for(int r=2;r<resultImage.height()-2;r++){
        for(int c=2;c<resultImage.width()-2;c++){
          lx.setPixel(c, r, convolute(sobeldx, tempBlur, c, r));
        }
     }
    QImage ly = resultImage;//Vertical Edge
    for(int r=2;r<resultImage.height()-2;r++){
        for(int c=2;c<resultImage.width()-2;c++){
          ly.setPixel(c, r, convolute(sobeldy, tempBlur, c, r));
        }
     }

    /////////////////////////////////////////////////////

    //2. Get Squared Edge Image
    QImage lx2_s = resultImage;// x Edge Sqaured
    for(int r=0;r<resultImage.height();r++){
        for(int c=0;c<resultImage.width();c++){
          lx2_s.setPixel(c, r, pixelMultiply(lx,lx, c, r));
        }
     }
     QImage ly2_s = resultImage;//y Edge Squared
     for(int r=0;r<resultImage.height();r++){
         for(int c=0;c<resultImage.width();c++){
           ly2_s.setPixel(c, r,pixelMultiply(ly,ly, c, r));
         }
      }
     QImage lxy_s = resultImage;// X Edge times Y Edge
     for(int r=0;r<resultImage.height();r++){
         for(int c=0;c<resultImage.width();c++){
           lxy_s.setPixel(c, r,pixelMultiply(lx,ly, c, r));
         }
      }
     //////////////////////////////////////////////////////

     //3. Get Gaussian Squared Edge Image
     QImage lx2 = resultImage;//Blurred
     for(int r=2;r<resultImage.height()-2;r++){
         for(int c=2;c<resultImage.width()-2;c++){
           lx2.setPixel(c, r, convoluteGauss(Gauss, lx2_s, c, r));
         }
      }
     QImage ly2 = resultImage;//Blurred
     for(int r=2;r<resultImage.height()-2;r++){
         for(int c=2;c<resultImage.width()-2;c++){
           ly2.setPixel(c, r, convoluteGauss(Gauss, ly2_s, c, r));
         }
      }
     QImage lxy = resultImage;//Blurred
     for(int r=2;r<resultImage.height()-2;r++){
         for(int c=2;c<resultImage.width()-2;c++){
           lxy.setPixel(c, r, convoluteGauss(Gauss, lxy_s, c, r));
         }
      }

     int w =  ui->label_ResultDisplay->width();//get display width
     int h = ui->label_ResultDisplay->height();//get display height
     QPixmap temp = QPixmap::fromImage(lxy);
     ui->label_ResultDisplay->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));
     lx.save("D:\\filter.jpg","JPG");



     //4. Get Corner strength image
     QImage cim = resultImage;//Corner strength image
     for(int r=0;r<resultImage.height();r++){
         for(int c=0;c<resultImage.width();c++){
           cim.setPixel(c, r, pixelHarrisMeasure(lx2, ly2, lxy, c, r));
         }
      }

     //5. Draw point on image
     QImage* set = new QImage(resultImage.width(), resultImage.height(), QImage::Format_ARGB32);
     for(int i = 0; i < resultImage.width(); i++){
         for( int j= 0; j <resultImage.height(); j++){
            set->setPixel(i,j, resultImage.pixel(i, j));
         }
     }

     QPainter painter(set);
     QPen pen;
     pen.setWidth(5);
     pen.setColor(Qt::red);
     painter.setPen(pen);
     for(int r=0;r<resultImage.height();r++){
         for(int c=0;c<resultImage.width();c++){
            if(cim.pixel(c,r)!=qRgb(0, 0, 0)){
                 painter.drawPoint(c,r);
            }
         }
      }
     painter.end();
     //=====================================

     w =  ui->label_ResultDisplay->width();//get display width
     h = ui->label_ResultDisplay->height();//get display height
     temp = QPixmap::fromImage(*set);
     ui->label_ResultDisplay->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));
     //////////////////////////////////////////////////
    /*int width = sourceImage.width();
    int height = sourceImage.height();
    unsigned char * dest_image = new unsigned char[width * height];//gaussian result
    //initialize destination image to zero
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            dest_image[i * width + j] = 0;
        }
    }
    */
}

void MainWindow::on_nextButton_clicked()
{
    if(currentImage ==5){
        currentImage = 0;
    }else{
        currentImage = currentImage +1;
    }
    sourceImage = image[currentImage];
    QPixmap temp;
    temp = QPixmap::fromImage(sourceImage);//get result pixmap
    int w =  ui->label_Display->width();//get display width
    int h = ui->label_Display->height();//get display height
    ui->label_Display->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));
}

void MainWindow::on_prevButton_clicked()
{ if(currentImage ==0){
        currentImage = 5;
    }else{
        currentImage = currentImage -1;
    }
    sourceImage = image[currentImage];
    QPixmap temp;
    temp = QPixmap::fromImage(sourceImage);//get result pixmap
    int w =  ui->label_Display->width();//get display width
    int h = ui->label_Display->height();//get display height
    ui->label_Display->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio));

}


void MainWindow::on_thresholdSlider_valueChanged(int value)
{
    Threshold = value;
    ui->threshold_value->setText(QString::number(value));
}
