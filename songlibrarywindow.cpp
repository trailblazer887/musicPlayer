#include "songlibrarywindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

SongLibraryWindow::SongLibraryWindow(QWidget *parent) :
    QDialog(parent),
    songList(new QListWidget(this))
{
    this->setWindowTitle("歌曲库");
    this->resize(400, 300);

    // 创建按钮
    QPushButton *importBtn = new QPushButton("导入歌曲", this);
    QPushButton *confirmBtn = new QPushButton("确定选择", this);
    QPushButton *cancelBtn = new QPushButton("取消", this);

    // 布局（纯代码）
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    mainLayout->addWidget(songList);
    btnLayout->addWidget(importBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(confirmBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);
    this->setLayout(mainLayout);

    // 信号槽
    connect(importBtn, &QPushButton::clicked, this, &SongLibraryWindow::on_importBtn_clicked);
    connect(confirmBtn, &QPushButton::clicked, this, &SongLibraryWindow::on_confirmBtn_clicked);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);

    // 初始禁用确定按钮
    confirmBtn->setEnabled(false);
    connect(songList, &QListWidget::itemSelectionChanged, this, [=](){
        confirmBtn->setEnabled(songList->currentItem() != nullptr);
    });
}

SongLibraryWindow::~SongLibraryWindow()
{
    // 无需delete ui（纯代码创建UI）
}

// 导入歌曲（支持AAC格式）
void SongLibraryWindow::on_importBtn_clicked()
{
    QStringList songPaths = QFileDialog::getOpenFileNames(
        this,
        "选择音乐文件",
        QDir::homePath(),
        "音频文件 (*.mp3 *.wav *.flac *.ogg *.m4a *.aac)" // 包含AAC
    );

    if (!songPaths.isEmpty()) {
        for (const QString &path : songPaths) {
            QString songName = QFileInfo(path).fileName();
            QListWidgetItem *item = new QListWidgetItem(songName);
            item->setData(Qt::UserRole, path); // 存储完整路径
            songList->addItem(item);
        }
        QMessageBox::information(this, "成功", "导入了 " + QString::number(songPaths.size()) + " 首歌曲！");
    }
}

// 确定选择歌曲
void SongLibraryWindow::on_confirmBtn_clicked()
{
    QListWidgetItem *selected = songList->currentItem();
    if (selected) {
        QString songPath = selected->data(Qt::UserRole).toString();
        emit songSelected(songPath);
        this->close();
    }
}
