#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSettings>

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

		void on_pushButton_clicked();

		void on_toolButton_2_clicked();

		void on_resetSettings_tb_clicked();

	private:
		Ui::Widget *ui;
		QStringList m_list;
		QString m_fileNameWE; // without extens
		QString m_folder_target;
		QString getFolderFromFile(QString& src);
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


};
#endif // WIDGET_H
