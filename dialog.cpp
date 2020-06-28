#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}
void Dialog::sendData(){
   int w =  ui->panoramaLabel->width();//get display width
   int  h = ui->panoramaLabel->height();//get display height
   ui->panoramaLabel->setPixmap(PanoramaPixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void Dialog::on_buttonBox_accepted()
{

}

void Dialog::on_pushButton_clicked()
{
      PanoramaImage.save("D:\\Panorama.jpg","JPG");
}
