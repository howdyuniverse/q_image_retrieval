#include "ImgViewer.h"

ImgViewerDialog::ImgViewerDialog(QWidget *parent)
        : QDialog(parent)
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);

    QVBoxLayout * mainLay = new QVBoxLayout;
    mainLay->addWidget(scrollArea);

    setLayout(mainLay);

    resize(800, 600);
}

void ImgViewerDialog::setImagePath(QString img_path)
{
    image_path = img_path;

    setWindowTitle(img_path);

    QImage image(image_path);
    if (image.isNull()) {
        QMessageBox::information(this, tr("Вбудований переглядач зображень"),
                                 tr("Не можу завантажити %1.").arg(image_path));
        return;
    }

    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
}
