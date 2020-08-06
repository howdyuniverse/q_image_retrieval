#ifndef IMGVIEWER_H
#define IMGVIEWER_H

#include <QtGui>

class ImgViewerDialog : public QDialog
{
        Q_OBJECT

public:
        ImgViewerDialog(QWidget *parent = 0);
        void setImagePath(QString img_path);

private:
        QString image_path;
        QLabel *imageLabel;
        QScrollArea *scrollArea;
};

#endif // IMGVIEWER_H
