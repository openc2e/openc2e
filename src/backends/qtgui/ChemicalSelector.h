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
		
		bool chemselected[255];

	signals:
		void onSelectionChange(unsigned int chemno);

	private slots:
		void onGroupChange();
		void onChemChange(QListWidgetItem *item);

	public:
		ChemicalSelector(class CreatureGrapher *p);
		bool chemSelected(unsigned char chemno) { return chemselected[chemno]; }
};

#endif /* CHEMICALSELECTOR_H */

