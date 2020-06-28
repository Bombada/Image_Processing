    #ifndef MAINWINDOW_H
    #define MAINWINDOW_H

    #include <QMainWindow>
    #include <QtWidgets>
    #include <math.h>
    #include <cmath>
    #include <iostream>
    #include <dialog.h>

    QT_BEGIN_NAMESPACE
    namespace Ui { class MainWindow; }
    QT_END_NAMESPACE



    class MainWindow : public QMainWindow
    {
        Q_OBJECT


    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        QPixmap sourcePixmap;
        QImage sourceImage;
        QPixmap resultPixmap;
        QImage resultImage;
        QByteArray arr1;
        QImage* prj1;//Projection Image
        QImage image[6];
        QImage multiImageResult[6];

        QImage multiImageKeypoint[6];
        int currentImage = 0;

        Dialog* panorama;
        int TotalImage = 0;
        struct Node{
           int x = 0;
           int y = 0;
           double value[9] = {0, };
           bool Matched = false;
        };

        Node AverageDistance[6];
        int NodeCount[6] = {0, };
        Node* HistogramVec[6];//Histogram Vector
        int Threshold = 14200;
    private slots:
        void open_Action();//menu open button
        void open_Multiple_Action();//open Multiple image

        void project_Action();//menu projection button

        QColor Bilinear(double x, double y);

        void on_focal_length_valueChanged(int value);
        void Myinterpolation();

        void on_ProjectionButton_clicked();

        void on_InterpolationButton_clicked();

        void on_nextButton_clicked();

        void on_prevButton_clicked();

        void on_detectButton_clicked();

        void on_thresholdSlider_valueChanged(int value);
        QImage MainWindow::grayscaleImage(QImage image);
        QRgb pixelHarrisMeasure(const QImage &lx2,const QImage &ly2,const QImage &lxy, int x, int y);
        QRgb NonMaxima(const QImage &lx2,const QImage &ly2,const QImage &lxy, int x, int y);
        void on_saveButton_clicked();
        void KeypointDrawLine();//Keypoint Matching Draw line(for Debug)
        void getHoG(int i, int j);
        void getBlockPosition(int i, int j);
        void on_HogButton_clicked();//(Hog Algorithm)
        void Euqlidian();//Calculate Euclidian
        void NormHisVec();
        void on_MatchingButton_clicked();

    private:
        Ui::MainWindow *ui;
    };
    #endif // MAINWINDOW_H
