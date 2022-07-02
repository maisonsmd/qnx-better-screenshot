#ifndef IPCTRANSCEIVER_H
#define IPCTRANSCEIVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QJsonObject>
#include <QTimer>

class Transceiver : public QObject
{
    Q_OBJECT
public:
    enum IpcId {
        IpcId_Unknown = -1,
        IpcId_Target,
        IpcId_PC,
    };
    Q_ENUM(IpcId);

    struct Peer {
        IpcId id;
        QTcpSocket *socket;

        bool operator==(const Peer &rhs) {
            return id == rhs.id
                    && socket == rhs.socket;
        }
    };

    static Transceiver *instance();
    void init(IpcId id = IpcId_Target);

    void broadcast(const QJsonObject &obj);
    void send(const QJsonObject &obj, IpcId targetId = IpcId_Target);
    void send(const QJsonObject &obj, QTcpSocket *socket);
    bool isClientConnected(IpcId id);
    IpcId lastSender() const;

    QJsonObject header(int function);

signals:
    bool sigHandleMessage(const QJsonObject &obj);
    void sigServerConnectionChanged(bool connected);
    void sigClientConnectionChanged(Transceiver::IpcId &id, bool connected);

private slots:
    void onNewConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState state);
    void onReadyRead();
    void onReconnectTimer();

private:
    Transceiver();
    bool isServer() { return m_id == IpcId_Target; }
    void registerToServer();
    void handleMessage(QTcpSocket *sender, const QJsonObject &obj);

    IpcId m_id;
    IpcId m_lastSender;

    bool m_initialized;

    QTcpServer *m_server;
    QTcpSocket *m_client;
    QList<Peer> m_peers;

    QTimer m_reconnectTimer;
    int m_sendPacketId;
    int m_lastReceivePacketId;
};

Q_DECLARE_METATYPE(Transceiver::IpcId);

#endif // IPCTRANSCEIVER_H
