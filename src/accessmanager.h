#ifndef ACCESSMANAGER_H
#define ACCESSMANAGER_H

#include <QHash>

class QNetworkAccessManager;
extern QNetworkAccessManager *access_manager;
extern QStringList unseekable_hosts;
extern QHash<QString,QByteArray> referer_table;

#endif // NETWORKMANAGER_H
