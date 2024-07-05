#pragma once

#include <QMainWindow>

class QLabel;
class QProgressDialog;
class ImageLoader;
class ImageSet;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

signals:
    void loadingFinished();

private slots:
    void onOpenNewFolder();
    void onOpenGlWidgetUpdate(const QString& str) const;
    void updateProgress();
    void onLoadingFinished();

private:
    void openFolder(const QString& selectedFolder);

    QLabel* m_InfoLabel{ nullptr };
    QProgressDialog* m_ProgressDialog{ nullptr };
    ImageLoader* m_ImageLoader{ nullptr };
    std::unique_ptr<ImageSet> m_ImageSet;

    constexpr static uint8_t m_MaxRecentFolders = 10;
};
