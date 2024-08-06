@startuml
actor User
participant KMainWindow
participant KMediaPlayerManager
participant KSongTableModel
participant KxHttpRequestManager
participant KxThreadTask

User -> KMainWindow: 启动窗口
KMainWindow -> KSongTableModel: 初始化歌曲列表
KSongTableModel-> KxHttpRequestManager: http请求歌单列表
KxHttpRequestManager-> KSongTableModel: http返回歌单所有信息，更新信息
KSongTableModel-> KMediaPlayerManager: 保存歌曲列表
KMainWindow-> User : 显示歌单

User -> KMainWindow: 播放指定歌曲
KMainWindow -> KSongTableModel: 获取歌曲信息
KSongTableModel-> KxThreadTask: 多线程Http下载歌曲资源(音频、图片、歌词)
KxThreadTask->KSongTableModel: 缓存歌曲资源(音频、图片、歌词)
KSongTableModel-> KMediaPlayerManager: 歌曲资源更新为本地缓存资源
KSongTableModel->KMainWindow : 显示歌曲图片、歌词等信息
KMainWindow-> User: 播放音乐



User -> KMainWindow: 设置播放模式(单曲、顺序、随机)
KMainWindow -> KMediaPlayerManager: 更新当前播放模式
KMainWindow-> User: 响应设置结果


User -> KMainWindow: 暂停/切换歌曲
KMainWindow -> KMediaPlayerManager: 判断播放模式，获取目标歌曲信息
KMediaPlayerManager-> KMainWindow: 更新歌曲图片、歌词等信息
KMainWindow-> User: 响应操作结果


@enduml