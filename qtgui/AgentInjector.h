/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef AGENTINJECTOR_H
#define AGENTINJECTOR_H 1

#include "ui_agents.h"

class AgentInjector : public QDialog {
	Q_OBJECT
	
	public:
		AgentInjector(QWidget *parent);
		~AgentInjector();

	private slots:
		void onInject();
		void onSelect(QListWidgetItem *current, QListWidgetItem *prev);
		void onRemove();

	private:
		Ui::AgentInjectorForm ui;
		std::vector<class cobFile *> cobfiles;

		void readAgents();
		void resetAgents();
};


#endif /* AGENTINJECTOR_H */
