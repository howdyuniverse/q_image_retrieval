#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include <QDebug>

#include "ImgHistogram.h"

class ImgViewerDialog;
class CorrSpectrum;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow();

private slots:
    void openImage();
    void openCatalog();
    void searchImages();

    void selectAllImages();
    void selectNoneImages();
    void selectInvertImages();
    void moveSelectedImages();
    void copySelectedImages();
    void delSelectedImages();
    void syncFolder();

    void winSettingsBlock();
    void winSearchBlock();
    void winOpenBlock();

    void about();
    void help();

    //
    void showFullImg(QListWidgetItem *item);

private:
    ImgViewerDialog * imgViewer;

    ImgHistogram * query_hist;
    CorrSpectrum * squery_hist;
    void searchCorrSpectrum();

    QString image_path;
    QString folder_path;

    void createActions();
    void createMenus();
    void initConnections();
    void initLayouts();
    void initWidgets();
    void initStatusBar();

    void updateImgList();
    void sortImgList();
    void addImageToList(QString img_path, QString img_name);

    QWidget *centralWidget;

    // Image table
    QListWidget * imgList;

    // Menu
    QMenu * mainMenu;
    QAction * selectQueryImg;
    QAction * selectCatalog;
    QAction * quitAction;

    QMenu * editMenu;
    QAction * selectAllAction;
    QAction * selectNoneAction;
    QAction * selectInvertAction;
    QAction * delSelectedAction;
    QAction * moveSelectedAction;
    QAction * copySelectedAction;

    QMenu * folderMenu;
    QAction * syncFolderAction;
    QAction * clearCacheAction;
    QAction * updateCacheAction;

    QMenu * viewMenu;
    QMenu * imgViewerMenu;
        QActionGroup * imgViewGroup;
        QAction * imgViewrStdAct;
        QAction * imgViewrProgAct;
    QAction * viewSettingsBlockAct;
    QAction * viewSearchBlockAct;
    QAction * viewOpenBlockAct;

    QMenu * helpMenu;
    QAction * helpAction;
    QAction * aboutAction;

    // Group boxes
    QGroupBox * prevImgBox;
    QGroupBox * openBox;
    QGroupBox * searchBox;
    QGroupBox * leftImgBox;
    QGroupBox * rightImgBox;

    //
    QLabel *imgLabel;
    QLabel *imgPathLabel;
    QLabel *dirPathLabel;
    // Buttons
    QPushButton * openQueryImgButton;
    QPushButton * openCatalogButton;
    QPushButton * searchButton;
    QPushButton * delImagesButton;
    QPushButton * syncFolderButton;

    QLabel *searchLabel;
    QComboBox *searchMethodCBox;
};

#endif // MAINWINDOW_H
