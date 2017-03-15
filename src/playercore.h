#ifndef MPLAYER_H
#define MPLAYER_H

#include <QWidget>
#include <QHash>
class QTimer;
class QMenu;
class QProcess;
class QResizeEvent;
class QLabel;
class DanmakuLoader;
#include <mpv/client.h>

class PlayerCore : public QWidget
{
    Q_OBJECT

signals:
    void cutVideo(void);
    void played(void);
    void paused(void);
    void stopped(void);
    void fullScreen(void);
    void newFile(const QString &name, const QString &path);
    void timeChanged(int pos);
    void lengthChanged(int len);
    void sizeChanged(const QSize &size);

public:
    typedef enum {STOPPING, VIDEO_PLAYING, VIDEO_PAUSING, TV_PLAYING} State;
    explicit PlayerCore(QWidget *parent = 0);
    ~PlayerCore();
    State state;
    inline QString currentFile() { return file; }
    inline int getTime() {return time;}
    inline int getLength() {return length;}

public slots:
    void stop(void);
    void changeState(void);
    void jumpTo(int pos);
    void setProgress(int pos);
    void setVolume(int volume);
    void openFile(const QString &file, const QString &danmaku = QString());
    void screenShot(void);
    void speedUp(void);
    void speedDown(void);
    void speedSetToDefault(void);
    void switchDanmaku(void);
    void showText(const QByteArray &text);

protected:
    void mouseDoubleClickEvent(QMouseEvent *);
    bool event(QEvent *e);

private:
    mpv_handle *mpv;
    DanmakuLoader *danmakuLoader;
    QLabel *msgLabel;
    QMenu *menu;
    QString file;
    QString danmaku;
    int64_t length;
    int64_t time;
    int64_t videoWidth;
    int64_t videoHeight;
    double speed;
    bool no_emit_stopped;
    bool reload_when_idle;
    bool emit_stopped_when_idle;
    bool danmaku_visible;
    bool openfile_called;
    bool unseekable_forced;

    void loadDanmaku(void);
    void handleMpvError(int code);

private slots:
    void loadAss(const QString &assFile);
    void setRatio_16_9(void);
    void setRatio_16_10(void);
    void setRatio_4_3(void);
    void setRatio_0(void);
    void showMenu(const QPoint&);
};

extern PlayerCore *player_core;

#endif // MPLAYER_H
