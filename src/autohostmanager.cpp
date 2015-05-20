/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/log.h>

#include "autohostmanager.h"

#include "ibattle.h"
#include "user.h"
#include "gui/mainwindow.h"
#include "utils/conversion.h"

AutohostHandler::AutohostHandler()
    : m_battle(0)
{
}

AutohostHandler::~AutohostHandler()
{
}

void AutohostHandler::Send(const std::string& cmd)
{
	m_battle->Say(cmd);
}

void AutohostHandler::SayFounder(const std::string& cmd)
{
	m_battle->GetFounder().Say(cmd);
}

//========================
//-------- Springie ------
//========================

SpringieHandler::SpringieHandler()
    : AutohostHandler()
{
}

SpringieHandler::~SpringieHandler()
{
}

void SpringieHandler::Balance()
{
	Send("!balance");
}

void SpringieHandler::SetRandomMap()
{
	Send("!map");
}

void SpringieHandler::SetMap(const std::string& map)
{
	Send("!map " + map);
}

void SpringieHandler::ClearStartBoxes()
{
	Send("!clear"); /// will check
}

void SpringieHandler::AddStartBox(int posx, int posy, int w, int h)
{
	Send(stdprintf("!addbox %i %i %i %i", posx, posy, w, h));
}

void SpringieHandler::Notify()
{
	Send("!notify");
}

void SpringieHandler::Start()
{
	Send("!start");
}

//------------------------------

//========================
//-------- Spads ---------
//========================
SpadsHandler::SpadsHandler()
    : AutohostHandler()
{
}

SpadsHandler::~SpadsHandler()
{
}

void SpadsHandler::Balance()
{
	Send("!balance");
}

void SpadsHandler::SetRandomMap()
{
	Send("!map 1"); //not so random
}

void SpadsHandler::SetMap(const std::string& map)
{
	Send("!map " + map);
}

void SpadsHandler::ClearStartBoxes()
{
}

void SpadsHandler::AddStartBox(int /*posx*/, int /*posy*/, int /*w*/, int /*h*/)
{
}

void SpadsHandler::Notify()
{
	Send("!notify");
}

void SpadsHandler::Start()
{
	Send("!start");
}

RelayHandler::RelayHandler()
{
}

RelayHandler::~RelayHandler()
{
}

void RelayHandler::Start()
{
	SayFounder("!stargame");
}

//-------------


AutohostManager::AutohostManager()
    : m_type(AUTOHOSTTYPE_NONE)
    , m_battle(0)
{
}

AutohostManager::~AutohostManager()
{
}

void AutohostManager::SetBattle(IBattle* battle)
{
	m_type = AutohostManager::AUTOHOSTTYPE_NONE;
	m_battle = battle;
}

AutohostHandler& AutohostManager::GetAutohostHandler()
{
	switch (m_type) {
		case AUTOHOSTTYPE_SPRINGIE:
			return m_springie;
		case AUTOHOSTTYPE_SPADS:
			return m_spads;
		case AUTOHOSTTYPE_RELAYHOST:
			return m_relayhandler;
		case AUTOHOSTTYPE_NONE:
			if (!m_battle->GetPresetList().empty()) { //FIXME: relayhost should set hosttype!
				return m_relayhandler;
			}
		case AUTOHOSTTYPE_UNKNOWN:
			return m_emptyhandler;
	}
	return m_emptyhandler;
}

bool AutohostManager::RecognizeAutohost(const std::string& type)
{
	if (type == "SPRINGIE") {
		m_type = AutohostManager::AUTOHOSTTYPE_SPRINGIE;
		return true;
	}
	if (type == "SPADS") {
		m_type = AutohostManager::AUTOHOSTTYPE_SPADS;
		return true;
	}
	if (type == "RELAY") {
		m_type = AutohostManager::AUTOHOSTTYPE_RELAYHOST;
		return true;
	}

	wxLogMessage(_T("Unknown autohost: %s"), type.c_str());
	m_type = AutohostManager::AUTOHOSTTYPE_UNKNOWN;
	return false;
}

AutohostManager::AutohostType AutohostManager::GetAutohostType()
{
	return m_type;
}
