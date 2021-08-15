#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSettings>
#include <QMenu>
#include <QAction>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
		Q_OBJECT

	public:
		Widget(QWidget *parent = 0);
		~Widget();

	private slots:
		void on_label_linkActivated(const QString &link);

		void on_toolButton_clicked();

		void on_directoryComboBox_editTextChanged(const QString &arg1);

		void on_buttonBox_accepted();

		void on_buttonBox_rejected();

		void on_fileName_tb_clicked();

		void on_find_v8unpack();

		void on_toolButton_2_clicked();

		void on_resetSettings_tb_clicked();

		void on_fileName_cb_currentTextChanged(const QString &arg1);

		void on_fileName_cb_currentIndexChanged(int index);

		void on_kill_item_file_tb_clicked();

		void on_kill_item_folder_tb_clicked();
		void on_show_path_to_me();

	private:
		Ui::Widget *ui;
		QMenu *m_serviseMenu;
		QAction *m_actFoundGU;
		QAction *m_actKillFileItem;
		QAction *m_actKillFolderItem;
		QAction *m_actCliarSettings;
		QAction *m_actWhereIAm;
		QStringList m_listArgs;
		QStringList m_list_dirs;
		QString m_fileNameBaseName; // without extens
		QString m_fileNameFolder; // without extens
		QString m_fileNameExt; // without extens
		QString m_folder_target;
		QString getFolderFromFile(QString& src);
		QString getRelationFolder(QString& folder);
		bool unpackFile(QString& src);
		void updateFolderCB();
		void updateFolderTarget();
		void showStatus(QString stat);
		QMap<QString, QString> m_foldersMap;
		void settingsLoad();
		void settingsSave();
		QString getDelim(QString& src);
		void clearFiles(QString dirertory);
		QStringList m_clearDirs;

		QString selectedFileName();
		void setSelectedFileName(QString f_name);
		QStringList parseFileName(QString fName);


};
#endif // WIDGET_H
