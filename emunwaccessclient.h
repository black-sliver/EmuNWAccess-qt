// EmuNWAccessClient-qt is a simple async qt implementation of EmuNWAccess Protocoll
// Copyright 2021 black-sliver
// Released under MIT license

#ifndef EMUNWACCESSCLIENT_H
#define EMUNWACCESSCLIENT_H

#include <QObject>
#include <QQueue>
#include <QTcpSocket>

class EmuNWAccessClient : public QObject
{
    Q_OBJECT
public:
    explicit EmuNWAccessClient(QObject *parent = nullptr);

    struct Reply {
        bool isValid=false;
        bool isBinary=false;
        bool isAscii=false;
        bool isError=false;
        QByteArray binary;
        QStringList ascii;
        QString error;
        QString cmd;

        QMap<QString,QString> toMap() const;
        QList< QMap<QString,QString> > toMapList() const;
        const QString& operator[](const QString& key);
        bool contains(const QString& key);

        static Reply makeBinary(const QByteArray& binary, QString&& cmd);
        static Reply makeBinary(QByteArray&& binary, QString&& cmd);
        static Reply makeAscii(const QString& text, QString&& cmd);
        static Reply makeAscii(QString&& text, QString&& cmd);
        static Reply makeInvalid();
    private:
        void parse();
        QMap<QString,QString> _mapCache;
    };

    QString error() const;
    bool isConnected() const;

    bool connectToHost(const QString& host, quint16 port);
    bool disconnectFromHost();
    bool waitForConnected(int msecs = 5000);
    bool waitForDisconnected(int msecs = 5000);
    bool waitForBytesWritten(int msecs = 5000);
    bool waitForReadyRead(int msecs = 5000);
    Reply readReply();

    void cmd(const QString& cmd, const QString& args="");
    void cmd(const QString& cmd, const QString& args, const QByteArray& data);

    void cmdEmuInfo() { cmd("EMU_INFO"); }
    void cmdEmuStatus() { cmd("EMU_STATUS"); }
    void cmdEmuReset() { cmd("EMU_RESET"); }
    void cmdEmuStop() { cmd("EMU_STOP"); }
    void cmdEmuPause() { cmd("EMU_PAUSE"); }
    void cmdEmuResume() { cmd("EMU_RESUME"); }
    void cmdEmuReload() { cmd("EMU_RELOAD"); }
    void cmdLoadGame(const QString &filename) { cmd("LOAD_GAME", filename); }
    void cmdGameInfo() { cmd("GAME_INFO"); }
    void cmdDebugBreak() { cmd("DEBUG_BREAK"); }
    void cmdDebugContinue() { cmd("DEBUG_CONTINUE"); }
    void cmdCoresList(const QString& platform="") { cmd("CORES_LIST", platform); }
    void cmdCoreInfo(const QString& core) { cmd("CORE_INFO", core); }
    void cmdCoreCurrentInfo() { cmd("CORE_CURRENT_INFO"); }
    void cmdCoreReset() { cmd("CORE_RESET"); }
    void cmdLoadCore(const QString& core="") { cmd("LOAD_CORE", core); }
    void cmdCoreMemories() { cmd("CORE_MEMORIES"); }
    void cmdCoreWriteMemory(const QString& memory, const QByteArray& data, const QString& addrs) { cmd("CORE_WRITE", addrs.isEmpty() ? memory : (memory+";"+addrs), data); }
    void cmdCoreWriteMemory(const QString& memory, const QByteArray& data, int start=0);
    void cmdCoreWriteMemory(const QString& memory, const QByteArray& data, QList< QPair<int,int> >& regions);
    void cmdCoreWriteMemory(const QString& memory, const QList< QPair<int,QByteArray> >& regions);
    void cmdCoreReadMemory(const QString& memory, const QString& addrs) { cmd("CORE_READ", addrs.isEmpty() ? memory : (memory+";"+addrs)); }
    void cmdCoreReadMemory(const QString& memory, int start=0, int len=-1);
    void cmdCoreReadMemory(const QString& memory, const QList< QPair<int,int> >& regions);

    void cmdCoreWriteMemoryPrepare(const QString& memory, const QString& addrs, int len) { cmdPrepare("CORE_WRITE", memory+";"+addrs, len); }
    void cmdCoreWriteMemoryPrepare(const QString& memory, QList< QPair<int,int> > regions);
    void cmdCoreWriteMemoryData(const QByteArray& data);

private:
    bool _connected;
    bool _connecting;
    QString _lastError;
    QTcpSocket *_socket;
    QByteArray _buffer;
    QQueue<Reply> _queue;
    QQueue<QString> _sent;

protected:
    void cmdPrepare(const QString& cmd, const QString& args, const int datalength);
    void cmdData(const QByteArray& data);

signals:
    void disconnected();
    void connected();
    void connectError();
    void readyRead();

private slots:
    void on_socket_connected();
    void on_socket_disconnected();
    void on_socket_errorOccured(QAbstractSocket::SocketError socketError);
    void on_socket_readyRead();
};

QDebug operator<<(QDebug debug, const EmuNWAccessClient::Reply& reply);

#endif // EMUNWACCESSCLIENT_H
