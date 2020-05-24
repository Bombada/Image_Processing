    #ifndef MAINWINDOW_H
    #define MAINWINDOW_H

    #include <QMainWindow>
    #include <QtWidgets>
    #include <math.h>
    #include <cmath>
    #include <iostream>

    QT_BEGIN_NAMESPACE
    namespace Ui { class MainWindow; }
    QT_END_NAMESPACE



    class MainWindow : public QMainWindow
    {
        Q_OBJECT


    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        QPixmap imgPixmap1;
        QImage image1;
        QByteArray arr1;
        QImage* prj1;//Projection Image
        QImage image[6];
        int currentImage = 0;
    private slots:
        void open_Action();//menu open button
        void open_Multiple_Action();//open Multiple image

        void project_Action();//menu projection button

        QColor Bilinear(double x, double y);

        void on_focal_length_valueChanged(int value);
        void Myinterpolation();

        void on_ProjectionButton_clicked();

        void on_InterpolationButton_clicked();



        void on_pushButton_2_clicked();

        void on_pushButton_clicked();

    private:
        Ui::MainWindow *ui;
    };
    #endif // MAINWINDOW_H
