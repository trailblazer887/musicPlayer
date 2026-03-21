#include "mainwindow.h"
#include "ui_mainwindow.h"       // 自动生成的UI头文件
#include "songlibrarywindow.h"   // 歌库窗口头文件
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    songLibWindow(nullptr),
    currentSongIndex(-1),
    progressTimer(new QTimer(this))
{
    ui->setupUi(this); // 加载主窗口UI

    qApp->installEventFilter(this); // 安装事件过滤器

    // 初始化播放器
    player = new QMediaPlayer(this);

    audioOutput = new QAudioOutput(this); // 新增
    player->setAudioOutput(audioOutput);  // 把播放器绑定到音频输出
    audioOutput->setVolume(1.0);

    // 初始化控件
    ui->playPauseBtn->setText("播放"); // 按钮文字为"播放"
    ui->progressSlider->setRange(0, 1000); // 初始化进度条
    ui->progressSlider->setValue(0);
    ui->prevSongBtn->setEnabled(false); // 上/下一曲按钮关闭
    ui->nextSongBtn->setEnabled(false);
    ui->progressSlider->setSingleStep(40); // 键盘改变进度条的幅度

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
        qint64 newPos = (player->duration() * position) / 1000;
        player->setPosition(newPos);
    }
}

// 实时更新进度条(由"定时器"驱动)
void MainWindow::updateProgressBar()
{
    if (player->duration() > 0) {
        int progress = (player->position() * 1000) / player->duration();
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

// 处理歌库选中的歌曲(当收到songSelected信号时)
void MainWindow::handleSongSelected(const QString &songPath)
{
    songList.clear(); // 清理songList(这里的songList是歌曲路径列表 QList<QUrl>)
    songList.append(QUrl::fromLocalFile(songPath)); // 将歌曲添加入songList
    currentSongIndex = 0; // 将歌曲索引设置为0
    player->setSource(songList[currentSongIndex]); // 加载索引为0处的资源
    player->play(); // 播放
    // 开放上/下曲开关
    ui->prevSongBtn->setEnabled(true);
    ui->nextSongBtn->setEnabled(true);
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


// 事件处理 Filter
bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::KeyPress){ // 识别事件是否是键盘按下
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Right){ // 如果是右方向键
            if (player->playbackState() == QMediaPlayer::StoppedState){ // 如果是没有歌曲正在播放或暂停(没有选择歌曲)
                return true;
            }
            qint64 currentPos = player->position();
            qint64 newPos = currentPos + 6000; // 若右方向键合法，歌曲加6000毫秒(6秒)
            if (newPos > player->duration()){ // 防止加后溢出
                newPos = player->duration();
            }
            player->setPosition(newPos);
            int sliderValue = static_cast<int>(newPos * 1000 / player->duration());
            ui->progressSlider->setValue(sliderValue);
            return true;
        }else if (keyEvent->key() == Qt::Key_Left){ // 如果是左方向键
            if (player->playbackState() == QMediaPlayer::StoppedState){ // 如果是没有歌曲正在播放或暂停(没有选择歌曲)
                return true;
            }
            qint64 currentPos = player->position();
            qint64 newPos = currentPos - 6000; // 若左方向键合法，歌曲减6000毫秒(6秒)
            if (newPos < 0){
                newPos = 0;
            }
            player->setPosition(newPos);
            int sliderValue = static_cast<int>(newPos * 1000 / player->duration());
            ui->progressSlider->setValue(sliderValue);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event); // 其他事件默认处理;
}



