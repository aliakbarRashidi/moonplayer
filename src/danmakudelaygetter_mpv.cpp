#include "danmakudelaygetter.h"
#include "downloader.h"
#include "playlist.h"
#include <QCoreApplication>

static void postEvent(void *ptr)
{
    DanmakuDelayGetter *g = (DanmakuDelayGetter*) ptr;
    QCoreApplication::postEvent(g, new QEvent(QEvent::User));
}

DanmakuDelayGetter::DanmakuDelayGetter(QStringList &names, QStringList &urls,
                                       const QString &danmakuUrl, bool download, QObject *parent) :
    QObject(parent), names(names), urls(urls), danmakuUrl(danmakuUrl), download(download)
{
    mpv = mpv_create();
    if (!mpv)
    {
        qDebug("Fails to create mpv instance.");
        deleteLater();
        return;
    }
    mpv_set_option(mpv, "no-video", MPV_FORMAT_NONE, NULL);
    mpv_set_option(mpv, "pause", MPV_FORMAT_NONE, NULL);
    mpv_set_option_string(mpv, "ao", "null");
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_set_wakeup_callback(mpv, postEvent, this);
    if (mpv_initialize(mpv) < 0)
    {
        qDebug("Fails to initialaze mpv instance.");
        deleteLater();
        return;
    }
    delay = 0;
    start();
}

DanmakuDelayGetter::~DanmakuDelayGetter()
{
    if (mpv)
    {
        mpv_detach_destroy(mpv);
        mpv = NULL;
    }
}

void DanmakuDelayGetter::start()
{
    QString name = names.takeFirst();
    QString url = urls.takeFirst();

    // Add clip
    if (delay < 0.5)
    {
        if (download)
            downloader->addTask(url.toUtf8(), name, true, danmakuUrl.toUtf8());
        else
            playlist->addFileAndPlay(name, url, danmakuUrl);
    }
    else
    {
        if (download)
            downloader->addTask(url.toUtf8(), name, true,
                                QByteArray::number(delay) + ' ' + danmakuUrl.toUtf8());
        else
            playlist->addFile(name, url, QString::number(delay) + ' ' + danmakuUrl);
    }

    // Parse next clip
    if (!urls.isEmpty())
    {
        QByteArray tmp = url.toUtf8();
        const char *args[] = {"loadfile", tmp.constData(), NULL};
        mpv_command_async(mpv, 2, args);
    }
    else // Finished
    {
        const char *args[] = {"stop", NULL};
        mpv_command_async(mpv, 2, args);
    }
}

bool DanmakuDelayGetter::event(QEvent *e)
{
    if (e->type() != QEvent::User)
        return QObject::event(e);

    while (mpv)
    {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event == NULL || event->event_id == MPV_EVENT_NONE)
            break;

        switch (event->event_id)
        {
        case MPV_EVENT_PROPERTY_CHANGE:
        {
            mpv_event_property *prop = (mpv_event_property*) event->data;
            if (prop->data == NULL)
                break;
            if (QByteArray(prop->name) == "duration")
            {
                double len = *(double*) prop->data;
                if (len > 0.5)
                {
                    delay += len;
                    start();
                }
            }
            break;
        }
        case MPV_EVENT_IDLE:
            if (urls.isEmpty()) // Finished
                deleteLater();
            break;
        default: break;
        }
    }
    return true;
}
