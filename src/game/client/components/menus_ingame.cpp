/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/math.h>

#include <engine/config.h>
#include <engine/demo.h>
#include <engine/friends.h>
#include <engine/graphics.h>
#include <engine/serverbrowser.h>
#include <engine/textrender.h>
#include <engine/storage.h> //H-Client: Ghost
#include <engine/keys.h> //H-Client: Ghost
#include <engine/shared/config.h>

#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/localization.h>
#include <game/client/animstate.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>
#include <game/client/ui.h>

#include "menus.h"
#include "motd.h"
#include "voting.h"
#include "ghost.h" //H-Client: Ghost

void CMenus::RenderGame(CUIRect MainView)
{
	CUIRect Button, ButtonBar;
	MainView.HSplitTop(45.0f, &ButtonBar, &MainView);
	RenderTools()->DrawUIRect(&ButtonBar, ms_ColorTabbarActive, CUI::CORNER_ALL, 10.0f);

	// button bar
	ButtonBar.HSplitTop(10.0f, 0, &ButtonBar);
	ButtonBar.HSplitTop(25.0f, &ButtonBar, 0);
	ButtonBar.VMargin(10.0f, &ButtonBar);

	ButtonBar.VSplitRight(120.0f, &ButtonBar, &Button);
	static int s_DisconnectButton = 0;
	if(DoButton_Menu(&s_DisconnectButton, Localize("Disconnect"), 0, &Button))
		Client()->Disconnect();

	if(m_pClient->m_Snap.m_pLocalInfo && m_pClient->m_Snap.m_pGameInfoObj)
	{
		if(m_pClient->m_Snap.m_pLocalInfo->m_Team != TEAM_SPECTATORS)
		{
			ButtonBar.VSplitLeft(10.0f, 0, &ButtonBar);
			ButtonBar.VSplitLeft(120.0f, &Button, &ButtonBar);
			static int s_SpectateButton = 0;
			if(DoButton_Menu(&s_SpectateButton, Localize("Spectate"), 0, &Button))
			{
				m_pClient->SendSwitchTeam(TEAM_SPECTATORS);
				SetActive(false);
			}
		}

		if(m_pClient->m_Snap.m_pGameInfoObj->m_GameFlags & GAMEFLAG_TEAMS)
		{
			if(m_pClient->m_Snap.m_pLocalInfo->m_Team != TEAM_RED)
			{
				ButtonBar.VSplitLeft(10.0f, 0, &ButtonBar);
				ButtonBar.VSplitLeft(120.0f, &Button, &ButtonBar);
				static int s_SpectateButton = 0;
				if(DoButton_Menu(&s_SpectateButton, Localize("Join red"), 0, &Button))
				{
					m_pClient->SendSwitchTeam(TEAM_RED);
					SetActive(false);
				}
			}

			if(m_pClient->m_Snap.m_pLocalInfo->m_Team != TEAM_BLUE)
			{
				ButtonBar.VSplitLeft(10.0f, 0, &ButtonBar);
				ButtonBar.VSplitLeft(120.0f, &Button, &ButtonBar);
				static int s_SpectateButton = 0;
				if(DoButton_Menu(&s_SpectateButton, Localize("Join blue"), 0, &Button))
				{
					m_pClient->SendSwitchTeam(TEAM_BLUE);
					SetActive(false);
				}
			}
		}
		else
		{
			if(m_pClient->m_Snap.m_pLocalInfo->m_Team != 0)
			{
				ButtonBar.VSplitLeft(10.0f, 0, &ButtonBar);
				ButtonBar.VSplitLeft(120.0f, &Button, &ButtonBar);
				static int s_SpectateButton = 0;
				if(DoButton_Menu(&s_SpectateButton, Localize("Join game"), 0, &Button))
				{
					m_pClient->SendSwitchTeam(0);
					SetActive(false);
				}
			}
		}
	}

	ButtonBar.VSplitLeft(100.0f, 0, &ButtonBar);
	ButtonBar.VSplitLeft(150.0f, &Button, &ButtonBar);

	static int s_DemoButton = 0;
	bool Recording = DemoRecorder()->IsRecording();
	if(DoButton_Menu(&s_DemoButton, Localize(Recording ? "Stop record" : "Record demo"), 0, &Button))	// Localize("Stop record");Localize("Record demo");
	{
		if(!Recording)
			Client()->DemoRecorder_Start("demo", true);
		else
			Client()->DemoRecorder_Stop();
	}

    CServerInfo Info;
    Client()->GetServerInfo(&Info);
    if (str_find_nocase(Info.m_aGameType, "race"))
    {
        ms_ColorTabbarActive = vec4(1,1,1,0.85f);
        ms_ColorTabbarInactive = vec4(1,1,1,0.15f);

        ButtonBar.VSplitLeft(100.0f, 0, &ButtonBar);
        ButtonBar.VSplitLeft(150.0f, &Button, &ButtonBar);
        static int s_GhostButton=0;
        if(DoButton_MenuTab(&s_GhostButton, Localize("Ghost"), g_Config.m_UiSubPage==SUBPAGE_GHOST, &Button, 0))
        {
            if (g_Config.m_UiSubPage == SUBPAGE_GHOST)
                g_Config.m_UiSubPage = 0;
            else
                g_Config.m_UiSubPage = SUBPAGE_GHOST;
        }
        ms_ColorTabbarActive = vec4(0,0,0,0.85f);
        ms_ColorTabbarInactive = vec4(0,0,0,0.15f);

        if (g_Config.m_UiSubPage == SUBPAGE_GHOST)
            RenderGhost(MainView);
    }
}

void CMenus::RenderPlayers(CUIRect MainView)
{
	CUIRect Button, ButtonBar, Options, Player;
	RenderTools()->DrawUIRect(&MainView, ms_ColorTabbarActive, CUI::CORNER_ALL, 10.0f);

	// player options
	MainView.Margin(10.0f, &Options);
	RenderTools()->DrawUIRect(&Options, vec4(1.0f, 1.0f, 1.0f, 0.25f), CUI::CORNER_ALL, 10.0f);
	Options.Margin(10.0f, &Options);
	Options.HSplitTop(50.0f, &Button, &Options);
	UI()->DoLabelScaled(&Button, Localize("Player options"), 34.0f, -1);

	// headline
	Options.HSplitTop(34.0f, &ButtonBar, &Options);
	ButtonBar.VSplitRight(220.0f, &Player, &ButtonBar);
	UI()->DoLabelScaled(&Player, Localize("Player"), 24.0f, -1);

	ButtonBar.HMargin(1.0f, &ButtonBar);
	float Width = ButtonBar.h*2.0f;
	ButtonBar.VSplitLeft(Width, &Button, &ButtonBar);
	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_GUIICONS].m_Id);
	Graphics()->QuadsBegin();
	RenderTools()->SelectSprite(SPRITE_GUIICON_MUTE);
	IGraphics::CQuadItem QuadItem(Button.x, Button.y, Button.w, Button.h);
	Graphics()->QuadsDrawTL(&QuadItem, 1);
	Graphics()->QuadsEnd();

	ButtonBar.VSplitLeft(20.0f, 0, &ButtonBar);
	ButtonBar.VSplitLeft(Width, &Button, &ButtonBar);
	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_GUIICONS].m_Id);
	Graphics()->QuadsBegin();
	RenderTools()->SelectSprite(SPRITE_GUIICON_FRIEND);
	QuadItem = IGraphics::CQuadItem(Button.x, Button.y, Button.w, Button.h);
	Graphics()->QuadsDrawTL(&QuadItem, 1);
	Graphics()->QuadsEnd();

	// options
	static int s_aPlayerIDs[MAX_CLIENTS][2] = {{0}};
	for(int i = 0, Count = 0; i < MAX_CLIENTS; ++i)
	{
		if(!m_pClient->m_Snap.m_paInfoByTeam[i])
			continue;

		int Index = m_pClient->m_Snap.m_paInfoByTeam[i]->m_ClientID;
		if(Index == m_pClient->m_Snap.m_LocalClientID)
			continue;

		Options.HSplitTop(28.0f, &ButtonBar, &Options);
		if(Count++%2 == 0)
			RenderTools()->DrawUIRect(&ButtonBar, vec4(1.0f, 1.0f, 1.0f, 0.25f), CUI::CORNER_ALL, 10.0f);
		ButtonBar.VSplitRight(220.0f, &Player, &ButtonBar);

		// player info
		Player.VSplitLeft(28.0f, &Button, &Player);
		CTeeRenderInfo Info = m_pClient->m_aClients[Index].m_RenderInfo;
		Info.m_Size = Button.h;
		RenderTools()->RenderTee(CAnimState::GetIdle(), &Info, EMOTE_NORMAL, vec2(1.0f, 0.0f), vec2(Button.x+Button.h/2, Button.y+Button.h/2));

		Player.HSplitTop(1.5f, 0, &Player);
		Player.VSplitMid(&Player, &Button);
		CTextCursor Cursor;
		TextRender()->SetCursor(&Cursor, Player.x, Player.y, 14.0f, TEXTFLAG_RENDER|TEXTFLAG_STOP_AT_END);
		Cursor.m_LineWidth = Player.w;
		TextRender()->TextEx(&Cursor, m_pClient->m_aClients[Index].m_aName, -1);

		TextRender()->SetCursor(&Cursor, Button.x,Button.y, 14.0f, TEXTFLAG_RENDER|TEXTFLAG_STOP_AT_END);
		Cursor.m_LineWidth = Button.w;
		TextRender()->TextEx(&Cursor, m_pClient->m_aClients[Index].m_aClan, -1);

		// ignore button
		ButtonBar.HMargin(2.0f, &ButtonBar);
		ButtonBar.VSplitLeft(Width, &Button, &ButtonBar);
		Button.VSplitLeft((Width-Button.h)/4.0f, 0, &Button);
		Button.VSplitLeft(Button.h, &Button, 0);
		if(DoButton_Toggle(&s_aPlayerIDs[Index][0], m_pClient->m_aClients[Index].m_ChatIgnore, &Button))
			m_pClient->m_aClients[Index].m_ChatIgnore ^= 1;

		// friend button
		ButtonBar.VSplitLeft(20.0f, &Button, &ButtonBar);
		ButtonBar.VSplitLeft(Width, &Button, &ButtonBar);
		Button.VSplitLeft((Width-Button.h)/4.0f, 0, &Button);
		Button.VSplitLeft(Button.h, &Button, 0);
		if(DoButton_Toggle(&s_aPlayerIDs[Index][1], m_pClient->m_aClients[Index].m_Friend, &Button))
		{
			if(m_pClient->m_aClients[Index].m_Friend)
				m_pClient->Friends()->RemoveFriend(m_pClient->m_aClients[Index].m_aName, m_pClient->m_aClients[Index].m_aClan);
			else
				m_pClient->Friends()->AddFriend(m_pClient->m_aClients[Index].m_aName, m_pClient->m_aClients[Index].m_aClan);
		}
	}

	/*
	CUIRect bars;
	votearea.HSplitTop(10.0f, 0, &votearea);
	votearea.HSplitTop(25.0f + 10.0f*3 + 25.0f, &votearea, &bars);

	RenderTools()->DrawUIRect(&votearea, color_tabbar_active, CUI::CORNER_ALL, 10.0f);

	votearea.VMargin(20.0f, &votearea);
	votearea.HMargin(10.0f, &votearea);

	votearea.HSplitBottom(35.0f, &votearea, &bars);

	if(gameclient.voting->is_voting())
	{
		// do yes button
		votearea.VSplitLeft(50.0f, &button, &votearea);
		static int yes_button = 0;
		if(UI()->DoButton(&yes_button, "Yes", 0, &button, ui_draw_menu_button, 0))
			gameclient.voting->vote(1);

		// do no button
		votearea.VSplitLeft(5.0f, 0, &votearea);
		votearea.VSplitLeft(50.0f, &button, &votearea);
		static int no_button = 0;
		if(UI()->DoButton(&no_button, "No", 0, &button, ui_draw_menu_button, 0))
			gameclient.voting->vote(-1);

		// do time left
		votearea.VSplitRight(50.0f, &votearea, &button);
		char buf[256];
		str_format(buf, sizeof(buf), "%d", gameclient.voting->seconds_left());
		UI()->DoLabel(&button, buf, 24.0f, 0);

		// do description and command
		votearea.VSplitLeft(5.0f, 0, &votearea);
		UI()->DoLabel(&votearea, gameclient.voting->vote_description(), 14.0f, -1);
		votearea.HSplitTop(16.0f, 0, &votearea);
		UI()->DoLabel(&votearea, gameclient.voting->vote_command(), 10.0f, -1);

		// do bars
		bars.HSplitTop(10.0f, 0, &bars);
		bars.HMargin(5.0f, &bars);

		gameclient.voting->render_bars(bars, true);

	}
	else
	{
		UI()->DoLabel(&votearea, "No vote in progress", 18.0f, -1);
	}*/
}

void CMenus::RenderServerInfo(CUIRect MainView)
{
	if(!m_pClient->m_Snap.m_pLocalInfo)
		return;

	// fetch server info
	CServerInfo CurrentServerInfo;
	Client()->GetServerInfo(&CurrentServerInfo);

	// render background
	RenderTools()->DrawUIRect(&MainView, ms_ColorTabbarActive, CUI::CORNER_ALL, 10.0f);

	CUIRect View, ServerInfo, GameInfo, Motd;

	float x = 0.0f;
	float y = 0.0f;

	char aBuf[1024];

	// set view to use for all sub-modules
	MainView.Margin(10.0f, &View);

	// serverinfo
	View.HSplitTop(View.h/2/UI()->Scale()-5.0f, &ServerInfo, &Motd);
	ServerInfo.VSplitLeft(View.w/2/UI()->Scale()-5.0f, &ServerInfo, &GameInfo);
	RenderTools()->DrawUIRect(&ServerInfo, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);

	ServerInfo.Margin(5.0f, &ServerInfo);

	x = 5.0f;
	y = 0.0f;

	TextRender()->Text(0, ServerInfo.x+x, ServerInfo.y+y, 32, Localize("Server info"), 250);
	y += 32.0f+5.0f;

	mem_zero(aBuf, sizeof(aBuf));
	str_format(
		aBuf,
		sizeof(aBuf),
		"%s\n\n"
		"%s: %s\n"
		"%s: %d\n"
		"%s: %s\n"
		"%s: %s\n",
		CurrentServerInfo.m_aName,
		Localize("Address"), g_Config.m_UiServerAddress,
		Localize("Ping"), m_pClient->m_Snap.m_pLocalInfo->m_Latency,
		Localize("Version"), CurrentServerInfo.m_aVersion,
		Localize("Password"), CurrentServerInfo.m_Flags &1 ? Localize("Yes") : Localize("No")
	);

	TextRender()->Text(0, ServerInfo.x+x, ServerInfo.y+y, 20, aBuf, 250);

	{
		CUIRect Button;
		int IsFavorite = ServerBrowser()->IsFavorite(CurrentServerInfo.m_NetAddr);
		ServerInfo.HSplitBottom(20.0f, &ServerInfo, &Button);
		static int s_AddFavButton = 0;
		if(DoButton_CheckBox(&s_AddFavButton, Localize("Favorite"), IsFavorite, &Button))
		{
			if(IsFavorite)
				ServerBrowser()->RemoveFavorite(CurrentServerInfo.m_NetAddr);
			else
				ServerBrowser()->AddFavorite(CurrentServerInfo.m_NetAddr);
		}
	}

	// gameinfo
	GameInfo.VSplitLeft(10.0f, 0x0, &GameInfo);
	RenderTools()->DrawUIRect(&GameInfo, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);

	GameInfo.Margin(5.0f, &GameInfo);

	x = 5.0f;
	y = 0.0f;

	TextRender()->Text(0, GameInfo.x+x, GameInfo.y+y, 32, Localize("Game info"), 250);
	y += 32.0f+5.0f;

	if(m_pClient->m_Snap.m_pGameInfoObj)
	{
		mem_zero(aBuf, sizeof(aBuf));
		str_format(
			aBuf,
			sizeof(aBuf),
			"\n\n"
			"%s: %s\n"
			"%s: %s\n"
			"%s: %d\n"
			"%s: %d\n"
			"\n"
			"%s: %d/%d\n",
			Localize("Game type"), CurrentServerInfo.m_aGameType,
			Localize("Map"), CurrentServerInfo.m_aMap,
			Localize("Score limit"), m_pClient->m_Snap.m_pGameInfoObj->m_ScoreLimit,
			Localize("Time limit"), m_pClient->m_Snap.m_pGameInfoObj->m_TimeLimit,
			Localize("Players"), m_pClient->m_Snap.m_NumPlayers, CurrentServerInfo.m_MaxClients
		);
		TextRender()->Text(0, GameInfo.x+x, GameInfo.y+y, 20, aBuf, 250);
	}

	// motd
	Motd.HSplitTop(10.0f, 0, &Motd);
	RenderTools()->DrawUIRect(&Motd, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);
	Motd.Margin(5.0f, &Motd);
	y = 0.0f;
	x = 5.0f;
	TextRender()->Text(0, Motd.x+x, Motd.y+y, 32, Localize("MOTD"), -1);
	y += 32.0f+5.0f;
	TextRender()->Text(0, Motd.x+x, Motd.y+y, 16, m_pClient->m_pMotd->m_aServerMotd, (int)Motd.w);
}

void CMenus::RenderServerControlServer(CUIRect MainView)
{
	static int s_VoteList = 0;
	static float s_ScrollValue = 0;
	CUIRect List = MainView;
	UiDoListboxStart(&s_VoteList, &List, 24.0f, "", "", m_pClient->m_pVoting->m_NumVoteOptions, 1, m_CallvoteSelectedOption, s_ScrollValue);

	for(CVoteOptionClient *pOption = m_pClient->m_pVoting->m_pFirst; pOption; pOption = pOption->m_pNext)
	{
		CListboxItem Item = UiDoListboxNextItem(pOption);

		if(Item.m_Visible)
			UI()->DoLabelScaled(&Item.m_Rect, pOption->m_aDescription, 16.0f, -1);
	}

	m_CallvoteSelectedOption = UiDoListboxEnd(&s_ScrollValue, 0);
}

void CMenus::RenderServerControlKick(CUIRect MainView, bool FilterSpectators)
{
	int NumOptions = 0;
	int Selected = -1;
	static int aPlayerIDs[MAX_CLIENTS];
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(!m_pClient->m_Snap.m_paInfoByTeam[i])
			continue;

		int Index = m_pClient->m_Snap.m_paInfoByTeam[i]->m_ClientID;
		if(Index == m_pClient->m_Snap.m_LocalClientID || (FilterSpectators && m_pClient->m_Snap.m_paInfoByTeam[i]->m_Team == TEAM_SPECTATORS))
			continue;
		if(m_CallvoteSelectedPlayer == Index)
			Selected = NumOptions;
		aPlayerIDs[NumOptions++] = Index;
	}

	static int s_VoteList = 0;
	static float s_ScrollValue = 0;
	CUIRect List = MainView;
	UiDoListboxStart(&s_VoteList, &List, 24.0f, "", "", NumOptions, 1, Selected, s_ScrollValue);

	for(int i = 0; i < NumOptions; i++)
	{
		CListboxItem Item = UiDoListboxNextItem(&aPlayerIDs[i]);

		if(Item.m_Visible)
		{
			CTeeRenderInfo Info = m_pClient->m_aClients[aPlayerIDs[i]].m_RenderInfo;
			Info.m_Size = Item.m_Rect.h;
			Item.m_Rect.HSplitTop(5.0f, 0, &Item.m_Rect); // some margin from the top
			RenderTools()->RenderTee(CAnimState::GetIdle(), &Info, EMOTE_NORMAL, vec2(1,0), vec2(Item.m_Rect.x+Item.m_Rect.h/2, Item.m_Rect.y+Item.m_Rect.h/2));
			Item.m_Rect.x +=Info.m_Size;
			UI()->DoLabelScaled(&Item.m_Rect, m_pClient->m_aClients[aPlayerIDs[i]].m_aName, 16.0f, -1);
		}
	}

	Selected = UiDoListboxEnd(&s_ScrollValue, 0);
	m_CallvoteSelectedPlayer = Selected != -1 ? aPlayerIDs[Selected] : -1;
}


void CMenus::RenderServerControl(CUIRect MainView)
{
    static int s_VoteOpt = 0; //H-Client
    CUIRect MainViewM = MainView; //H-Client
	static int s_ControlPage = 0;

	// render background
	CUIRect Bottom, Extended, TabBar, Button;
	MainView.HSplitTop(20.0f, &Bottom, &MainView);
	RenderTools()->DrawUIRect(&Bottom, ms_ColorTabbarActive, CUI::CORNER_T, 10.0f);
	MainView.HSplitTop(20.0f, &TabBar, &MainView);
	RenderTools()->DrawUIRect(&MainView, ms_ColorTabbarActive, CUI::CORNER_B, 10.0f);
	MainView.Margin(10.0f, &MainView);
	MainViewM.Margin(10.0f, &MainViewM);
	MainView.HSplitBottom(90.0f, &MainView, &Extended);

	// tab bar
	{
		//TabBar.VSplitLeft(TabBar.w/3, &Button, &TabBar);
		TabBar.VSplitMid(&Button, &TabBar);
		static int s_Button0 = 0;
		if(DoButton_MenuTab(&s_Button0, Localize("Change settings"), s_ControlPage == 0, &Button, 0))
			s_ControlPage = 0;

		//TabBar.VSplitMid(&Button, &TabBar);
		static int s_Button1 = 0;
		if(DoButton_MenuTab(&s_Button1, Localize("Player votes"), s_ControlPage == 1, &TabBar, 0))
			s_ControlPage = 1;

		/*static int s_Button2 = 0;
		if(DoButton_MenuTab(&s_Button2, Localize("Move player to spectators"), s_ControlPage == 2, &TabBar, 0))
			s_ControlPage = 2;*/
	}

	// render page
	MainView.HSplitBottom(ms_ButtonHeight + 5*2, &MainView, &Bottom);
	Bottom.HMargin(5.0f, &Bottom);

	if(s_ControlPage == 0)
		RenderServerControlServer(MainView);
	else if(s_ControlPage == 1)
		RenderServerControlKick(MainView, false);
	/*else if(s_ControlPage == 2)
		RenderServerControlKick(MainView, true);*/

	// vote menu
	{
		CUIRect Button;

        //H-Client: Options
        if(s_ControlPage == 1)
        {
            CUIRect CallOptions;
            MainViewM.HSplitTop(MainView.h+TabBar.h+25.0f, 0x0, &CallOptions);
            CallOptions.VSplitLeft(250.0f, &CallOptions, 0x0);
            CallOptions.HSplitTop(20.0f, &Button, &CallOptions);

            CUIRect LButton, MButton, RButton;
            Button.VSplitLeft(Button.w/3, &LButton, &MButton);
            MButton.VSplitMid(&MButton, &RButton);
            static int s_CallVoteKick = 0;
            static int s_CallVoteSpectate = 0;
            static int s_CallVoteBan = 0;
            vec4 orgColor = ms_ColorTabbarActive;
            ms_ColorTabbarActive = vec4(1.0f, 0.39f, 0.0f, 1.0f);
            if (DoButton_MenuTab(&s_CallVoteKick, Localize("Kick"), (s_VoteOpt==0?1:0), &LButton, 0))
                s_VoteOpt = 0;

            if (m_CallvoteSelectedPlayer >= 0 && m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer]->m_Team != TEAM_SPECTATORS)
            {
                if (DoButton_MenuTab(&s_CallVoteSpectate, Localize("Spectator"), (s_VoteOpt==1?1:0), &MButton, 0))
                    s_VoteOpt = 1;
            }

            if (m_CallvoteSelectedPlayer >= 0 && Client()->RconAuthed())
            {
                if (DoButton_MenuTab(&s_CallVoteBan, Localize("Ban"), (s_VoteOpt==2?1:0), &RButton, 0))
                    s_VoteOpt = 2;
            }

            ms_ColorTabbarActive = orgColor;
        }

		Bottom.VSplitRight(120.0f, &Bottom, &Button);

		static int s_CallVoteButton = 0;
		if(DoButton_Menu(&s_CallVoteButton, Localize("Call vote"), 0, &Button))
		{
			if(s_ControlPage == 0)
				m_pClient->m_pVoting->CallvoteOption(m_CallvoteSelectedOption, m_aCallvoteReason);
			else if(s_ControlPage == 1)
			{
				/*if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
					m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
				{
					m_pClient->m_pVoting->CallvoteKick(m_CallvoteSelectedPlayer, m_aCallvoteReason);
					SetActive(false);
				}*/

                if(s_VoteOpt == 0)
                {
                    if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
                        m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
                    {
                        m_pClient->m_pVoting->CallvoteKick(m_CallvoteSelectedPlayer, m_aCallvoteReason, false);
                        SetActive(false);
                    }
                }
                else if(s_VoteOpt == 1)
                {
                    if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
                        m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
                    {
                        m_pClient->m_pVoting->CallvoteSpectate(m_CallvoteSelectedPlayer, m_aCallvoteReason, false);
                        SetActive(false);
                    }
                }
			}
			/*else if(s_ControlPage == 2)
			{
				if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
					m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
				{
					m_pClient->m_pVoting->CallvoteSpectate(m_CallvoteSelectedPlayer, m_aCallvoteReason);
					SetActive(false);
				}
			}*/
			m_aCallvoteReason[0] = 0;
		}

		// render kick reason
		CUIRect Reason;
		Bottom.VSplitRight(40.0f, &Bottom, 0);
		Bottom.VSplitRight(160.0f, &Bottom, &Reason);
		Reason.HSplitTop(5.0f, 0, &Reason);
		const char *pLabel = Localize("Reason:");
		UI()->DoLabelScaled(&Reason, pLabel, 14.0f, -1);
		float w = TextRender()->TextWidth(0, 14.0f, pLabel, -1);
		Reason.VSplitLeft(w+10.0f, 0, &Reason);
		static float s_Offset = 0.0f;
		DoEditBox(&m_aCallvoteReason, &Reason, m_aCallvoteReason, sizeof(m_aCallvoteReason), 14.0f, &s_Offset, false, CUI::CORNER_ALL);

		// extended features (only available when authed in rcon)
		if(Client()->RconAuthed())
		{
			// background
			Extended.Margin(10.0f, &Extended);
			Extended.HSplitTop(20.0f, &Bottom, &Extended);
			Extended.HSplitTop(5.0f, 0, &Extended);

			// force vote
			Bottom.VSplitLeft(5.0f, 0, &Bottom);
			Bottom.VSplitLeft(120.0f, &Button, &Bottom);
			static int s_ForceVoteButton = 0;
			if(DoButton_Menu(&s_ForceVoteButton, Localize("Force vote"), 0, &Button))
			{
				if(s_ControlPage == 0)
					m_pClient->m_pVoting->CallvoteOption(m_CallvoteSelectedOption, m_aCallvoteReason, true);
				else if(s_ControlPage == 1)
				{
					/*if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
						m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
					{
						m_pClient->m_pVoting->CallvoteKick(m_CallvoteSelectedPlayer, m_aCallvoteReason, true);
						SetActive(false);
					}*/

                    if(s_VoteOpt == 0)
                    {
                        if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
                            m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
                        {
                            m_pClient->m_pVoting->CallvoteKick(m_CallvoteSelectedPlayer, m_aCallvoteReason, true);
                            SetActive(false);
                        }
                    }
                    else if(s_VoteOpt == 1)
                    {
                        if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
                            m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
                        {
                            m_pClient->m_pVoting->CallvoteSpectate(m_CallvoteSelectedPlayer, m_aCallvoteReason, true);
                            SetActive(false);
                        }
                    }
                    else if(s_VoteOpt == 2)
                    {
                        if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
                            m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
                        {
                            m_pClient->m_pVoting->CallvoteBan(m_CallvoteSelectedPlayer, m_aCallvoteReason, true);
                            SetActive(false);
                        }
                    }
				}
				/*else if(s_ControlPage == 2)
				{
					if(m_CallvoteSelectedPlayer >= 0 && m_CallvoteSelectedPlayer < MAX_CLIENTS &&
						m_pClient->m_Snap.m_paPlayerInfos[m_CallvoteSelectedPlayer])
					{
						m_pClient->m_pVoting->CallvoteSpectate(m_CallvoteSelectedPlayer, m_aCallvoteReason, true);
						SetActive(false);
					}
				}*/
				m_aCallvoteReason[0] = 0;
			}

			if(s_ControlPage == 0)
			{
				// remove vote
				Bottom.VSplitRight(10.0f, &Bottom, 0);
				Bottom.VSplitRight(120.0f, 0, &Button);
				static int s_RemoveVoteButton = 0;
				if(DoButton_Menu(&s_RemoveVoteButton, Localize("Remove"), 0, &Button))
					m_pClient->m_pVoting->RemovevoteOption(m_CallvoteSelectedOption);


				// add vote
				Extended.HSplitTop(20.0f, &Bottom, &Extended);
				Bottom.VSplitLeft(5.0f, 0, &Bottom);
				Bottom.VSplitLeft(250.0f, &Button, &Bottom);
				UI()->DoLabelScaled(&Button, Localize("Vote description:"), 14.0f, -1);

				Bottom.VSplitLeft(20.0f, 0, &Button);
				UI()->DoLabelScaled(&Button, Localize("Vote command:"), 14.0f, -1);

				static char s_aVoteDescription[64] = {0};
				static char s_aVoteCommand[512] = {0};
				Extended.HSplitTop(20.0f, &Bottom, &Extended);
				Bottom.VSplitRight(10.0f, &Bottom, 0);
				Bottom.VSplitRight(120.0f, &Bottom, &Button);
				static int s_AddVoteButton = 0;
				if(DoButton_Menu(&s_AddVoteButton, Localize("Add"), 0, &Button))
					if(s_aVoteDescription[0] != 0 && s_aVoteCommand[0] != 0)
						m_pClient->m_pVoting->AddvoteOption(s_aVoteDescription, s_aVoteCommand);

				Bottom.VSplitLeft(5.0f, 0, &Bottom);
				Bottom.VSplitLeft(250.0f, &Button, &Bottom);
				static float s_OffsetDesc = 0.0f;
				DoEditBox(&s_aVoteDescription, &Button, s_aVoteDescription, sizeof(s_aVoteDescription), 14.0f, &s_OffsetDesc, false, CUI::CORNER_ALL);

				Bottom.VMargin(20.0f, &Button);
				static float s_OffsetCmd = 0.0f;
				DoEditBox(&s_aVoteCommand, &Button, s_aVoteCommand, sizeof(s_aVoteCommand), 14.0f, &s_OffsetCmd, false, CUI::CORNER_ALL);
			}
		}
	}
}

//H-Client: Ghost
int CMenus::GhostlistFetchCallback(const char *pName, int IsDir, int StorageType, void *pUser)
{
	CMenus *pSelf = (CMenus *)pUser;
	int Length = str_length(pName);
	if((pName[0] == '.' && (pName[1] == 0 ||
		(pName[1] == '.' && pName[2] == 0))) ||
		(!IsDir && (Length < 4 || str_comp(pName+Length-4, ".gho"))))
		return 0;

	CGhost::CGhostHeader Header;
	if(!pSelf->m_pClient->m_pGhost->GetInfo(pName, &Header))
		return 0;

	CGhostItem Item;
	str_copy(Item.m_aFilename, pName, sizeof(Item.m_aFilename));
	str_copy(Item.m_aPlayer, Header.m_aOwner, sizeof(Item.m_aPlayer));
	Item.m_Time = Header.m_Time;
	Item.m_Active = false;
	Item.m_ID = pSelf->m_lGhosts.add(Item);

	return 0;
}

void CMenus::GhostlistPopulate()
{
	m_OwnGhost = 0;
	m_lGhosts.clear();
	Storage()->ListDirectory(IStorageTW::TYPE_ALL, "ghosts", GhostlistFetchCallback, this);

	for(int i = 0; i < m_lGhosts.size(); i++)
	{
		if(str_comp(m_lGhosts[i].m_aPlayer, g_Config.m_PlayerName) == 0 && (!m_OwnGhost || m_lGhosts[i] < *m_OwnGhost))
			m_OwnGhost = &m_lGhosts[i];
	}

	if(m_OwnGhost)
	{
		m_OwnGhost->m_ID = -1;
		m_OwnGhost->m_Active = true;
		m_pClient->m_pGhost->Load(m_OwnGhost->m_aFilename, -1);
	}
}

void CMenus::RenderGhost(CUIRect MainView)
{
	// render background
	MainView.HSplitTop(10.0f, 0, &MainView);
	RenderTools()->DrawUIRect(&MainView, ms_ColorTabbarActive, CUI::CORNER_ALL, 10.0f);

	MainView.Margin(10.0f, &MainView);

	CUIRect Headers, Status;
	CUIRect View = MainView;

	View.HSplitTop(17.0f, &Headers, &View);
	View.HSplitBottom(28.0f, &View, &Status);

	// split of the scrollbar
	RenderTools()->DrawUIRect(&Headers, vec4(1,1,1,0.25f), CUI::CORNER_T, 5.0f);
	Headers.VSplitRight(20.0f, &Headers, 0);

	struct CColumn
	{
		int m_Id;
		CLocConstString m_Caption;
		float m_Width;
		CUIRect m_Rect;
		CUIRect m_Spacer;
	};

	enum
	{
		COL_ACTIVE=0,
		COL_NAME,
		COL_TIME,
	};

	static CColumn s_aCols[] = {
		{-1,			" ",		            2.0f,		{0}, {0}},
		{COL_ACTIVE,	" ",		            30.0f,		{0}, {0}},
		{COL_NAME,		Localize("Name"),		300.0f,		{0}, {0}},
		{COL_TIME,		Localize("Time"),		200.0f,		{0}, {0}},
	};

	int NumCols = sizeof(s_aCols)/sizeof(CColumn);

	// do layout
	for(int i = 0; i < NumCols; i++)
	{
		Headers.VSplitLeft(s_aCols[i].m_Width, &s_aCols[i].m_Rect, &Headers);

		if(i+1 < NumCols)
			Headers.VSplitLeft(2, &s_aCols[i].m_Spacer, &Headers);
	}

	// do headers
	for(int i = 0; i < NumCols; i++)
		DoButton_GridHeader(s_aCols[i].m_Caption, s_aCols[i].m_Caption, 0, &s_aCols[i].m_Rect);

	RenderTools()->DrawUIRect(&View, vec4(0,0,0,0.15f), 0, 0);

	CUIRect Scroll;
	View.VSplitRight(15, &View, &Scroll);

	int NumGhosts = m_lGhosts.size();

	int Num = (int)(View.h/s_aCols[0].m_Rect.h) + 1;
	static int s_ScrollBar = 0;
	static float s_ScrollValue = 0;

	Scroll.HMargin(5.0f, &Scroll);
	s_ScrollValue = DoScrollbarV(&s_ScrollBar, &Scroll, s_ScrollValue);

	int ScrollNum = NumGhosts-Num+1;
	if(ScrollNum > 0)
	{
		if(Input()->KeyPresses(KEY_MOUSE_WHEEL_UP))
			s_ScrollValue -= 1.0f/ScrollNum;
		if(Input()->KeyPresses(KEY_MOUSE_WHEEL_DOWN))
			s_ScrollValue += 1.0f/ScrollNum;
	}
	else
		ScrollNum = 0;

	static int s_SelectedIndex = 0;
	for(int i = 0; i < m_NumInputEvents; i++)
	{
		int NewIndex = -1;
		if(m_aInputEvents[i].m_Flags&IInput::FLAG_PRESS)
		{
			if(m_aInputEvents[i].m_Key == KEY_DOWN) NewIndex = s_SelectedIndex + 1;
			if(m_aInputEvents[i].m_Key == KEY_UP) NewIndex = s_SelectedIndex - 1;
		}
		if(NewIndex > -1 && NewIndex < NumGhosts)
		{
			//scroll
			float IndexY = View.y - s_ScrollValue*ScrollNum*s_aCols[0].m_Rect.h + NewIndex*s_aCols[0].m_Rect.h;
			int Scroll = View.y > IndexY ? -1 : View.y+View.h < IndexY+s_aCols[0].m_Rect.h ? 1 : 0;
			if(Scroll)
			{
				if(Scroll < 0)
				{
					int NumScrolls = (View.y-IndexY+s_aCols[0].m_Rect.h-1.0f)/s_aCols[0].m_Rect.h;
					s_ScrollValue -= (1.0f/ScrollNum)*NumScrolls;
				}
				else
				{
					int NumScrolls = (IndexY+s_aCols[0].m_Rect.h-(View.y+View.h)+s_aCols[0].m_Rect.h-1.0f)/s_aCols[0].m_Rect.h;
					s_ScrollValue += (1.0f/ScrollNum)*NumScrolls;
				}
			}

			s_SelectedIndex = NewIndex;
		}
	}

	if(s_ScrollValue < 0) s_ScrollValue = 0;
	if(s_ScrollValue > 1) s_ScrollValue = 1;

	// set clipping
	UI()->ClipEnable(&View);

	CUIRect OriginalView = View;
	View.y -= s_ScrollValue*ScrollNum*s_aCols[0].m_Rect.h;

	int NewSelected = -1;

	for (int i = 0; i < NumGhosts; i++)
	{
		const CGhostItem *pItem = &m_lGhosts[i];
		CUIRect Row;
		CUIRect SelectHitBox;

		View.HSplitTop(17.0f, &Row, &View);
		SelectHitBox = Row;

		// make sure that only those in view can be selected
		if(Row.y+Row.h > OriginalView.y && Row.y < OriginalView.y+OriginalView.h)
		{
			if(i == s_SelectedIndex)
			{
				CUIRect r = Row;
				r.Margin(1.5f, &r);
				RenderTools()->DrawUIRect(&r, vec4(1,1,1,0.5f), 0, 4.0f);
			}

			// clip the selection
			if(SelectHitBox.y < OriginalView.y) // top
			{
				SelectHitBox.h -= OriginalView.y-SelectHitBox.y;
				SelectHitBox.y = OriginalView.y;
			}
			else if(SelectHitBox.y+SelectHitBox.h > OriginalView.y+OriginalView.h) // bottom
				SelectHitBox.h = OriginalView.y+OriginalView.h-SelectHitBox.y;

			if(UI()->DoButtonLogic(pItem, "", 0, &SelectHitBox))
			{
				NewSelected = i;
			}
		}

		for(int c = 0; c < NumCols; c++)
		{
			CUIRect Button;
			Button.x = s_aCols[c].m_Rect.x;
			Button.y = Row.y;
			Button.h = Row.h;
			Button.w = s_aCols[c].m_Rect.w;

			int Id = s_aCols[c].m_Id;

			if(Id == COL_ACTIVE)
			{
				if(pItem->m_Active)
				{
					Graphics()->TextureSet(g_pData->m_aImages[IMAGE_EMOTICONS].m_Id);
					Graphics()->QuadsBegin();
					RenderTools()->SelectSprite(SPRITE_OOP + 7);
					IGraphics::CQuadItem QuadItem(Button.x+Button.w/2, Button.y+Button.h/2, 20.0f, 20.0f);
					Graphics()->QuadsDraw(&QuadItem, 1);

					Graphics()->QuadsEnd();
				}
			}
			else if(Id == COL_NAME)
			{
				CTextCursor Cursor;
				TextRender()->SetCursor(&Cursor, Button.x, Button.y, 12.0f * UI()->Scale(), TEXTFLAG_RENDER|TEXTFLAG_STOP_AT_END);
				Cursor.m_LineWidth = Button.w;

				char aBuf[128];
				bool Own = m_OwnGhost && pItem == m_OwnGhost;
				str_format(aBuf, sizeof(aBuf), "%s%s", pItem->m_aPlayer, Own?" (own)":"");
				TextRender()->TextEx(&Cursor, aBuf, -1);
			}
			else if(Id == COL_TIME)
			{
				CTextCursor Cursor;
				TextRender()->SetCursor(&Cursor, Button.x, Button.y, 12.0f * UI()->Scale(), TEXTFLAG_RENDER|TEXTFLAG_STOP_AT_END);
				Cursor.m_LineWidth = Button.w;

				char aBuf[64];
				str_format(aBuf, sizeof(aBuf), "%02d:%06.3f", (int)pItem->m_Time/60, pItem->m_Time-((int)pItem->m_Time/60*60));
				TextRender()->TextEx(&Cursor, aBuf, -1);
			}
		}
	}

	if(NewSelected != -1)
		s_SelectedIndex = NewSelected;

	CGhostItem *pGhost = &m_lGhosts[s_SelectedIndex];

	UI()->ClipDisable();

	RenderTools()->DrawUIRect(&Status, vec4(1,1,1,0.25f), CUI::CORNER_B, 5.0f);
	Status.Margin(5.0f, &Status);

	CUIRect Button;
	Status.VSplitRight(120.0f, &Status, &Button);

	static int s_GhostButton = 0;
	const char *pText = pGhost->m_Active ? "Deactivate" : "Activate";

	if(DoButton_Menu(&s_GhostButton, Localize(pText), 0, &Button) || (NewSelected != -1 && Input()->MouseDoubleClick()))
	{
		if(pGhost->m_Active)
			m_pClient->m_pGhost->Unload(pGhost->m_ID);
		else
			m_pClient->m_pGhost->Load(pGhost->m_aFilename, pGhost->m_ID);
		pGhost->m_Active ^= 1;
	}
}
