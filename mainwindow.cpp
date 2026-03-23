#include "mainwindow.h"
#include "ui_mainwindow.h"       // 自动生成的UI头文件
#include "songlibrarywindow.h"   // 歌库窗口头文件
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    songLibWindow(nullptr),
    currentSongIndex(-1),
    progressTimer(new QTimer(this)),
    autoNextEnabled(true)
{
    ui->setupUi(this); // 加载主窗口UI
    ui->autoNextBtn->setText("关闭自动切换"); // 自动切换按钮初始化

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

    // 歌曲播放完后自动切换下一曲
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia && !songList.isEmpty() && autoNextEnabled){
            on_nextSongBtn_clicked();
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
    if (!songLibWindow) { // 如果曾经没有打开歌库
        songLibWindow = new SongLibraryWindow(this); // 新建歌库窗口
        connect(songLibWindow, &SongLibraryWindow::songSelected, this, &MainWindow::handleSongSelected); // 接收到songSelected信号后调用函数处理
    }
    songLibWindow->show(); // 展示窗口
}

// 处理歌库选中的歌曲(当收到songSelected信号时)
void MainWindow::handleSongSelected(const QList<QUrl> &newSongList, int selectedIndex)
{
    for (const QUrl &url : newSongList) { // 合并新播放列表(去重)
        if (!songList.contains(url)){
            songList.append(url);
        }
    }

    if (selectedIndex >= 0 && selectedIndex < newSongList.size()){ // 将newSongList中的索引转换为songList中的索引
        QUrl selectedUrl = newSongList.at(selectedIndex);
        currentSongIndex = songList.indexOf(selectedUrl);
    }else {
        currentSongIndex = 0;
    }

    if (currentSongIndex >= 0 && currentSongIndex < songList.size()){ // 加载并播放音乐
        player->setSource(songList[currentSongIndex]);
        player->play();
    }

    ui->prevSongBtn->setEnabled(!songList.isEmpty()); // 上/下一曲按钮激活
    ui->nextSongBtn->setEnabled(!songList.isEmpty());
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

    currentSongIndex = (currentSongIndex + 1 + songList.size()) % songList.size();
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

// 切换播放状态
void MainWindow::on_autoNextBtn_clicked(){
    autoNextEnabled = !autoNextEnabled;
    if (autoNextEnabled){
        ui->autoNextBtn->setText("关闭自动切换");
    }else{
        ui->autoNextBtn->setText("开启自动切换");
    }
}


