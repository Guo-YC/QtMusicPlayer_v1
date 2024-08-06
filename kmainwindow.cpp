#include "kmainwindow.h"
#include "klyricsmanager.h"
#include "ui_kmainwindow.h"
#include "ksongtablemodel.h"
#include "kmediaplayermanager.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPainter>
#include <QTimer>
#include <QBitmap>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QMessageBox>
KMainWindow::KMainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::KMainWindow)
{

	ui->setupUi(this);

	// 创建托盘图标
	trayIcon = new QSystemTrayIcon(QIcon(":/image/musicLogo.png"), this);
	QMenu* trayIconMenu = new QMenu(this);
	QAction* quitAction = trayIconMenu->addAction(QString::fromLocal8Bit("退出"));
	QAction* restoreAction = trayIconMenu->addAction(QString::fromLocal8Bit("打开界面"));

	(void)connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
	(void)connect(restoreAction, &QAction::triggered, this, &KMainWindow::showNormal);

	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setToolTip(QString::fromLocal8Bit("QtMusicPlay"));
	trayIcon->show();

	QIcon iconLeft(":/image/originalLeftButton.png");
	QIcon iconRight(":/image/originalRightButton.png");
	QIcon iconStoped(":/image/originalPlayButton.png");
	QIcon iconVolume(":/image/soundIcon.png");		
	ui->m_lastItemPushButton->setIcon(iconLeft);
	ui->m_nextItemPushButton->setIcon(iconRight);
	ui->m_startPushButton->setIcon(iconStoped);
	ui->m_voiceControlPushButton->setIcon(iconVolume);
	ui->m_logolabel->setPixmap(QPixmap(":/image/musicLogo.png"));

	ui->m_playingPercentHorizontalSlider->setToolTip(QString::fromLocal8Bit("拖动滑块调节进度"));
	ui->m_voiceVerticalSlider->setToolTip(QString::fromLocal8Bit("拖动滑块调节音量"));


	initLyricsManager();
	(void)connect(ui->m_onlineTableView, &QTableView::doubleClicked, this, &KMainWindow::onRowDoubleClicked);
	(void)connect(ui->m_startPushButton, &QPushButton::clicked, this, &KMainWindow::switchState);
	(void)connect(ui->m_nextItemPushButton, &QPushButton::clicked, this, &KMainWindow::next);
	(void)connect(ui->m_lastItemPushButton, &QPushButton::clicked, this, &KMainWindow::previous);
	(void)connect(this, &KMainWindow::songChanged, this, &KMainWindow::showCurrentSongName);
	(void)connect(m_mediaPlayerManager, &KMediaPlayerManager::progressChanged, this, &KMainWindow::onProgressChanged);
	initTableView();
	initMediaPlayManager();
	(void)connect(m_mediaPlayerManager, &KMediaPlayerManager::progressChanged, this, &KMainWindow::onProgressChanged);
	(void)connect(m_songOnlineModel, &KSongTableModel::updatetPixmap, this, &KMainWindow::updatePixmapHandle);
	(void)connect(ui->m_voiceVerticalSlider, &QSlider::sliderMoved, this, &KMainWindow::voiceChangedHandle);
	(void)connect(ui->m_playingPercentHorizontalSlider, &QSlider::sliderMoved, this, &KMainWindow::positionChangedHandle);
	(void)connect(ui->m_voiceControlPushButton, &QPushButton::clicked, this, &KMainWindow::voiceSwitchHandle);
	(void)connect(ui->m_playModePushButton, &QPushButton::clicked, this, &KMainWindow::playModeSwitchHandle);
	(void)connect(m_songOnlineModel, &KSongTableModel::updatePlayListSignal, this, &KMainWindow::updatePlayListHandle);
	(void)connect(m_songOnlineModel, &KSongTableModel::downLoadSucSignal, this, [=](int index, QString localPath)
		{
			ui->m_onlineTableView->selectionModel()->clearSelection();
			ui->m_onlineTableView->selectionModel()->select(ui->m_onlineTableView->model()->index(index, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
			qDebug() << QString::fromLocal8Bit("已下载，本地播放：") << localPath;
			m_mediaPlayerManager->resume(index, localPath);
			QString lyricPath = m_songOnlineModel->getLocalLyricPathByIndex(index);
			m_lyricsManager->loadLyrics(lyricPath);
		});
	connect(trayIcon, &QSystemTrayIcon::activated, this, &KMainWindow::trayIconActivated);
}



KMainWindow::~KMainWindow()
{
	m_mediaPlayerManager->stop();

	delete trayIcon;
	delete ui;
	delete m_lyricsManager;
	delete m_songOnlineModel;
}


void KMainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason) 
{
	if (reason == QSystemTrayIcon::DoubleClick) 
	{
		showNormal();  // 或者使用 show(), showMaximized() 等，取决于你想如何显示窗口
	}
}

void KMainWindow::closeEvent(QCloseEvent* event)
{
	QMessageBox msgBox;
	msgBox.setWindowTitle("QtMusicPlay");
	msgBox.setText(tr(u8"您想要关闭还是最小化到系统托盘？"));
	msgBox.addButton(tr(u8"关闭"), QMessageBox::AcceptRole);
	msgBox.addButton(tr(u8"最小化"), QMessageBox::RejectRole);
	msgBox.setDefaultButton(QMessageBox::NoButton);

	int ret = msgBox.exec();

	if (ret == QMessageBox::AcceptRole) 
	{
		QApplication::quit();  // 用户选择关闭程序
	}
	else 
	{
		hide();                // 用户选择最小化到托盘
		event->ignore();       // 忽略关闭事件，不关闭程序
	}
}


void KMainWindow::initLyricsManager() 
{
	m_lyricsManager = new KLyricsManager(this);
}

void KMainWindow::initTableView()
{
	m_songOnlineModel = new KSongTableModel(this);
	m_songOnlineModel->initDataSource();

	ui->m_onlineTableView->setModel(m_songOnlineModel);
	ui->m_onlineTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->m_onlineTableView->horizontalHeader()->resizeSection(0, 150);
	ui->m_onlineTableView->horizontalHeader()->resizeSection(1, 60);
	ui->m_onlineTableView->horizontalHeader()->resizeSection(2, 50);
	ui->m_onlineTableView->horizontalHeader()->resizeSection(3, 70);
	ui->m_onlineTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->m_onlineTableView->verticalHeader()->hide();

	//设置单击选中行
	ui->m_onlineTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

	//关闭滚动条
	ui->m_onlineTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


}
void KMainWindow::initMediaPlayManager()
{
	m_mediaPlayerManager = new KMediaPlayerManager(this);
	m_mediaPlayerManager->setPlayMode("sortPlay");
	QIcon icon(":/image/sortPlay.png");
	ui->m_playModePushButton->setIcon(icon);
	ui->m_playModLabel->setText(QString::fromLocal8Bit("顺序播放"));

	ui->m_voiceVerticalSlider->setValue(20);
}

void KMainWindow::updatePlayListHandle(QVector<QString> playList)
{
	m_mediaPlayerManager->updatePlayList(playList);
}

void KMainWindow::downloadSong(int index, bool isPlay = true)
{
	m_mediaPlayerManager->setCurrentIndex(index);
	ui->m_songWordsLabel->setText(QString::fromLocal8Bit("歌曲下载中..."));
	switchPushButton("playing");
	m_songOnlineModel->getMp3ByIndex(m_mediaPlayerManager->getCurrentIndex());
}

void KMainWindow::onRowDoubleClicked(const QModelIndex& index)
{

	//ui->m_songWordsLabel
	qDebug() << "双击了第" << index.row() << "行";

	int int_index = index.row();
	m_mediaPlayerManager->setCurrentIndex(int_index);
	//	m_mediaPlayerManager->resume();
	m_songOnlineModel->getPicByIndex(int_index);
	showCurrentSongName(int_index);

	ui->m_onlineTableView->selectionModel()->clearSelection();
	ui->m_onlineTableView->selectionModel()->select(ui->m_onlineTableView->model()->index(int_index, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);

	QString path = m_songOnlineModel->getLocalMp3PathByIndex(int_index);
	if (path.length() == 0)// 无缓存
		downloadSong(int_index, true); //下载
	else // 有缓存
		playLocalMp3ByIndex(int_index);// 缓存	
}




void KMainWindow::switchState()
{

	if (m_mediaPlayerManager->isStoppedState())
	{
		int int_index = m_mediaPlayerManager->getCurrentIndex();
		if (int_index == -1) int_index = 0;
		m_mediaPlayerManager->setCurrentIndex(int_index);
		m_songOnlineModel->getPicByIndex(int_index);
		showCurrentSongName(int_index);
		ui->m_onlineTableView->selectionModel()->clearSelection();
		ui->m_onlineTableView->selectionModel()->select(ui->m_onlineTableView->model()->index(int_index, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);

		QString path = m_songOnlineModel->getLocalMp3PathByIndex(int_index);
		if (path.length() == 0)// 无缓存
			downloadSong(int_index, true); //下载
		else // 有缓存
			playLocalMp3ByIndex(int_index);// 缓存

	}
	else
	{
		if (m_mediaPlayerManager->isPlayingState())
		{
			switchPushButton("stoped");
			m_timer.stop();
		}
		else
		{
			switchPushButton("playing");
			m_timer.start(100);

		}

		m_mediaPlayerManager->check();
	}
}

void KMainWindow::next()
{
	m_mediaPlayerManager->next();
	m_songOnlineModel->getPicByIndex(m_mediaPlayerManager->getCurrentIndex());
	showCurrentSongName(m_mediaPlayerManager->getCurrentIndex());

	int int_index = m_mediaPlayerManager->getCurrentIndex();
	m_mediaPlayerManager->setCurrentIndex(int_index);
	m_songOnlineModel->getPicByIndex(int_index);
	showCurrentSongName(int_index);

	ui->m_onlineTableView->selectionModel()->clearSelection();
	ui->m_onlineTableView->selectionModel()->select(ui->m_onlineTableView->model()->index(int_index, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
	QString path = m_songOnlineModel->getLocalMp3PathByIndex(int_index);
	if (path.length() == 0)// 无缓存
		downloadSong(int_index, true); //下载
	else // 有缓存
		playLocalMp3ByIndex(int_index);// 缓存	
}

void KMainWindow::previous()
{
	m_mediaPlayerManager->previous();
	m_songOnlineModel->getPicByIndex(m_mediaPlayerManager->getCurrentIndex());
	showCurrentSongName(m_mediaPlayerManager->getCurrentIndex());
	int int_index = m_mediaPlayerManager->getCurrentIndex();
	m_mediaPlayerManager->setCurrentIndex(int_index);
	m_songOnlineModel->getPicByIndex(int_index);
	showCurrentSongName(int_index);

	ui->m_onlineTableView->selectionModel()->clearSelection();
	ui->m_onlineTableView->selectionModel()->select(ui->m_onlineTableView->model()->index(int_index, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
	QString path = m_songOnlineModel->getLocalMp3PathByIndex(int_index);
	if (path.length() == 0)// 无缓存
		downloadSong(int_index, true); //下载
	else // 有缓存
		playLocalMp3ByIndex(int_index);// 缓存	
}

void KMainWindow::showCurrentSongName(int index)
{
	ui->m_picLabel->clear();
	QString songName = m_songOnlineModel->getMusicNameByIndex(index);
	ui->m_currSongLabel->setText(songName);
	qDebug() << index << songName;
	ui->m_playingPercentHorizontalSlider->setValue(0);
}

QString formatDuration(qint64 milliseconds) 
{
	int minutes = milliseconds / 60;
	int seconds = (milliseconds) % 60;

	return QString("%1:%2").arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));

}

void KMainWindow::updateLyrics(qint64 position) 
{
	QString lyrics = m_lyricsManager->getLyrics(position);
	ui->m_songWordsLabel->setText(lyrics);
}

void KMainWindow::onProgressChanged(qint64 position, qint64 duration)
{
	duration = m_songOnlineModel->getDurationByIndex(m_mediaPlayerManager->getCurrentIndex());
	//position = position / 1000;
	int progress = static_cast<int>((static_cast<double>(position) / (duration * 1000)) * 100);
	ui->m_playingPercentHorizontalSlider->setValue(progress);


	QString formattedPosition = formatDuration(position / 1000);
	QString formattedDuration = formatDuration(duration);

	ui->m_currDurationLabel->setText(formattedPosition);
	ui->m_totalDurationLabel->setText(formattedDuration);
	if (position >= 500)
		updateLyrics(position);
	//m_rotation += 15;

	rotate(((position) / 1000) % 360);

}

void KMainWindow::updatePixmapHandle(QPixmap& Pixmap)
{
	m_rotation = 0;
	QPixmap pixMap = Pixmap.scaled(370, 370, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	int radius = 370 / 2;
	QSize size(370, 370);
	QBitmap mask(size);
	QPainter painter(&mask);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawRoundedRect(0, 0, size.width(), size.height(), radius, radius);
	QPixmap image = pixMap.scaled(size);
	image.setMask(mask);

	m_rotation = 0;
	ui->m_picLabel->clear();
	m_pixmap = image;

	ui->m_picLabel->setPixmap(m_pixmap);
}


void KMainWindow::rotateNoRotation()
{
	QPixmap& m_pixmapOrigin = m_pixmap;
	//m_rotation += 1;  // 每次增加 10 度

	// 使用 QPainter 进行旋转
	int imageWidth = m_pixmapOrigin.width();
	int imageHeight = m_pixmapOrigin.height();
	QPixmap temp(m_pixmapOrigin.size());
	temp.fill(Qt::transparent);
	QPainter painter(&temp);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.translate(imageWidth / 2, imageHeight / 2);  // 将旋转中心移动到图片中心
	painter.rotate(1);  // 旋转图片
	painter.translate(-(imageWidth / 2), -(imageHeight / 2));  // 将旋转中心移回原来的位置
	painter.drawPixmap(0, 0, m_pixmapOrigin);
	painter.end();

	m_pixmap = temp;

	ui->m_picLabel->setPixmap(temp);  // 将旋转后的 QPixmap 设置为 QLabel 的显示内容
}

void KMainWindow::rotate(double rotation)
{
	QPixmap& m_pixmapOrigin = m_pixmap;
	//m_rotation += 1;  // 每次增加 10 度

	// 使用 QPainter 进行旋转
	int imageWidth = m_pixmapOrigin.width();
	int imageHeight = m_pixmapOrigin.height();
	QPixmap temp(m_pixmapOrigin.size());
	temp.fill(Qt::transparent);
	QPainter painter(&temp);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.translate(imageWidth / 2, imageHeight / 2);  // 将旋转中心移动到图片中心
	painter.rotate(rotation);  // 旋转图片
	painter.translate(-(imageWidth / 2), -(imageHeight / 2));  // 将旋转中心移回原来的位置
	painter.drawPixmap(0, 0, m_pixmapOrigin);
	painter.end();

	//m_pixmap = temp;

	ui->m_picLabel->setPixmap(temp);  // 将旋转后的 QPixmap 设置为 QLabel 的显示内容
}


void KMainWindow::toggleRotation() 
{
	if (m_timer.isActive()) 
	{
		m_timer.stop();  // 停止定时器，停止旋转图片
	}
	else 
	{
		m_timer.start(1000);  // 启动定时器，开始旋转图片
	}
}
void KMainWindow::positionChangedHandle()
{
	int value = ui->m_playingPercentHorizontalSlider->value();
	int dura = m_songOnlineModel->getDurationByIndex(m_mediaPlayerManager->getCurrentIndex());
	value = int((value * 1.0 / 100) * dura * 1000);
	m_mediaPlayerManager->setPosition(value);
}



void KMainWindow::voiceChangedHandle(int value)
{
	m_mediaPlayerManager->setVolume(value);
	if (value == 99)
	{
		value = 100;
	}
	ui->m_voiceLabel->setText(QString::fromLocal8Bit("音量:%1").arg(value == 99 ? 100 : value));
}

void KMainWindow::voiceSwitchHandle()
{
	int volume = m_mediaPlayerManager->switchVolume();
	ui->m_voiceVerticalSlider->setValue(volume);
	ui->m_voiceLabel->setText(QString::fromLocal8Bit("音量:%1").arg(volume == 99 ? 100 : volume));
	if (volume == 0)
	{
		QIcon icon(":/image/volume.png");
		ui->m_voiceControlPushButton->setIcon(icon);
	}
	else
	{
		QIcon icon(":/image/soundIcon.png");
		ui->m_voiceControlPushButton->setIcon(icon);
	}
}

void KMainWindow::playModeSwitchHandle()
{
	QString mode = m_mediaPlayerManager->getPlayMode();
	if (mode == "sortPlay")
	{
		m_mediaPlayerManager->setPlayMode("randomPlay");
		QIcon icon(":/image/randomPlay.png");
		ui->m_playModePushButton->setIcon(icon);
		ui->m_playModLabel->setText(QString::fromLocal8Bit("随机播放"));
	}
	else if (mode == "randomPlay")
	{
		m_mediaPlayerManager->setPlayMode("singlePlay");
		QIcon icon(":/image/singlePlay.png");
		ui->m_playModePushButton->setIcon(icon);
		ui->m_playModLabel->setText(QString::fromLocal8Bit("单曲循环"));
	}
	else if (mode == "singlePlay")
	{
		m_mediaPlayerManager->setPlayMode("sortPlay");
		QIcon icon(":/image/sortPlay.png");
		ui->m_playModePushButton->setIcon(icon);
		ui->m_playModLabel->setText(QString::fromLocal8Bit("顺序播放"));
	}
}

void KMainWindow::playLocalMp3ByIndex(int index)
{

	QString path = m_songOnlineModel->getLocalMp3PathByIndex(index);
	if (path.length() == 0)
	{
		ui->m_songWordsLabel->setText(QString("无本地缓存"));
		return; //无本地缓存
	}
	else
	{
		qDebug() << QString::fromLocal8Bit("本地播放：") << path;
		path = m_songOnlineModel->getLocalMp3PathByIndex(index);
		m_mediaPlayerManager->resume(index, path);
		switchPushButton("playing");
		QString lyricPath = m_songOnlineModel->getLocalLyricPathByIndex(index);
		m_lyricsManager->loadLyrics(lyricPath);
	}
}

void KMainWindow::switchPushButton(QString state)
{
	if (state == "playing")
	{
		QIcon icon(":/image/playButton.png");
		ui->m_startPushButton->setIcon(icon);
	}
	else
	{
		QIcon icon(":/image/originalPlayButton.png");
		ui->m_startPushButton->setIcon(icon);
	}


}