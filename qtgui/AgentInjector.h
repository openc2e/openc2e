#ifndef AGENTINJECTOR_H
#define AGENTINJECTOR_H 1

#include "ui_agents.h"

class AgentInjector : public QDialog {
	Q_OBJECT
	
	public:
		AgentInjector(QWidget *parent = 0);
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
