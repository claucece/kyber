#include "Connections/IOverlaySender.hpp"
#include "CSNetwork.hpp"

namespace Dissent {
  using Connections::Connection;
  using Connections::IOverlaySender;

namespace ClientServer {
  CSNetwork::CSNetwork(const QSharedPointer<ConnectionManager> &cm,
      const QSharedPointer<RpcHandler> &rpc,
      const QSharedPointer<GroupHolder> &group_holder) :
    DefaultNetwork(cm, rpc),
    _group_holder(group_holder),
    _forwarder(CSForwarder::Get(cm->GetId(), cm->GetConnectionTable(),
              rpc, group_holder)),
    _broadcaster(new CSBroadcast(cm, rpc, group_holder, _forwarder))
  {
  }

  CSNetwork::~CSNetwork()
  {
  }

  void CSNetwork::Broadcast(const QByteArray &data)
  {
    QVariantList msg;
    msg.append(GetConnectionManager()->GetId().GetByteArray());
    msg.append(GetMethod());
    QVariantHash packet(GetHeaders());
    packet["data"] = data;
    msg.append(packet);

    foreach(const QSharedPointer<Connection> &con,
        GetConnectionManager()->GetConnectionTable().GetConnections())
    {
      if(!_group_holder->GetGroup().Contains(con->GetRemoteId())) {
        continue;
      }

      GetRpcHandler()->SendNotification(con, "CS::Broadcast", msg);
    }
  }
}
}
