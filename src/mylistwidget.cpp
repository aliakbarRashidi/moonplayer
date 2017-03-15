#include "mylistwidget.h"
#include "accessmanager.h"
#include "settings_player.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>

//MyListWidgetItem
MyListWidgetItem::MyListWidgetItem(const QString &name, const QByteArray &pic_url, const QByteArray &flag) :
    QListWidgetItem(name)
{
    m_picUrl = pic_url;
    m_flag = flag;
}

MyListWidget::MyListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setViewMode(QListWidget::IconMode);
    setSpacing(4);
    setResizeMode(QListWidget::Adjust);
    setWrapping(true);
    setMovement(QListWidget::Static);
    loading_item = NULL;
    reply = NULL;
}

void MyListWidget::addPicItem(const QString &name, const QByteArray &picUrl, const QByteArray &flag)
{
    MyListWidgetItem *item = new MyListWidgetItem(name, picUrl, flag);
    items_to_load_pic << item;
    if (loading_item == NULL)
        loadNextPic();
}


void MyListWidget::loadNextPic()
{
    loading_item = items_to_load_pic.takeFirst();
    QNetworkRequest request(QString::fromUtf8(loading_item->picUrl()));
    request.setRawHeader("User-Agent", "moonplayer");
    reply = access_manager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onLoadPicFinished()));
}

void MyListWidget::onLoadPicFinished()
{
    if (loading_item)
    {
        QPixmap pic;
        pic.loadFromData(reply->readAll());
        if (pic.width() > 100 * Settings::uiScale)
            pic = pic.scaledToWidth(100 * Settings::uiScale, Qt::SmoothTransformation);
        loading_item->setIcon(QIcon(pic));
        loading_item->setSizeHint(pic.size() + QSize(10, 20) * Settings::uiScale);
        if (count() == 0) // First item
            setIconSize(pic.size());
        addItem(loading_item);
    }
    reply->deleteLater();
    reply = NULL;
    if (items_to_load_pic.size())
        loadNextPic();
    else
        loading_item = NULL;
}

void MyListWidget::clearItem()
{
    loading_item = NULL;
    QList<MyListWidgetItem*> items_to_clear = items_to_load_pic;
    items_to_load_pic.clear();
    clear();
    while (!items_to_clear.isEmpty())
    {
        MyListWidgetItem *item = items_to_clear.takeFirst();
        delete item;
    }
}
