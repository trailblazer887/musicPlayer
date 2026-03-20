#include "songlibrarywindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

SongLibraryWindow::SongLibraryWindow(QWidget *parent) :
    QDialog(parent),
    songList(new QListWidget(this)) // 这里的songList是窗口控件
{
    // 窗口标题，尺寸
    this->setWindowTitle("歌曲库");
    this->resize(400, 300);

    // 创建按钮
    QPushButton *importBtn = new QPushButton("导入歌曲", this);
    QPushButton *confirmBtn = new QPushButton("确定", this);
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

    // 当选择歌曲时,确定按钮变为可用状态
    connect(songList, &QListWidget::itemSelectionChanged, this, [=](){
        confirmBtn->setEnabled(songList->currentItem() != nullptr);
    });
}

SongLibraryWindow::~SongLibraryWindow()
{
    // 无需delete ui(纯代码创建UI)
}

// 导入歌曲(支持AAC格式)
void SongLibraryWindow::on_importBtn_clicked()
{
    QStringList songPaths = QFileDialog::getOpenFileNames( // 选择文件窗口
        this,
        "选择音乐文件",
        QDir::homePath(),
        "音频文件 (*.mp3 *.wav *.flac *.ogg *.m4a *.aac)"
    );

    if (!songPaths.isEmpty()) { // 判断是否选择文件
        for (const QString &path : songPaths) { // 遍历选择的文件
            QString songName = QFileInfo(path).baseName(); // 提取歌曲名
            QListWidgetItem *item = new QListWidgetItem(songName); // 创造单个歌曲的列表项目(包含歌曲名称)
            item->setData(Qt::UserRole, path); // 存储歌曲完整路径
            songList->addItem(item); // 将列表项添加至总歌曲列表
        }
        QMessageBox::information(this, "成功", "导入了 " + QString::number(songPaths.size()) + " 首歌曲！"); // 窗口提示
    }
}

// 确定选择歌曲
void SongLibraryWindow::on_confirmBtn_clicked()
{
    QListWidgetItem *selected = songList->currentItem(); // 指向选中列表项
    if (selected) { // 如果选择了一个列表项(指针不是NULL)
        QString songPath = selected->data(Qt::UserRole).toString(); // 获取歌曲路径
        emit songSelected(songPath); // 发送"歌曲已选择"信号
        this->close(); // 关闭窗口
    }
}
