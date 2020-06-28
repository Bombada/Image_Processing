//RGB Convolution함수!!!!!!!!!!!!
#include "mainwindow.h"
#include "ui_mainwindow.h"
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


//Pixel끼리 곱 연산을 진행하는 함수(Color space version)
QImage MainWindow::grayscaleImage(QImage image){
    QImage result = image;
    for(int i = 0; i < image.width(); i++){
        for(int j = 0; j < image.height(); j++){
            int gray = qGray(image.pixel(i,j));
            result.setPixel(i, j, QColor(gray, gray, gray).rgb());
        }
    }
    return result;
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
                 painter.drawPoint(c,r);//해당 점에 그린다.
            }
         }
      }
     painter.end();
     //=====================================

     w =  ui->label_ResultDisplay->width();//get display width
     h = ui->label_ResultDisplay->height();//get display height
     resultPixmap = QPixmap::fromImage(*set);
     multiImageResult[currentImage] = *set;

     ui->label_ResultDisplay->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio));
     //////////////////////////////////////////////////
}
