/* (c) unsigned char*. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at https://github.com/CytraL/HClient */
#ifndef ENGINE_CLIENT_IRC_H
#define ENGINE_CLIENT_IRC_H

#include <base/system.h>
#include <engine/irc.h>
#include <list>
#include <string>

class CIrc : public IIrc
{
public:
    CIrc();

    void Init();

    int GetState() { return m_State; }
    void NextRoom();

    void SetActiveCom(size_t index);
    CIrcCom* GetActiveCom();
    CIrcCom* GetCom(size_t index);
    CIrcCom* GetCom(std::string name);
    int GetNumComs() { return m_IrcComs.size(); }

    void OpenQuery(const char *to);
    void JoinTo(const char *to);
    void SetTopic(const char *topic);
    void Part();

    void SetMode(const char *mode, const char *to);
    void SetNick(const char *nick);
    const char* GetNick() { return m_Nick.c_str(); }

    void SendMsg(const char *to, const char *msg, int type = MSG_TYPE_NORMAL);
    void SendRaw(const char *fmt, ...);

    void StartConnection();
    void EndConnection();

    void SetAway(bool state, const char *msg = 0x0);

    std::string m_Nick;

protected:
    class IStorage *m_pStorage;
    class IGraphics *m_pGraphics;
    class IGameClient *m_pGameClient;
    class IClient *m_pClient;

    int m_State;
    int m_ActiveCom;
    NETSOCKET m_Socket;
    NETADDR m_HostAddress;

    long m_CmdToken;

    std::list<CIrcCom*> m_IrcComs;

private:
    int GetMsgType(const char *msg);
    void SendServer(const char *to, long Token);
    void SendGetServer(const char *to);
};
#endif
