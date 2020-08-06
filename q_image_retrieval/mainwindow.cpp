#include "mainwindow.h"
#include "ImgViewer.h"

#include "PearsonCorr.h"

MainWindow::MainWindow()
{
    imgViewer = new ImgViewerDialog;

    //
    query_hist = new ImgHistogram;
    squery_hist = new CorrSpectrum;

    //
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    initWidgets();
    initLayouts();

    createMenus();
    createActions();
    initConnections();

    setWindowTitle(tr("Image retrieval"));
    setWindowIcon(QIcon(":/icon.png"));
    setMinimumSize(690, 480);
    resize(870, 600);
}

void MainWindow::createMenus()
{
    mainMenu    = menuBar()->addMenu(tr("&Файл"));
    editMenu  = menuBar()->addMenu(tr("&Редагування"));
    folderMenu  = menuBar()->addMenu(tr("&Папка"));
    viewMenu    = menuBar()->addMenu(tr("&Вигляд"));
    imgViewerMenu = viewMenu->addMenu(tr("П&ереглядач зображень"));
    helpMenu    = menuBar()->addMenu(tr("&Довідка"));
}

void MainWindow::createActions()
{
    // File menu
    selectQueryImg = mainMenu->addAction(tr("Вибрати &зображення..."));
    selectQueryImg->setShortcut(tr("Ctrl+P"));
    selectCatalog = mainMenu->addAction(tr("Вибрати &каталог..."));
    selectCatalog->setShortcut(tr("Ctrl+F"));
    mainMenu->addSeparator();
    quitAction   = mainMenu->addAction(tr("&Вихід"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    // Edit menu
    selectAllAction = editMenu->addAction(tr("Виділити все"));
    selectAllAction->setShortcut(tr("Ctrl+A"));
    selectNoneAction = editMenu->addAction(tr("Скинути виділення"));
    selectNoneAction->setShortcut(tr("Ctrl+D"));
    selectInvertAction = editMenu->addAction(tr("Інвертувати виділення"));
    selectInvertAction->setShortcut(tr("Ctrl+I"));
    editMenu->addSeparator();
    moveSelectedAction = editMenu->addAction(tr("Перемістити виділене..."));
    moveSelectedAction->setShortcut(tr("Ctrl+X"));
    copySelectedAction = editMenu->addAction(tr("Копіювати вибране..."));
    copySelectedAction->setShortcut(tr("Ctrl+C"));
    delSelectedAction = editMenu->addAction(tr("Видалити вибране"));
    delSelectedAction->setShortcut(tr("Del"));

    // Folder menu
    syncFolderAction = folderMenu->addAction(tr("Синхронізувати каталог"));
    clearCacheAction = folderMenu->addAction(tr("Очистити кеш"));
    updateCacheAction = folderMenu->addAction(tr("Оновити кеш"));


    // View
    imgViewGroup = new QActionGroup(this);
    imgViewrStdAct = imgViewerMenu->addAction(tr("Стандартний"));
    imgViewrStdAct->setCheckable(true);
    imgViewrStdAct->setActionGroup(imgViewGroup);
    imgViewrProgAct = imgViewerMenu->addAction(tr("Вбудований"));
    imgViewrProgAct->setCheckable(true);
    imgViewrProgAct->setActionGroup(imgViewGroup);
    imgViewrProgAct->setChecked(true);

    viewMenu->addSeparator();

    viewSettingsBlockAct = viewMenu->addAction(tr("Панель налаштувань"));
    viewSettingsBlockAct->setCheckable(true);
    viewSettingsBlockAct->setChecked(true);
    viewSettingsBlockAct->setShortcut(tr("Alt+1"));
    viewSearchBlockAct  = viewMenu->addAction(tr("Блок пошуку"));
    viewSearchBlockAct->setCheckable(true);
    viewSearchBlockAct->setChecked(true);
    viewOpenBlockAct    = viewMenu->addAction(tr("Блок відкриття"));
    viewOpenBlockAct->setCheckable(true);
    viewOpenBlockAct->setChecked(true);

    //
    helpAction  = helpMenu->addAction(tr("Допомога"));
    helpAction->setShortcut(tr("F1"));
    aboutAction = helpMenu->addAction(tr("Про програму"));
}

void MainWindow::initWidgets()
{
    imgList = new QListWidget;
    imgList->setViewMode(QListView::IconMode);
    imgList->setMovement(QListView::Static);
    imgList->setResizeMode(QListView::Adjust);
    imgList->setIconSize(QSize(140,140));
    imgList->setGridSize(QSize(180,180));

    imgList->setFlow(QListView::LeftToRight);
    //---
    imgLabel = new QLabel(tr("[Оберіть зображення]"));
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setFrameShape(QFrame::Box);
    imgLabel->setFixedSize(220, 200);

    imgPathLabel = new QLabel(tr("Зображення: [None]"));
    imgPathLabel->setAlignment(Qt::AlignLeft);
    dirPathLabel = new QLabel(tr("Каталог: [None]"));
    dirPathLabel->setAlignment(Qt::AlignRight);

    searchLabel = new QLabel(tr("Метод пошуку:"));
    searchMethodCBox = new QComboBox;
    searchMethodCBox->addItem(tr("Intersection RGB"));
    searchMethodCBox->addItem(tr("Euclidean RGB"));
    searchMethodCBox->addItem(tr("Intersection HSV"));
    searchMethodCBox->addItem(tr("Euclidean HSV"));
    searchMethodCBox->addItem(tr("Spectrum Correlation"));

    // Buttons
    openQueryImgButton  = new QPushButton(tr("Відкрити зображ."));
    openCatalogButton   = new QPushButton(tr("Відкрити каталог"));
    searchButton        = new QPushButton(tr("Почати пошук!"));

    delImagesButton     = new QPushButton(tr("Видал. виділене"));
    syncFolderButton    = new QPushButton(tr("Синхр. каталог"));
}

void MainWindow::initLayouts()
{
    // Image Preview
    prevImgBox = new QGroupBox(tr("Зображення"));
    QVBoxLayout *prevImgBoxLayout = new QVBoxLayout;
    //
    prevImgBoxLayout->addWidget( imgLabel );
    prevImgBoxLayout->addWidget( imgPathLabel );
    prevImgBox->setLayout(prevImgBoxLayout);

    // Open Image/Catalog buttons
    openBox = new QGroupBox(tr("Відкрити..."));
    QHBoxLayout *openBoxLayout = new QHBoxLayout;
    openBoxLayout->addWidget( openQueryImgButton );
    openBoxLayout->addWidget( openCatalogButton );
    openBox->setLayout(openBoxLayout);

    // Search
    searchBox = new QGroupBox(tr("Пошук"));
    QHBoxLayout *searchMethodsLayout = new QHBoxLayout;
    searchMethodsLayout->addWidget( searchLabel );
    searchMethodsLayout->addWidget( searchMethodCBox );
    QVBoxLayout *searchMethodLayout = new QVBoxLayout;
    searchMethodLayout->addLayout( searchMethodsLayout) ;
    searchMethodLayout->addWidget( searchButton );
    searchBox->setLayout(searchMethodLayout);


    // Left Layout
    leftImgBox = new QGroupBox(tr("Налаштування"));
    leftImgBox->setFixedWidth(260);
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget( prevImgBox );
    //leftLayout->addWidget( histImgBox );
    leftLayout->addWidget( openBox );
    leftLayout->addWidget( searchBox );
    leftLayout->addStretch();
    leftImgBox->setLayout(leftLayout);

    // Right Layout for images
    rightImgBox = new QGroupBox(tr("Каталог"));
    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->addWidget(imgList);
    QHBoxLayout * imgBoxButtonsLay = new QHBoxLayout;
    imgBoxButtonsLay->addWidget( dirPathLabel );
    imgBoxButtonsLay->addStretch();
    imgBoxButtonsLay->addWidget( delImagesButton );
    imgBoxButtonsLay->addWidget( syncFolderButton );
    rightLayout->addLayout(imgBoxButtonsLay);
    rightImgBox->setLayout(rightLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(leftImgBox);
    mainLayout->addWidget(rightImgBox);

    centralWidget->setLayout(mainLayout);
}

void MainWindow::initConnections()
{
    // Connect Actions
    //
    connect( selectQueryImg, SIGNAL(triggered()), this, SLOT( openImage() ));
    connect( selectCatalog, SIGNAL(triggered()), this, SLOT( openCatalog() ));
    connect( quitAction, SIGNAL(triggered()), this, SLOT( close() ));

    // Edit
    connect( selectAllAction, SIGNAL(triggered()), this, SLOT( selectAllImages() ));
    connect( selectNoneAction, SIGNAL(triggered()), this, SLOT( selectNoneImages() ));
    connect( selectInvertAction, SIGNAL(triggered()), this, SLOT( selectInvertImages() ));
    //
    connect( copySelectedAction, SIGNAL(triggered()), this, SLOT( copySelectedImages() ));
    connect( moveSelectedAction, SIGNAL(triggered()), this, SLOT( moveSelectedImages() ));
    connect( delSelectedAction, SIGNAL(triggered()), this, SLOT( delSelectedImages() ));

    //
    connect( viewSettingsBlockAct, SIGNAL(triggered()), this, SLOT( winSettingsBlock() ));
    connect( viewOpenBlockAct, SIGNAL(triggered()), this, SLOT( winOpenBlock() ));
    connect( viewSearchBlockAct, SIGNAL(triggered()), this, SLOT( winSearchBlock() ));

    //
    connect( helpAction, SIGNAL(triggered()), this, SLOT( help() ));
    connect( aboutAction, SIGNAL(triggered()), this, SLOT( about() ));

    connect( syncFolderAction, SIGNAL(triggered()), this, SLOT( syncFolder() ));

    // Connection Buttons
    connect( openQueryImgButton, SIGNAL(clicked()), this, SLOT( openImage() ));
    connect( openCatalogButton, SIGNAL(clicked()), this, SLOT( openCatalog() ));

    connect( searchButton, SIGNAL(clicked()), this, SLOT( searchImages() ));

    connect( delImagesButton, SIGNAL(clicked()), this, SLOT( delSelectedImages() ));
    connect( syncFolderButton, SIGNAL(clicked()), this, SLOT( syncFolder() ));

    //
    connect( imgList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(showFullImg(QListWidgetItem *)) );
}

void MainWindow::showFullImg(QListWidgetItem *item)
{
    // system image viewer or my image viewer
    if (imgViewrProgAct->isChecked()) {
        imgViewer->setImagePath( folder_path + '\\' + item->text().split("\n")[0] );
        imgViewer->show();
    }
    else
        QDesktopServices::openUrl(QUrl::fromLocalFile( folder_path + '\\' + item->text().split("\n")[0] ));
}

// ---
void MainWindow::openImage()
{
    image_path = QFileDialog::getOpenFileName(this, tr("Відкриття зображення"), ".", "Images (*.jpg *.jpeg)");

    if (!image_path.isEmpty()) {
        QImage image(image_path);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Відкриття зображення"),
                                     tr("Не можу завантажити %1.").arg(image_path));
            return;
        }

        imgLabel->setPixmap(QPixmap::fromImage(image).scaled(imgLabel->size(), Qt::KeepAspectRatio));

        imgPathLabel->setText(tr("Зображення: %1").arg(image_path));
        imgPathLabel->setToolTip(image_path);
        imgLabel->setToolTip(image_path);
    }
}

void MainWindow::openCatalog()
{
    QString temp_path = folder_path;
    folder_path = QFileDialog::getExistingDirectory(
                this,
                tr("Відкриття каталогу..."),
                ".",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );

    qDebug() << folder_path;
    if (!folder_path.isEmpty()) {
        dirPathLabel->setText(tr("Каталог: %1").arg(folder_path));
        dirPathLabel->setToolTip(folder_path);

        updateImgList();
    }
    else
        folder_path = temp_path;
}

void MainWindow::updateImgList()
{
    QDir img_dir(folder_path);
    if (img_dir.exists()) {
        QFileInfoList dirContent = img_dir.entryInfoList(QStringList() << "*.jpg",
              QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

        if ( imgList->count() != 0 )
            imgList->clear();

        QProgressDialog progress(this);
        progress.setLabelText(tr("Відкриття..."));
        progress.setRange(0, dirContent.count());
        progress.setModal(true);

        for (int i = 0; i < dirContent.count(); ++i)
        {
            progress.setValue(i);
            QApplication::processEvents();

            QFileInfo image = dirContent.at(i);
            addImageToList( image.absoluteFilePath(), image.fileName() );
        }
    }
}

void MainWindow::addImageToList(QString img_path, QString img_name)
{
    int icoWidth     = 140;
    int icoHeight    = 140;
    QImageReader imageReader( img_path );

    QSize size;
    int image_width;
    int image_height;

    if (imageReader.supportsOption(QImageIOHandler::Size))
    {
        size = imageReader.size();
        image_width = size.width();
        image_height = size.height();
    }

    double ratio = (double)image_width / (double)image_height;
    if (ratio >= 1) {
        image_width = icoWidth;
        image_height = image_width / ratio;
    }
    else {
        image_height = icoHeight;
        image_width = image_height * ratio;
    }

    imageReader.setScaledSize(QSize(image_width, image_height));
    QImage image = imageReader.read();

    //QImage image(img_path);
    if (!image.isNull()) {
        QIcon icon(QPixmap::fromImage(image));
        QListWidgetItem *item = new QListWidgetItem(icon, img_name);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        imgList->addItem(item);
    }
}

void MainWindow::syncFolder()
{
    updateImgList();
}

void MainWindow::selectAllImages()
{
    for (int i = 0; i < imgList->count(); ++i)
    {
        QListWidgetItem * item = imgList->item(i);
        item->setCheckState(Qt::Checked);
    }
}

void MainWindow::selectNoneImages()
{
    for (int i = 0; i < imgList->count(); ++i)
    {
        QListWidgetItem * item = imgList->item(i);
        if (item->checkState())
            item->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::selectInvertImages()
{
    for (int i = 0; i < imgList->count(); ++i)
    {
        QListWidgetItem * item = imgList->item(i);
        if (item->checkState())
            item->setCheckState(Qt::Unchecked);
        else
            item->setCheckState(Qt::Checked);
    }
}

void MainWindow::delSelectedImages()
{
    int del_num = 0;
    int choice = QMessageBox::question(
                this,
                tr("Delete..."),
                tr("Delete selected files?"),
                QMessageBox::Yes, QMessageBox::No
                );

    if (choice == QMessageBox::Yes) {
        QList<QListWidgetItem *> items = imgList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);

        foreach(QListWidgetItem * item, items)
        {
            if (item->checkState()) {
                QString img_name = item->text().split("\n")[0];
                QFile::remove( folder_path + '\\' + img_name );
                delete item;
                del_num++;
            }
        }
        QMessageBox::information(this, "Removing is completed", "Removing " + QString::number(del_num) + " files completed.");
    }
}

void MainWindow::copySelectedImages()
{
    int copy_num = 0;
    QString copy_path = QFileDialog::getExistingDirectory(
                this,
                "Open Catalog...",
                ".",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );
    if (!copy_path.isEmpty()) {

        QList<QListWidgetItem *> items = imgList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);

        foreach(QListWidgetItem * item, items)
        {
            if (item->checkState()) {
                QString img_name = item->text().split("\n")[0];
                QFile::copy( folder_path + '\\' + img_name, copy_path + '\\' + img_name);
                copy_num++;
            }
        }

        QMessageBox::information(this, "Copying is completed", "Copying " + QString::number(copy_num) + " files completed.");
    }
}

void MainWindow::moveSelectedImages()
{
    int move_num = 0;
    QString move_path = QFileDialog::getExistingDirectory(
                this,
                "Open Catalog...",
                ".",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );
    if (!move_path.isEmpty()) {

        QList<QListWidgetItem *> items = imgList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);

        foreach(QListWidgetItem * item, items)
        {
            if (item->checkState()) {
                QString img_name = item->text().split("\n")[0];
                QFile::rename( folder_path + '\\' + img_name, move_path + '\\' + img_name );
                delete item;
                move_num++;
            }
        }

        QMessageBox::information(this, "Move completed", "Move " + QString::number(move_num) + " files completed.");
    }
}

void MainWindow::winSettingsBlock()
{
    if ( viewSettingsBlockAct->isChecked() )
        leftImgBox->show();
    else
        leftImgBox->hide();
}

void MainWindow::winOpenBlock()
{
    if ( viewOpenBlockAct->isChecked() )
        openBox->show();
    else
        openBox->hide();
}

void MainWindow::winSearchBlock()
{
    if ( viewSearchBlockAct->isChecked() )
        searchBox->show();
    else
        searchBox->hide();
}

// ---

void MainWindow::searchImages()
{
    // check path to image and folder...
    if (image_path.isNull() || folder_path.isNull()) {
        QMessageBox::critical(this, "Помилка", "Спочатку оберіть зображення та каталог.");
        return;
    }

    // передаем путь к изобрж. запросу и изобрж. для сравнения
    // получаем оценку сравнения

    QProgressDialog progress(this);
    progress.setLabelText(tr("Здійснюється пошук..."));
    progress.setRange(0, imgList->count());
    progress.setModal(true);

    //
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    //

    QString img_name;

    //
    ImgHistogram hist2;
    COLOR_SPACE cspace;

    double (ImgHistogram::*s_method)(ImgHistogram &, COLOR_SPACE);

    switch( searchMethodCBox->currentIndex() ) {
        case 0:
            cspace = RGB;
            hist2.initQHistRGB(8, 8, 8);
            query_hist->initQHistRGB(8, 8, 8);
            s_method = &ImgHistogram::histIntersectionDist;
            break;
        case 1:
            cspace = RGB;
            hist2.initQHistRGB(8, 8, 8);
            query_hist->initQHistRGB(8, 8, 8);
            s_method = &ImgHistogram::histEuclideanDist;
            break;

        case 2:
            cspace = HSV;
            hist2.initQHistHSV(18,3,3);
            query_hist->initQHistHSV(18, 3, 3);
            s_method = &ImgHistogram::histIntersectionDist;
            break;
        case 3:
            cspace = HSV;
            hist2.initQHistHSV(18,3,3);
            query_hist->initQHistHSV(18, 3, 3);
            s_method = &ImgHistogram::histEuclideanDist;
            break;
        case 4:
            searchCorrSpectrum();
            return;
        break;
    }

    // query image

    query_hist->getImageHist(image_path.toLocal8Bit().data(), cspace);

    for (int i = 0; i < imgList->count(); ++i)
    {
        progress.setValue(i);
        QApplication::processEvents();

        QListWidgetItem * item = imgList->item(i);
        img_name = item->text().split("\n")[0];

        //

        QString pathToImg = folder_path + '\\' + img_name;
        qDebug() << pathToImg;
        hist2.getImageHist( pathToImg.toLocal8Bit().data(), cspace );

        //

        //double result = query_hist->histIntersectionDist(hist2, cspace);
        double result = (query_hist->*s_method)(hist2, cspace);
        //
        qDebug() << result;
        item->setText(img_name+"\n"+QString::number(result));
        item->setToolTip(QString::number(result));
    }

    // сортируем полученные результаты
    // и выводим уже отсортированные
    sortImgList();

    //
    qDebug() << QDateTime::currentMSecsSinceEpoch() - time;
    //
}

void MainWindow::searchCorrSpectrum()
{
    QString img_name;
    CorrSpectrum hist2;
    squery_hist->getImageHist( image_path.toLocal8Bit().data() );

    for (int i = 0; i < imgList->count(); ++i)
    {
        //progress.setValue(i);
        QApplication::processEvents();

        QListWidgetItem * item = imgList->item(i);
        img_name = item->text().split("\n")[0];

        //

        QString pathToImg = folder_path + '\\' + img_name;
        qDebug() << pathToImg;
        hist2.getImageHist( pathToImg.toLocal8Bit().data() );

        //

        double result = squery_hist->calcCorrelation(hist2);
        //
        qDebug() << result;
        item->setText(img_name+"\n"+QString::number(result));
        item->setToolTip(QString::number(result));
    }

    // сортируем полученные результаты
    // и выводим уже отсортированные
    sortImgList();
}

void MainWindow::sortImgList()
{
    int size = imgList->count();
    QListWidgetItem * item_a;
    QListWidgetItem * item_b;
    QListWidgetItem * temp;
    float num_a = 0;
    float num_b = 0;

    int currIndex = searchMethodCBox->currentIndex();

    if ( currIndex == 0 || currIndex == 2 || currIndex == 4 ) {
        for (int a = 1; a < size; ++a) {
            for (int b = size-1; b >= a; --b) {
                item_a = imgList->item(b-1);
                item_b = imgList->item(b);
                num_a = item_a->text().split("\n")[1].toFloat();
                num_b = item_b->text().split("\n")[1].toFloat();
    
                if (num_a < num_b) {
                    temp = imgList->item(b-1);
                    imgList->insertItem(b-1, imgList->takeItem(b) );
                    imgList->insertItem(b, temp);
                }
            }
        }
    }
    else {
        for (int a = 1; a < size; ++a) {
            for (int b = size-1; b >= a; --b) {
                item_a = imgList->item(b-1);
                item_b = imgList->item(b);
                num_a = item_a->text().split("\n")[1].toFloat();
                num_b = item_b->text().split("\n")[1].toFloat();

                if (num_a > num_b) {
                    temp = imgList->item(b-1);
                    imgList->insertItem(b-1, imgList->takeItem(b) );
                    imgList->insertItem(b, temp);
                }
            }
        }
    }
}

// ---

void MainWindow::help()
{
    QMessageBox::information(this, tr("Допомога"),
                                                    tr(
    "<h1>Допомога</h1>"
    "<p align='justify'>щось таке...</p>"
                                                         )
                                                    );
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("Про програму Image Retrieval ver. 0.5.13.12q"),
        tr("<b>Image Retrieval — ver. 0.5.13.12 Qt version</b>"
            "<br><br>Розробив як дипломний проект (c) 2012. Dmytro Korynetskyi (424-pr)."
            ));
}

