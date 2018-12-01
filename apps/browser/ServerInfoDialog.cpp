//
// Created by koncord on 07.01.17.
//

#include <apps/browser/netutils/QueryClient.hpp>
#include "qdebug.h"

#include "ServerInfoDialog.hpp"
#include <algorithm>
#include <utility>
#include <QThread>

using namespace std;
using namespace RakNet;

ThrWorker::ThrWorker(ServerInfoDialog *dialog, QString addr, unsigned short port): addr(std::move(addr)), port(port), stopped(false)
{
    this->dialog = dialog;
}

void ThrWorker::process()
{
    stopped = false;
    auto newSD = QueryClient::Get().Update(SystemAddress(addr.toUtf8(), port));
    if (dialog != nullptr)
        dialog->setData(newSD);
    stopped = true;
    emit finished();
}

ServerInfoDialog::ServerInfoDialog(const QString &addr, QWidget *parent): QDialog(parent)
{
    setupUi(this);
    refreshThread = new QThread;

    QStringList list = addr.split(':');
    worker = new ThrWorker(this, list[0].toLatin1(), list[1].toUShort());
    worker->moveToThread(refreshThread);
    connect(refreshThread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), refreshThread, SLOT(quit()));
    connect(refreshThread, SIGNAL(finished()), this, SLOT(refresh()));

    connect(btnRefresh, &QPushButton::clicked, [this]{
        if (!refreshThread->isRunning())
            refreshThread->start();
    });
}

ServerInfoDialog::~ServerInfoDialog()
{
    worker->dialog = nullptr;
    if (!refreshThread->isRunning())
        refreshThread->terminate();
}

bool ServerInfoDialog::isUpdated()
{
    return sd.first != UNASSIGNED_SYSTEM_ADDRESS;
}

void ServerInfoDialog::setData(std::pair<RakNet::SystemAddress, QueryData> &newSD)
{
    sd = newSD;
}

void ServerInfoDialog::refresh()
{
    if (sd.first != UNASSIGNED_SYSTEM_ADDRESS)
    {
        leAddr->setText(sd.first.ToString(true, ':'));
        lblName->setText(sd.second.GetName());
        int ping = PingRakNetServer(sd.first.ToString(false), sd.first.GetPort());
        if (ping == PING_UNREACHABLE) {
            lblPing->setText("Unreachable");
        } else {
            lblPing->setNum(ping);
        }
        btnConnect->setDisabled(ping == PING_UNREACHABLE);

        listPlayers->clear();
        for (const auto &player : sd.second.players)
            listPlayers->addItem(QString::fromStdString(player));

        listPlugins->clear();
        for (const auto &plugin : sd.second.plugins)
            listPlugins->addItem(QString::fromStdString(plugin.name));

        listRules->clear();
        const static vector<std::string> defaultRules {"gamemode", "maxPlayers", "name", "passw", "players", "version"};
        for (auto &rule : sd.second.rules)
        {
            if (::find(defaultRules.begin(), defaultRules.end(), rule.first) != defaultRules.end())
                continue;
            QString ruleStr = QString::fromStdString(rule.first) + " : ";
            if (rule.second.type == 's')
                ruleStr += QString::fromStdString(rule.second.str);
            else
                ruleStr += QString::number(rule.second.val);
            listRules->addItem(ruleStr);
        }

        lblPlayers->setText(QString::number(sd.second.players.size()) + " / " + QString::number(sd.second.GetMaxPlayers()));
    }
}

int ServerInfoDialog::exec()
{
    if (!refreshThread->isRunning())
        refreshThread->start();
    return QDialog::exec();
}
