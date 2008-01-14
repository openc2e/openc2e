#ifndef AGENTINJECTOR_H
#define AGENTINJECTOR_H 1

#include "ui_agents.h"

class AgentInjector : public QDialog {
	Q_OBJECT
	
	public:
		AgentInjector(QWidget *parent = 0);

	private slots:
		void onInject();
		/*void onRemove();*/
		void onSelect(QListWidgetItem *cur);

	private:
		Ui::AgentInjectorForm ui;

		void readAgents();
};


#endif /* AGENTINJECTOR_H */
