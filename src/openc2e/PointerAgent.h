/*
 *  PointerAgent.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Aug 30 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#pragma once

#include "SimpleAgent.h"
#include "common/backend/BackendEvent.h"

class DullPart;

class PointerAgent : public SimpleAgent {
  private:
	DullPart* overlay;
	int overlayTimer;

  protected:
	int hotspotx, hotspoty;

	AgentRef agent_under_pointer;
	unsigned int part_under_pointer;

	void carry(AgentRef);
	void drop(AgentRef);

  public:
	std::string name;
	bool handle_events;

	int holdingWire; // 0 for no, 1 for from output port, 2 for from input port
	AgentRef wireOriginAgent;
	unsigned int wireOriginID;

	AgentRef editAgent;

	PointerAgent(std::string spritefile);
	void finishInit();
	void firePointerScript(unsigned short event, Agent* src);
	void physicsTick();
	void tick();
	void kill();
	void handleEvent(BackendEvent& event);
	void setHotspot(int, int);

	int pointerX() { return x + hotspotx; }
	int pointerY() { return y + hotspoty; }
};

/* vim: set noet: */
