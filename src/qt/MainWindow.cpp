#include "MainWindow.h"
#include "OpenGLWidget.h"
#include "Renderer.h"
#include "OpenGL.h"
#include "ImageLoader.h"
#include "ImageSet.h"

#include <QMenuBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QProgressDialog>
#include <QApplication>
#include <QThread>

MainWindow::MainWindow()
    : QMainWindow(nullptr)
{
    move(100, 100);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(centralWidget);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(OpenGlInfo::m_MajorVersion, OpenGlInfo::m_MinorVersion);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    OpenGLWidget* glWidget = new OpenGLWidget(this);
    glWidget->setFixedSize(Renderer::RENDER_WIDTH, Renderer::RENDER_HEIGHT);
    glWidget->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(glWidget);
    connect(glWidget, &OpenGLWidget::mouseMove, this, &MainWindow::onOpenGlWidgetUpdate);

    m_InfoLabel = new QLabel("This is a QLabel", this);
    layout->addWidget(m_InfoLabel);

    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu* fileMenu = menuBar->addMenu("File");

    QAction* openFolderAction = fileMenu->addAction("Open DICOM Folder");
    connect(openFolderAction, &QAction::triggered, this, &MainWindow::onOpenNewFolder);

    const auto recentMenu = fileMenu->addMenu("Open Recent...");
    QSettings settings("HeartToHeart", "CtVis");
    auto recentList = settings.value("recentFolders").toStringList();
    for (const auto& dir : recentList)
    {
        QAction* action = new QAction(dir, this);
        recentMenu->addAction(action);
        connect(action, &QAction::triggered, [this, dir]() { openFolder(dir); });
    }

    QAction* exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    connect(this, &MainWindow::loadingFinished, this, &MainWindow::onLoadingFinished);
}

MainWindow::~MainWindow() = default;

void MainWindow::onOpenNewFolder()
{
    QSettings settings("HeartToHeart", "CtVis");
    const auto recentList = settings.value("recentFolders").toStringList();

    const auto mostRecent = recentList.count() > 0 ? recentList.last() : "";
    const auto selectedFolder = QFileDialog::getExistingDirectory(nullptr, "Open a folder containing DICOM files", mostRecent, QFileDialog::DontResolveSymlinks);

    if (selectedFolder.isEmpty())
    {
        return;
    }

    openFolder(selectedFolder);
}

void MainWindow::openFolder(const QString& selectedFolder)
{
    const QDir dir(selectedFolder);
    if (dir.entryInfoList({ "*.dcm" }, QDir::Files).empty())
    {
        QMessageBox::warning(nullptr, "No DICOM files", "The folder you selected contains no DICOM files.", QMessageBox::Ok);
        return;
    }

    QSettings settings("HeartToHeart", "CtVis");
    auto recentList = settings.value("recentFolders").toStringList();
    if (!recentList.contains(selectedFolder))
    {
        recentList.push_front(selectedFolder);
        while (recentList.size() > m_MaxRecentFolders) {
            recentList.removeLast();
        }
        settings.setValue("recentFolders", recentList);
    }

    m_ImageLoader = new ImageLoader(selectedFolder.toStdString());

    // why is the -1 needed????
    m_ProgressDialog = new QProgressDialog("Loading images", "Cancel", 0, int(m_ImageLoader->entries() - 1), this);
    m_ProgressDialog->setWindowModality(Qt::WindowModal);
    m_ProgressDialog->setAutoClose(true);
    m_ProgressDialog->show();

    std::thread thread([&]() {
        m_ImageSet = std::move(m_ImageLoader->load([&]() {
            QMetaObject::invokeMethod(this, "updateProgress",
                Qt::QueuedConnection);
            }));

        delete m_ImageLoader;
        m_ImageLoader = nullptr;

        emit loadingFinished();
    });

    thread.detach();
}

void MainWindow::onOpenGlWidgetUpdate(const QString& str) const
{
    m_InfoLabel->setText(str);
}

void MainWindow::updateProgress()
{
    m_ProgressDialog->setValue(m_ProgressDialog->value() + 1);
}

void MainWindow::onLoadingFinished()
{
    Renderer::instance().setImageSet(std::move(m_ImageSet));

    //m_ProgressDialog = new QProgressDialog("Uploading textures to GPU", "Cancel", 0, 1, this);
    //m_ProgressDialog->setWindowModality(Qt::WindowModal);
    //m_ProgressDialog->setAutoClose(true);
    //m_ProgressDialog->show();
    //
    //std::thread thread([&]() {
    //    Renderer::instance().setImageSet(std::move(m_ImageSet));
    //    m_ImageSet = nullptr;
    //});


    //m_ProgressDialog->setValue(1);
}
