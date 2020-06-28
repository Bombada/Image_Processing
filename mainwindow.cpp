#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusbar->showMessage("Start Program",4000);


    panorama = new Dialog();

    for(int i = 0; i < 6; i++){
        HistogramVec[i] = new Node[8000];
    }
//    for(int q = 0; q < 6; q++){
//        HistogramVec[q] = new int**[1300];
//        for (int i = 0; i < 1300; i++){
//            HistogramVec[q][i] = new int*[1000];
//            for (int j = 0; j < 1000; j++)
//                HistogramVec[q][i][j] = new int[9];
//        }
//    }
//    for(int q = 0; q < 6; q++){
//        for (int i = 0; i < 1300; i++){
//            for (int j = 0; j < 1000; j++)
//                HistogramVec[q][i][j] = 0;
//        }
//    }



    //Menu
    QMenu *pFileMenu;
    //Open Image menu
  //  pFileMenu = menuBar()->addMenu(tr("Open"));// first Menu is "Open"
  //  QAction *pSlot1 = new QAction(tr("Open Image"),this);//create new menu
  //  pSlot1->setStatusTip(tr("Open Image Raw or Format"));//set status
  // connect(pSlot1, SIGNAL(triggered()), this, SLOT(open_Action()));//set Action
   // pFileMenu->addAction(pSlot1);//Add action to menubar

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



    //Debug
    pFileMenu = menuBar()->addMenu(tr("Debug"));// first Menu is "Open"
    QAction *pSlot1 = new QAction(tr("DrawLine"),this);//create new menu
    pSlot1->setStatusTip(tr("Draw Line between Matched Keypoints"));//set status
    connect(pSlot1, SIGNAL(triggered()), this, SLOT(KeypointDrawLine()));//set Action
    pFileMenu->addAction(pSlot1);//Add action to menubar


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
        TotalImage = filenameList.length();
        QPixmap temp;
        temp = QPixmap::fromImage(image[0]);
        const int h = ui->label_Display->height();
        const int w = ui->label_Display->width();
        sourceImage = image[0];
        sourcePixmap = QPixmap::fromImage(image[0]);
        resultImage = image[0];
        resultPixmap= QPixmap::fromImage(image[0]);//get result pixmap
        ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->label_Display->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));//image display with keeping Original Ratio
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
    set->fill(Qt::black);
    for(int i = 0; i < sourceImage.width(); ++i){
        for( int j= 0; j <sourceImage.height(); ++j){
           set->setPixel(i,j, sourceImage.pixel(i, j));
        }
    }
    const int h = ui->label_Display->height();
    const int w = ui->label_Display->width();
    resultImage = *set;
    resultPixmap= QPixmap::fromImage(*set);//get result pixmap
    ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->label_Display->setPixmap(sourcePixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));//image display with keeping Original Ratio

}


void MainWindow::project_Action()
{
    int width = sourceImage.width();
    int height = sourceImage.height();
    double f = ui->focal_length->value();
    qDebug() << "focal: " << f;


    prj1 = new QImage(width, height, QImage::Format_ARGB32);
    prj1->fill(Qt::black);
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
     multiImageResult[currentImage] = *prj1;
     resultPixmap= QPixmap::fromImage(*prj1);//get result pixmap
     int w =  ui->label_ResultDisplay->width();//get display width
     int h = ui->label_ResultDisplay->height();//get display height
     ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::Myinterpolation(){//Inverse warping & Bilinear interpolation
    int width = sourceImage.width();
    int height = sourceImage.height();
    double f = ui->focal_length->value();

    //Interpolation
    QImage* interImg = new QImage(width, height, QImage::Format_ARGB32);
    interImg->fill(Qt::black);
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
     multiImageResult[currentImage] = *interImg;
     resultPixmap = QPixmap::fromImage(*interImg);//get result pixmap
     w =  ui->label_ResultDisplay->width();//get display width
     h = ui->label_ResultDisplay->height();//get display height
     ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
    for(int i = 1; i < TotalImage; i++){
        on_nextButton_clicked();
        Myinterpolation();
    }
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
      return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//rounding
  return qRgb(qBound(0, qRound(red/total), 255), qBound(0, qRound(green/total), 255), qBound(0, qRound(blue/total), 255));//rounding
}
QRgb convoluteRow1D(const QImage &image, int x, int y){
  qreal total = 0;
  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;
  for(int r = -1 ; r<=1; ++r){
      red += qRed(image.pixel(x,y+r))*r;
  }
  green = red;
  blue = red;
  if(total==0)
      return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//rounding
  return qRgb(qBound(0, qRound(red/total), 255), qBound(0, qRound(green/total), 255), qBound(0, qRound(blue/total), 255));//rounding
}
QRgb convoluteCol1D(const QImage &image, int x, int y){
  qreal total = 0;
  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;
  for(int c = -1 ; c<=1; ++c){
      red += qRed(image.pixel(x+c,y))*c;
  }
  green = red;
  blue = red;
  if(total==0)
      return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//rounding
  return qRgb(qBound(0, qRound(red/total), 255), qBound(0, qRound(green/total), 255), qBound(0, qRound(blue/total), 255));//rounding
}
//Floating Filter convolution 함수
QRgb convoluteGauss(const QList<float> &kernel, const QImage &image, int x, int y){
  int kernelsize = sqrt(kernel.size());
  qreal total = 0;
  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;
  for(int r = -kernelsize/2 ; r<=kernelsize/2; ++r){//Use Kernel size
    for(int c = -kernelsize/2; c<=kernelsize/2; ++c){
      float kerVal = kernel.at((kernelsize/2+r)*kernelsize+(kernelsize/2+c))/256;
      total+=kerVal;
      red += qRed(image.pixel(x+c, y+r))*kerVal;
      green += qGreen(image.pixel(x+c, y+r))*kerVal;
      blue += qBlue(image.pixel(x+c, y+r))*kerVal;
    }
  }
  if(total==0)
      return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//rounding
  return qRgb(qBound(0, qRound(red/total), 255), qBound(0, qRound(green/total), 255), qBound(0, qRound(blue/total), 255));//rounding
}
//Pixel끼리 곱 연산을 진행하는 함수(Color space version)
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

//Harris corner Measurement 연산 진행하는 함수
QRgb MainWindow::pixelHarrisMeasure(const QImage &lx2,const QImage &ly2,const QImage &lxy, int x, int y){

  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;
  qreal k = 0.04;//Kappa 값을 0.04~0.06사이의 값으로 진행한다.

  red = qRed(lx2.pixel(x, y)) * qRed(ly2.pixel(x, y)) - qRed(lxy.pixel(x, y))*qRed(lxy.pixel(x, y))
          - k *(qRed(lx2.pixel(x,y)) +qRed(ly2.pixel(x,y))*(qRed(lx2.pixel(x,y)) +qRed(ly2.pixel(x,y))));
  green = qGreen(lx2.pixel(x, y)) * qGreen(ly2.pixel(x, y)) - qGreen(lxy.pixel(x, y))*qGreen(lxy.pixel(x, y))
          - k *(qGreen(lx2.pixel(x,y)) +qGreen(ly2.pixel(x,y))*(qGreen(lx2.pixel(x,y)) +qGreen(ly2.pixel(x,y))));
  blue = qBlue(lx2.pixel(x, y)) * qBlue(ly2.pixel(x, y)) - qBlue(lxy.pixel(x, y))*qBlue(lxy.pixel(x, y))
          - k *(qBlue(lx2.pixel(x,y)) +qBlue(ly2.pixel(x,y))*(qBlue(lx2.pixel(x,y)) +qBlue(ly2.pixel(x,y))));



  if(red > Threshold && green > Threshold && blue > Threshold){//Threshold값은 member variable로 Slider로 변경되는 값
   return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//Rounding(0~255)
  }else{
      return qRgb(0, 0, 0);
  }

//  return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//Rounding(0~255)
}

QRgb MainWindow::NonMaxima(const QImage &lx2,const QImage &ly2,const QImage &lxy, int x, int y){

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

    //Various filter Mask
    QList<int> sharpen, blur, laplace, sobeldx, sobeldy;//Filter mask

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
    ///////////////////////////////////////


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
     ui->label_ResultDisplay->setPixmap(temp.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
     QImage* m_keyPoints = new QImage(resultImage.width(), resultImage.height(), QImage::Format_ARGB32);
     for(int i = 0; i < resultImage.width(); i++){
         for( int j= 0; j <resultImage.height(); j++){
            set->setPixel(i,j, resultImage.pixel(i, j));

             m_keyPoints->setPixel(i,j,qRgb(0,0,0));//set Keypoint Image white
         }
     }
    multiImageResult[currentImage] = *set;//Image get
    resultImage = *set;

 //키포인트 그리는 곳
     QPainter painter(set);
     QPen pen;
     pen.setWidth(5);
     pen.setColor(Qt::red);
     painter.setPen(pen);
     for(int r=0;r<resultImage.height();r++){
         for(int c=0;c<resultImage.width();c++){
            if(cim.pixel(c,r)!=qRgb(0, 0, 0)){
                 painter.drawPoint(c,r);//해당 점에 그린다.


                 m_keyPoints->setPixel(c,r,qRgb(255,255,255));//set Keypoint
            }
         }
      }
     painter.end();
     //=====================================

     w =  ui->label_ResultDisplay->width();//get display width
     h = ui->label_ResultDisplay->height();//get display height
      //  resultPixmap = QPixmap::fromImage(*set);
     resultPixmap = QPixmap::fromImage(*set);
     multiImageKeypoint[currentImage] = *m_keyPoints;//keyPoint array setting ; 키포인트 클래스이미지 저장!

     ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
     //////////////////////////////////////////////////
}

void MainWindow::on_nextButton_clicked()
{
    if(currentImage ==5){
        currentImage = 0;
    }else{
        currentImage = currentImage +1;
    }

    int w =  ui->label_Display->width();//get display width
    int h = ui->label_Display->height();//get display height

    sourceImage = image[currentImage];
    sourcePixmap = QPixmap::fromImage(sourceImage);//get result pixmap
    ui->label_Display->setPixmap(sourcePixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    if(multiImageResult[currentImage].isNull()){
     resultImage = image[currentImage];
     resultPixmap = QPixmap::fromImage(resultImage);//get result pixmap
    }else{
        resultImage = multiImageResult[currentImage];
        resultPixmap = QPixmap::fromImage(multiImageResult[currentImage]);//get result pixmap
    }
    ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::on_prevButton_clicked()//Multi image select
{ if(currentImage ==0){
        currentImage = 5;
    }else{
        currentImage = currentImage -1;
    }
    int w =  ui->label_Display->width();//get display width
    int h = ui->label_Display->height();//get display height
    sourceImage = image[currentImage];
    sourcePixmap = QPixmap::fromImage(sourceImage);//get result pixmap
    ui->label_Display->setPixmap(sourcePixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    if(multiImageResult[currentImage].isNull()){
     resultImage = image[currentImage];
     resultPixmap = QPixmap::fromImage(resultImage);//get result pixmap
    }else{
        resultImage = multiImageResult[currentImage];
        resultPixmap = QPixmap::fromImage(multiImageResult[currentImage]);//get result pixmap
    }
    ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void MainWindow::on_thresholdSlider_valueChanged(int value)//Harris corner Threshold value
{
    Threshold = value;
    ui->threshold_value->setText(QString::number(value));
}


//6개의 image저장하는 함수.
void MainWindow::on_saveButton_clicked()
{
    multiImageResult[0].save("D:\\result0.jpg","JPG");
    multiImageResult[1].save("D:\\result1.jpg","JPG");
    multiImageResult[2].save("D:\\result2.jpg","JPG");
    multiImageResult[3].save("D:\\result3.jpg","JPG");
    multiImageResult[4].save("D:\\result4.jpg","JPG");
    multiImageResult[5].save("D:\\result5.jpg","JPG");
}

void MainWindow::KeypointDrawLine(){
    QImage* set = new QImage(resultImage.width(), resultImage.height(), QImage::Format_ARGB32);
    //Copy from result Image
    for(int i = 0; i < resultImage.width(); i++){
        for( int j= 0; j <resultImage.height(); j++){
           set->setPixel(i,j, resultImage.pixel(i, j));
        }
    }

    //QPainter on image 'set'
    QPainter painter(set);
    QPen pen;
    pen.setWidth(5);
    pen.setColor(Qt::green);
    painter.setPen(pen);

    painter.drawLine(50, 50, 350, 50);

    for(int r=0;r<resultImage.height();r++){
        for(int c=0;c<resultImage.width();c++){
          // if(cim.pixel(c,r)!=qRgb(0, 0, 0)){
             //   painter.drawPoint(c,r);//해당 점에 그린다.
           //}
        }
     }
    painter.end();
    //=====================================

    int w =  ui->label_ResultDisplay->width();//get display width
    int h = ui->label_ResultDisplay->height();//get display height
    resultPixmap = QPixmap::fromImage(*set);
    multiImageResult[currentImage] = *set;
    ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));

}


void MainWindow::getHoG(int i, int j){

}
//Pixel 제곱끼리 합하고, 루트하는 함수
double SquareOfSum(const QImage &image,const QImage &image2, int x, int y){
  qreal total = 0;
  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;

  red = sqrt(qRed(image.pixel(x, y))*qRed(image.pixel(x, y))  + qRed(image2.pixel(x, y))*qRed(image2.pixel(x, y)));
  green = sqrt(qGreen(image.pixel(x, y))*qGreen(image.pixel(x, y))+ qRed(image2.pixel(x, y))* qRed(image2.pixel(x, y)));
  blue = sqrt(qBlue(image.pixel(x, y))*qBlue(image.pixel(x, y))+ qRed(image2.pixel(x, y))*qRed(image2.pixel(x, y)));

//  qDebug() << "Mag(" << x << ", "<< y <<"): " << red;//출력

  if(total==0)
      return qRgb(qBound(0, qRound(red), 255), qBound(0, qRound(green), 255), qBound(0, qRound(blue), 255));//(Rounding (0~255)
  return (double)red;//Rounding(0~255)
}
//Pixel의 atan하는 함수
double ArctanPixel(const QImage &image,const QImage &image2, int x, int y){
  qreal red = 0;
  qreal green = 0;
  qreal blue = 0;

  double gx = qRed(image.pixel(x,y));
  double gy = qRed(image2.pixel(x,y));
  double theta = atan2(gy,gx);


 // qDebug() << "Dir: (" << gx << ", "<< gy <<"): " << theta;//출력

  return theta;
}

void MainWindow::getBlockPosition(int i, int j){// Calculate 8x8 block's direction and maginitude
    //Various filter Mask
    QList<int> sharpen, blur, laplace, sobeldx, sobeldy;//Filter mask

    QList<float> Gauss;
    sobeldx << -1 << 0 << 1;

    sobeldy << 1
            << 0
            << -1;

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
    ///////////////////////////////////////

    //0. Get gray Image
    QImage grayImage = resultImage;
    for(int i = 0; i < resultImage.width(); i++){
        for(int j = 0; j< resultImage.height(); j++){
            float gray = 0.299 * qRed(resultImage.pixel(i, j)) + 0.587 * qGreen(resultImage.pixel(i, j )) + 0.114 * qBlue(resultImage.pixel(i, j));
            QColor color;
            color.setRgb(gray, gray, gray);
            grayImage.setPixelColor(i, j , color);//set to Gray Image
        }
    }
    QImage gx = grayImage;//Vertical Edge
    QImage gy = grayImage;//Horizontal Edge



    //1. Find Block Position
    int w = resultImage.width();
    int h = resultImage.height();
    double **direction;//Dynamic 2D Array
    double **magnitude;//Dynamic 2D Array
    direction = new double*[w];
    for (int i = 0; i < w; i++)
        direction[i] = new double[h];

    magnitude = new double*[w];
    for (int i = 0; i < w; i++)
        magnitude[i] = new double[h];

    int bx = resultImage.width()/8;
    int by = resultImage.height()/8;
    for(by = 2; by <resultImage.height()-10; by+=8){//8 블록 만큼 이동
       for(bx = 2; bx<resultImage.width()-10; bx+=8){// 8 블록 만큼이동
            if(i >= bx && i < bx+8 && j >= by && j < by+8){//if i is in the block

                //Sobel Filtering
                for(int r=by;r<by+8;r++){
                    for(int c=bx;c<bx+8;c++){
                      gx.setPixel(c, r, convoluteCol1D(grayImage, c, r));
                      gy.setPixel(c, r, convoluteRow1D(grayImage, c, r));
                    }
                 }

                //Magnitude and Direction
                for(int r=by;r<by+8;r++){
                    for(int c=bx;c<bx+8;c++){
                      magnitude[c][r] = SquareOfSum(gx,gy,c,r);//Magnitude
                      direction[c][r] = ArctanPixel(gx,gy,c,r);//Arctan(Direction)
                    }

                 }
               // qDebug()<<"HoG: block";
                for(int r=by;r<by+8;r++){
                    for(int c=bx;c<bx+8;c++){
                        double t = direction[c][r];
                        int dire = (int)direction[c][r]/20;
                        int res = (int)direction[c][r]%20;
                       // qDebug() << j*w+i;
                        if(res>0){//if between 2 slot
                            HistogramVec[currentImage][NodeCount[currentImage]].x = i;
                            HistogramVec[currentImage][NodeCount[currentImage]].y = j;
                            if(dire == 8){
                               HistogramVec[currentImage][NodeCount[currentImage]].value[dire] = (int)magnitude[c][r]/2;
                               HistogramVec[currentImage][NodeCount[currentImage]].value[0] = (int)magnitude[c][r]/2;
                            }
                            else{
                             HistogramVec[currentImage][NodeCount[currentImage]].value[dire] = (int)magnitude[c][r]/2;
                             HistogramVec[currentImage][NodeCount[currentImage]].value[dire+1] = (int)magnitude[c][r]/2;
                            }
                        }else{//if no residual
                            HistogramVec[currentImage][NodeCount[currentImage]].value[dire] = (int)magnitude[c][r];
                        }
                        if(NodeCount[currentImage] >= 7999){
                            qDebug()<<"HoG: Count End";
                            return;
                        }
                        NodeCount[currentImage] ++;//count Up;

                    }
                    //qDebug()<<"HoG: block";
                 }
                //qDebug()<<"HoG: Complete";


            }
       }
    }


   w =  ui->label_ResultDisplay->width();//get display width
   h = ui->label_ResultDisplay->height();//get display height
   resultPixmap = QPixmap::fromImage(gx);//setPixmap

   ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // double a = f*(atan2((double)(i-(width/2)), f))+(width/2);
}


//HoG Calculate
void MainWindow::on_HogButton_clicked()
{

    for(int r=2;r< multiImageKeypoint[currentImage].height()-2;r++){//가장자리는 제외하고
        for(int c=2;c< multiImageKeypoint[currentImage].width()-2;c++){//가장자리는 제외하고
          if(multiImageKeypoint[currentImage].pixel(c,r)!=qRgb(0, 0, 0)){//만약 keypoint가 있는 값이면
                getBlockPosition(c,r);//calculate Keypoint Block HistogramVector
          }
        }
     }
    NormHisVec();

}

void MainWindow::Euqlidian(){

}//Calculate Euclidian
void MainWindow::NormHisVec(){
    for(int i = 0; i < NodeCount[currentImage]; i++){
           float sum = 0;
           float square = 0;
           for(int j = 0; j <9; j++){
              square =  HistogramVec[currentImage][i].value[j] * HistogramVec[currentImage][i].value[j];
              sum += square;
           }
           float result = sqrt(sum);
           for(int j = 0; j <9; j++){
             HistogramVec[currentImage][i].value[j] =  HistogramVec[currentImage][i].value[j] / result;
           }
       }
}
void MainWindow::on_MatchingButton_clicked(){
//    for(int i = 0; i < 6; i++){
//        AverageDistance[i].x = 500;
//        AverageDistance[i].y = 0;
//    }



    AverageDistance[1].x = 390;
    AverageDistance[1].y = -10;
    AverageDistance[2].x = 400;
    AverageDistance[2].y = -15;
    AverageDistance[3].x = 410;
    AverageDistance[3].y = -10;
    AverageDistance[4].x = 360;
    AverageDistance[4].y = 10;
    AverageDistance[5].x = 400;
    AverageDistance[5].y = -20;

    AverageDistance[0].x = 380;//obsolete
    AverageDistance[0].y = 100;//obsolete

    int w = sourceImage.width()*2.5;
    int h = sourceImage.height()*1.35;

    QImage* final = new QImage(w, h, QImage::Format_ARGB32);
    final->fill(Qt::black);

    int sourceX = sourceImage.width();
    int sourceY = sourceImage.height();

    int offsetY = 100;
    int offsetX = 0;
    float a = 0.5f;
    float b = 0.5f;
    int q ;
    for(q = 1; q<TotalImage; q++){//모든 이미지에 대해서
        for(int r=0;r<sourceY;r++){//각 이미지의 크기에 따라
            for(int c=0;c<sourceX;c++){
              if(multiImageResult->pixelColor(c,r)!= Qt::black){//배경일 경우 제거
                  if(final->pixelColor(c+offsetX, r+offsetY)!=Qt::black){
                     float test = fminf(500.5, 400.4);
                     if(c < sourceX/2){
                       b = (float)c/sourceX * 2;
                     }
                     else{
                        b = (1-(float)c/sourceX)*2;
                     }
                     if(TotalImage>=5){
                         if(TotalImage == 5 && q == 2){
                              b = 0;
                         }
                         if(TotalImage == 6 && q == 3){
                             b = 0;
                         }

                     }


                      a = 1-b;
                     QRgb result = qRgb(qRed(final->pixel(c+offsetX, r+offsetY))*a +qRed(multiImageResult[q].pixel(c,r))*b,
                          qGreen(final->pixel(c+offsetX, r+offsetY))*a+ qGreen(multiImageResult[q].pixel(c,r))*b,
                         qBlue(final->pixel(c+offsetX, r+offsetY))*a+qBlue(multiImageResult[q].pixel(c,r))*b);
                 // qAlpha(final->pixel(c+offsetX, r+offsetY)*a + qAlpha(multiImageResult[0].pixel(c,r)*b))
                 //QRgb x = qRgba(qRed(multiImageResult[0].pixel(c,r)),  qGreen(multiImageResult[0].pixel(c,r)),qBlue(multiImageResult[0].pixel(c,r)), 100);
                     final->setPixel(c+offsetX, r+offsetY, result);
                  }
                 else{
                      QRgb x = qRgba(qRed(multiImageResult[q].pixel(c,r)),  qGreen(multiImageResult[q].pixel(c,r)),
                              qBlue(multiImageResult[q].pixel(c,r)), 255);
                      final->setPixel(c+offsetX, r+offsetY, x);
                  }
              }
            }
         }
        offsetX += AverageDistance[q].x;
        offsetY += AverageDistance[q].y;
    }
    // Image 0 add


    for(int r=0;r<sourceY;r++){
        for(int c=0;c<sourceX;c++){
          if(multiImageResult->pixelColor(c,r)!= Qt::black){
              if(final->pixelColor(c+offsetX, r+offsetY)!=Qt::black){
                //  qDebug() <<"a: "<< qRed(final->pixel(c+offsetX, r+offsetY))*a;
               //   qDebug() <<"b: "<< qRed(multiImageResult[0].pixel(c,r))*b;

                  if(c < sourceX/2){
                    b = (float)c/sourceX * 2;
                  }
                  else{
                     b = (1-(float)c/sourceX)*2;
                  }
                  a = 1-b;
                 QRgb result = qRgb(qRed(final->pixel(c+offsetX, r+offsetY))*a +qRed(multiImageResult[0].pixel(c,r))*b,
                      qGreen(final->pixel(c+offsetX, r+offsetY))*a+ qGreen(multiImageResult[0].pixel(c,r))*b,
                     qBlue(final->pixel(c+offsetX, r+offsetY))*a+qBlue(multiImageResult[0].pixel(c,r))*b);
             // qAlpha(final->pixel(c+offsetX, r+offsetY)*a + qAlpha(multiImageResult[0].pixel(c,r)*b))
             //QRgb x = qRgba(qRed(multiImageResult[0].pixel(c,r)),  qGreen(multiImageResult[0].pixel(c,r)),qBlue(multiImageResult[0].pixel(c,r)), 100);
                 final->setPixel(c+offsetX, r+offsetY, result);
              }
             else{
                  QRgb x = qRgba(qRed(multiImageResult[0].pixel(c,r)),  qGreen(multiImageResult[0].pixel(c,r)),
                          qBlue(multiImageResult[0].pixel(c,r)), 255);
                  final->setPixel(c+offsetX, r+offsetY, x);

              }
          }
        }
     }


//    if(multiImageResult[currentImage].isNull()){
//     resultImage = image[currentImage];
//     resultPixmap = QPixmap::fromImage(resultImage);//get result pixmap
//    }else{
//        resultImage = multiImageResult[currentImage];
//        resultPixmap = QPixmap::fromImage(multiImageResult[currentImage]);//get result pixmap
//    }
//    ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio));
//}


    panorama->PanoramaImage = *final;
    panorama->PanoramaPixmap = QPixmap::fromImage(*final);
    panorama->sendData();
    panorama->show();

}
