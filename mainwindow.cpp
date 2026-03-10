#include "mainwindow.h"
#include "ui_mainwindow.h"       // 自动生成的UI头文件
#include "songlibrarywindow.h"   // 歌库窗口头文件

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    songLibWindow(nullptr),
    currentSongIndex(-1),
    progressTimer(new QTimer(this))
{
    ui->setupUi(this); // 加载主窗口UI

    // 初始化播放器
    player = new QMediaPlayer(this);

    audioOutput = new QAudioOutput(this); // 新增
    player->setAudioOutput(audioOutput);  // 把播放器绑定到音频输出
    audioOutput->setVolume(1.0);

    // 初始化控件
    ui->playPauseBtn->setText("播放");
    ui->progressSlider->setRange(0, 100);
    ui->progressSlider->setValue(0);
    ui->prevSongBtn->setEnabled(false);
    ui->nextSongBtn->setEnabled(false);

    // 定时器：每500ms更新进度条
    progressTimer->setInterval(500);
    connect(progressTimer, &QTimer::timeout, this, &MainWindow::updateProgressBar);

    // 播放状态改变时更新按钮文本
    connect(player, &QMediaPlayer::playbackStateChanged, this, [=](QMediaPlayer::PlaybackState state){
        if (state == QMediaPlayer::PlayingState) {
            ui->playPauseBtn->setText("暂停");
            progressTimer->start();
        } else {
            ui->playPauseBtn->setText("播放");
            if (state == QMediaPlayer::StoppedState) {
                progressTimer->stop();
                ui->progressSlider->setValue(0);
            }
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 播放/暂停
void MainWindow::on_playPauseBtn_clicked()
{
    if (songList.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先从歌库选择歌曲！");
        return;
    }
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
    } else {
        player->play();
    }
}

// 进度条拖动
void MainWindow::on_progressSlider_sliderMoved(int position)
{
    if (player->duration() > 0) {
        qint64 newPos = (player->duration() * position) / 100;
        player->setPosition(newPos);
    }
}

// 实时更新进度条
void MainWindow::updateProgressBar()
{
    if (player->duration() > 0) {
        int progress = (player->position() * 100) / player->duration();
        ui->progressSlider->setValue(progress);
    }
}

// 打开歌库
void MainWindow::on_openLibraryBtn_clicked()
{
    if (!songLibWindow) {
        songLibWindow = new SongLibraryWindow(this);
        connect(songLibWindow, &SongLibraryWindow::songSelected, this, &MainWindow::handleSongSelected);
    }
    songLibWindow->show();
}

// 手动上一曲
void MainWindow::on_prevSongBtn_clicked()
{
    if (songList.isEmpty()) return;
    currentSongIndex = (currentSongIndex - 1 + songList.size()) % songList.size();
    player->setSource(songList[currentSongIndex]);
    player->play();
}

// 手动下一曲
void MainWindow::on_nextSongBtn_clicked()
{
    if (songList.isEmpty()) return;
    currentSongIndex = (currentSongIndex + 1) % songList.size();
    player->setSource(songList[currentSongIndex]);
    player->play();
}

// 处理歌库选中的歌曲
void MainWindow::handleSongSelected(const QString &songPath)
{
    songList.clear();
    songList.append(QUrl::fromLocalFile(songPath));
    currentSongIndex = 0;
    player->setSource(songList[currentSongIndex]);
    player->play();
    ui->prevSongBtn->setEnabled(true);
    ui->nextSongBtn->setEnabled(true);
}
