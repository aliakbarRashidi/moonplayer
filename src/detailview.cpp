#include "detailview.h"
#include "ui_detailview.h"
#include "utils.h"
#include "accessmanager.h"
#include "plugin.h"
#include "settings_player.h"
#include "yougetbridge.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QDebug>

DetailView::DetailView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DetailView)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->playPushButton, SIGNAL(clicked()), this, SLOT(onPlay()));
    connect(ui->sourceListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onPlay()));
    connect(ui->downloadPushButton, SIGNAL(clicked()), this, SLOT(onDownload()));
}

DetailView::~DetailView()
{
    delete ui;
}

PyObject* DetailView::loadDetail(PyObject *dict)
{
    static QString nameFmt = "<span style=\" font-size:16pt; font-weight:600;\">%1</span> (rating: %2)";
    if (!PyDict_Check(dict))
    {
        PyErr_SetString(PyExc_TypeError, "The argument is not a dict.");
        return NULL;
    }

    PyObject *item;
    QString name;
    //name
    if (NULL != (item = PyDict_GetItemString(dict, "name")))
    {
        name = PyString_AsQString(item);
        setWindowTitle(name + tr(" - Detail page"));
    }

    //rating
    if (NULL != (item = PyDict_GetItemString(dict, "rating")))
        ui->nameLabel->setText(nameFmt.arg(name, QString::number(PyFloat_AsDouble(item))));
    else
        ui->nameLabel->setText(nameFmt.arg(name, tr("Unknown")));

    //length
    if (NULL != (item = PyDict_GetItemString(dict, "length")))
        ui->lengthLabel->setText(PyString_AsQString(item));
    else
        ui->lengthLabel->setText(tr("Unknown"));

    //summary
    if (NULL != (item = PyDict_GetItemString(dict, "summary")))
        ui->summaryLabel->setText(PyString_AsQString(item));
    else
        ui->summaryLabel->setText(tr("Unknown"));

    //others
    struct Item {const char *item_name; QLabel *label;};
    struct Item items[] = {
        {"directors", ui->directorLabel},
        {"script_writers", ui->scriptwriterLabel},
        {"players", ui->playerLabel},
        {"types", ui->typeLabel},
        {"nations", ui->nationLabel},
        {"languages", ui->langLabel},
        {"dates", ui->dateLabel},
        {"alt_names", ui->alternameLabel},
        {NULL, NULL}
    };
    for (struct Item *i = items; i->item_name; i++) {
        item = PyDict_GetItemString(dict, i->item_name);
        if (item) {
            QStringList list = PyList_AsQStringList(item);
            i->label->setText(list.join(" / ").simplified());
        }
        else
            i->label->setText(tr("Unknown"));
    }


    // Source
    ui->sourceListWidget->clear();
    urls.clear();
    item = PyDict_GetItemString(dict, "source");
    if (item)
    {
        int n = PyList_Size(item);
        for (int i = 0; i < n; i += 2)
        {
            QString name = PyString_AsQString(PyList_GetItem(item, i));
            const char *url = PyString_AsString(PyList_GetItem(item, i+1));
            ui->sourceListWidget->addItem(name);
            urls.append(url);
        }
    }

    // Image
    item = PyDict_GetItemString(dict, "image");
    if (item)
    {
        QNetworkRequest request(QUrl(PyString_AsQString(item)));
        request.setRawHeader("User-Agent", "moonplayer");
        reply = access_manager->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(onImageLoaded()));
    }
    Py_IncRef(Py_None);
    return Py_None;
}

void DetailView::onImageLoaded()
{
    QPixmap pic;
    pic.loadFromData(reply->readAll());
    if (pic.height() > 300 * Settings::uiScale)
        pic = pic.scaledToHeight(300 * Settings::uiScale, Qt::SmoothTransformation);
    reply->deleteLater();
    reply = NULL;
    ui->picLabel->setPixmap(pic);
    ui->picLabel->setFixedSize(pic.size());
}

void DetailView::onPlay()
{
    int current_row = ui->sourceListWidget->currentRow();
    if (current_row < 0)
        return;

    QString host = QUrl(QString::fromUtf8(urls[current_row])).host();
    Plugin *plugin = getPluginByHost(host);
    if (plugin)
        plugin->parse(urls[current_row].constData(), false);
    else
        flvcd_parser->parse(urls[current_row].constData(), false);
}

void DetailView::onDownload()
{
    qDebug() << "onDownload";

    int current_row = ui->sourceListWidget->currentRow();
    if (current_row < 0)
        return;

    QString host = QUrl(QString::fromUtf8(urls[current_row])).host();

    qDebug() << "onDownload1";

    Plugin *plugin = getPluginByHost(host);
    if (plugin) {
        qDebug() << "onDownload2";
        plugin->parse(urls[current_row].constData(), true);
    }
    else {
        qDebug() << "onDownload3";
        flvcd_parser->parse(urls[current_row].constData(), true);
    }

    qDebug() << "onDownload end";
}
