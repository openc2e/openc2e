#ifndef CHEMICALSELECTOR_H
#define CHEMICALSELECTOR_H 1

#include <map>
#include <string>
#include <vector>
#include <QWidget>
#include <QListWidget>

class ChemicalSelector : public QWidget {
	Q_OBJECT

	protected:
		class CreatureGrapher *parent;
		std::map<unsigned int, std::string> chemnames;
		std::map<std::string, std::vector<unsigned int> > chemgroups;

		QListWidget *grouplist;
		QListWidget *chemlist;

	private slots:
		void onGroupChange();

	public:
		ChemicalSelector(class CreatureGrapher *p);
};

#endif /* CHEMICALSELECTOR_H */

