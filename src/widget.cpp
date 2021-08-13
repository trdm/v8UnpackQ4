#include "widget.h"
#include "ui_widget.h"
#include "QDebug"
#include <QtGui>

static QString gMyName = "trdmval";
static QString gProgName = "v8UnpackQ4";
static QString gSettListName = "list_dirs";

Widget::Widget(QWidget *parent)
: QWidget(parent)
, ui(new Ui::Widget)
{
	ui->setupUi(this);
	m_list = qApp->arguments();
	if (m_list.count() > 1) {
		ui->fileName_le->setText(m_list.at(1));
	}	
	updateFolderCB();
	settingsLoad();
	showStatus("Run...");

}

Widget::~Widget()
{
	delete ui;
}
void Widget::settingsLoad()
{
	QSettings set(gMyName,gProgName);
	ui->forVCS_cb->setChecked(set.value("v8UnpackQ4/forVCS").toBool());
	ui->for1Cv77->setChecked(set.value("v8UnpackQ4/for1Cv77").toBool());
	QStringList vals = set.value("v8UnpackQ4/list_dirs").toStringList();
	QStringList total_folders = set.value("v8UnpackQ4/total_folders").toStringList();
	QStringList vals_sub;
	m_foldersMap.clear();
	QString vDir = ui->directoryComboBox->currentText();
	ui->directoryComboBox->clear();
	QString val;
	QString vFolderPs = getFolderFromFile(ui->fileName_le->text());
	//m_foldersSet.insert()
	if (!vals.isEmpty()) {
		foreach (val, vals) {
			vals_sub = val.split('\t');
			if (vals_sub.count() == 2) {
				try {
					m_foldersMap.insert(vals_sub.at(0),vals_sub.at(1));
				} catch (...) {
				}
				if (vals_sub.at(0) == vFolderPs) {
					ui->directoryComboBox->insertItem(0,vals_sub.at(1));
					break;
				}

			}
			//ui->directoryComboBox->addItem(val);
		}
	}
	ui->directoryComboBox->addItem(vDir);
	foreach (val, total_folders) {
		if (ui->directoryComboBox->findText(val) == -1) {
			ui->directoryComboBox->addItem(val);
		}
	}
}

void Widget::settingsSave(){
	QSettings set(gMyName,gProgName);
	QStringList vals = set.value("v8UnpackQ4/list_dirs").toStringList();
	QStringList total_folders = set.value("v8UnpackQ4/total_folders").toStringList();
	QString vFolderPs = getFolderFromFile(ui->fileName_le->text());
	QString vFileSep = getDelim(vFolderPs);
	if (vFolderPs.at(vFolderPs.count()-1) != vFileSep.at(vFileSep.count()-1)) {
		vFolderPs += vFileSep;
	}
	vFolderPs += "	";
	vFolderPs += ui->directoryComboBox->currentText();
	if (vals.indexOf(vFolderPs) == -1) {
		vals.append(vFolderPs);
	}
	QString val;
	for (int var = 0; var < ui->directoryComboBox->count(); ++var) {
		val = ui->directoryComboBox->itemText(var);
		if (val.isEmpty()) {
			continue;
		}
		if (!total_folders.contains(val)) {
			total_folders.push_back(val);
		}
		if (var >= 30) {
			break;
		}
	}
	//total_folders = set.value("v8UnpackQ4/total_folders"

	set.setValue("v8UnpackQ4/total_folders",total_folders);
	set.setValue("v8UnpackQ4/list_dirs",vals);
	set.setValue("v8UnpackQ4/forVCS",ui->forVCS_cb->isChecked());
	set.setValue("v8UnpackQ4/for1Cv77",ui->for1Cv77->isChecked());
}


void Widget::on_label_linkActivated(const QString &link)
{
	//qDebug() << link;
	QDesktopServices::openUrl(QUrl::fromLocalFile(link));
}

void Widget::on_toolButton_clicked()
{
	QString currentPath = QDir::currentPath(), currentT = ui->directoryComboBox->currentText();
	if (!currentT.isEmpty()) {
		currentPath = currentT;
	}
	//QDesktopServices::w
	QString directory = QFileDialog::getExistingDirectory(this,	tr("Find Files"), currentPath);

	if (!directory.isEmpty()) {
		if (ui->directoryComboBox->findText(directory) == -1)
			ui->directoryComboBox->addItem(directory);
		ui->directoryComboBox->setCurrentIndex(ui->directoryComboBox->findText(directory));
	}
	updateFolderTarget();
}

//label_folder_target
void Widget::updateFolderCB(){
	QString vFile = ui->fileName_le->text();

	if (!vFile.isEmpty()) {
		QString vSpl = getDelim(vFile);
		QStringList li = vFile.split(vSpl);
		vFile = "";
		for (int var = 0; var < li.count()-1; ++var) {
			vFile += li.at(var);
			vFile += vSpl;
		}
		m_fileNameWE = li.at(li.count()-1);
		li = m_fileNameWE.split(".");
		m_fileNameWE = "";
		for (int var = 0; var < li.count()-1; ++var) {
			if (!m_fileNameWE.isEmpty()) {
				m_fileNameWE += ".";
			}
			m_fileNameWE += li.at(var);
		}
		qDebug() << m_fileNameWE;
		int ft = ui->directoryComboBox->findText(vFile);
		if (ft == -1) {
			ui->directoryComboBox->addItem(vFile);
		} else {
			ui->directoryComboBox->setCurrentIndex(ft);
		}
		//qDebug() << vFile;
	}
	updateFolderTarget();


}

void Widget::updateFolderTarget(){
	QString vPath = ui->directoryComboBox->currentText();
	m_folder_target = "";
	ui->label_folder_target->setText(m_folder_target);
	if (vPath.isEmpty()) {
		return;
	}
	QString vSpl = getDelim(vPath);
	if (vPath.at(vPath.count()-1) != vSpl.at(0)) {
		vPath += vSpl;
	}
	vPath += m_fileNameWE;
	m_folder_target = vPath;
	ui->label_folder_target->setText(m_folder_target);
}

void Widget::showStatus(QString stat){
	ui->label_status->setText(stat);
}


QString Widget::getDelim(QString &src)
{
	QString vSpl = "\\";
	if (src.indexOf(vSpl) == -1) {
		vSpl = "/";
	}
	return vSpl;
}

void Widget::clearFiles(QString dirertory) {
	QStringList listExt;
	QString vExt, vFName;
	showStatus("scan "+dirertory);
	bool isit;
	listExt << "FileHeader" << ".data" << ".header";
	if (dirertory.endsWith(".unp")) {
		listExt.clear();
		listExt << "FileHeader" << ".header";
	}
	QDir vDir;
	vDir.setPath(dirertory);
	QFileInfo fi;
	QFileInfoList files = vDir.entryInfoList(QStringList("*"), QDir::Files | QDir::Dirs |  QDir::NoSymLinks |QDir::NoDotAndDotDot , QDir::DirsFirst | QDir::Name );
	qDebug() << "Find " << " by " << vExt << " total: " <<  files.count() <<  " in " << dirertory ;
	for (int cntr = 0; cntr < files.count(); ++cntr) {
		fi = files[cntr];
		isit = false;
		vFName = fi.fileName();
		foreach (vExt, listExt) {
			if (vFName.endsWith(vExt)) {
				isit = true;
				break;
			}
		}
		if (isit) {
			qDebug() << "remove: " << fi.filePath();
			showStatus("remove "+fi.filePath());
			QFile::remove(fi.filePath());
		}
		if (fi.isDir()) {
			clearFiles(fi.absoluteFilePath());
		}
	}

}

void Widget::on_directoryComboBox_editTextChanged(const QString &arg1)
{
	updateFolderTarget();
}

void Widget::on_buttonBox_accepted(){
	QString vTitle = "v8UnpackQ4";
	if (m_folder_target.isEmpty()) {
		QMessageBox::warning(this,vTitle,QString::fromUtf8("Целевой каталог не задан!"));
		return;
	}
	QString vFilePath = ui->fileName_le->text();
	if (vFilePath.isEmpty()) {
		QMessageBox::warning(this,vTitle,QString::fromUtf8("Файл не выбран!"));
		return;
	}
	m_clearDirs.clear();

	QFileInfo vFi(vFilePath);
	QFileInfo fi;
	QFileInfoList files;
	bool isDir = vFi.isDir();
	QStringList vFiles, vExtens1C;
	vExtens1C << "*.epf" << "*.erf";
	bool vFore77 = ui->for1Cv77->isChecked();
	if (vFore77) {
		vExtens1C.clear();
		vExtens1C << "*.ert";
	}
	if (vFi.isDir()) {
		QDir dir(vFilePath);
		files = dir.entryInfoList(vExtens1C , QDir::Files );
		if (files.isEmpty())
			return;
	} else {
		files.append(vFi);
	}
	QString vDelim = getDelim(m_folder_target);
	QStringList li = m_folder_target.split(vDelim);
	QDir vDir;
	if (! vDir.exists(m_folder_target)) {
		if (!vFore77) {
			if (!vDir.mkpath(m_folder_target)) {
				QMessageBox::warning(this,vTitle,QString::fromUtf8("Ошибка при создании каталога:")+m_folder_target);
				return;
			}
		}
	}
	settingsSave();
	QString vFName, vFolderTarget = m_folder_target, vFolderCopy = ui->directoryComboBox->currentText();
	if (vFolderTarget.at(vFolderTarget.count()-1) != vDelim.at(vDelim.count()-1)) {
		vFolderTarget += vDelim;
	}
	if (vFolderCopy.at(vFolderCopy.count()-1) != vDelim.at(vDelim.count()-1)) {
		vFolderCopy += vDelim;
	}
	QString vFilePath_2, vFilePath_1; // = vFolderCopy + "unpac.bat";
//	QFile vBatFile(vFilePath_1);
//	if (vBatFile.exists()) {
//		vBatFile.remove();
//	}

	QDateTime vF1Lm, vF2Lm;
	for (int cntr = 0; cntr < files.count(); ++cntr) {
		fi = files[cntr];
		vFilePath_1 = fi.absoluteFilePath();
		vFName = fi.fileName();
		vFilePath_2 = vFolderCopy + vFName;
		vFi.setFile(vFilePath_2);
		if (vFi.exists()) {
			// сравним время.
			vF2Lm = vFi.lastModified();
			vF1Lm = fi.lastModified();
			if (vF2Lm == vF1Lm) {
				int y = 2;
				continue;
			}
			QFile vFlObj(vFilePath_2);
			vFlObj.remove();
		}
		QFile::copy(vFilePath_1,vFilePath_2);
		unpackFile(vFilePath_2);

	}
	// надо удалить файлы: FileHeader,.data, .header если для VCS
	/*
	 * QProcess proc;
	proc.setWorkingDirectory(vFolderCopy);

	proc.start("unpac.bat");
	proc.waitForFinished();
	*/
	if (ui->forVCS_cb->isChecked()) {
		qDebug() << "clear...";
		foreach (vFName, m_clearDirs) {
			clearFiles(vFName);
		}
	}
	settingsSave();
	showStatus(QString::fromUtf8("Закончили..."));
}

void Widget::on_buttonBox_rejected()
{
	settingsSave();
	close();
}

QString Widget::getFolderFromFile(QString &src){
	QString vFile = src; //ui->fileName_le->text();
	QFileInfo vFileObj(vFile);
	//bool isFile = vFileObj.isFile();
	if (vFileObj.isDir()) {
		return vFile;
	}
	if (!vFile.isEmpty()) {
		QString vSpl = getDelim(vFile);
		QStringList li = vFile.split(vSpl);
		vFile = "";
		for (int var = 0; var < li.count()-1; ++var) {
			vFile += li.at(var);
			vFile += vSpl;
		}
	}
	return vFile;
}
QString addQts(QString src) {
	return "\""+src+"\"";
}

bool Widget::unpackFile(QString &src){
	QFileInfo vFi(src), fi;
	QString vFolderNm = vFi.dir().absolutePath(), vFileNm = vFi.fileName(), vFileNmBase = vFi.baseName();
	QString vFolderNm2 = vFolderNm + getDelim(vFolderNm)+vFileNmBase;
	QString vFilePath_1, vFName1, vFName2, vFName3;
	if (m_clearDirs.indexOf(vFolderNm2) == -1) {
		m_clearDirs.append(vFolderNm2);
	}


	showStatus(QString::fromUtf8("Разбираем: ")+src);
	QProcess proc;
	proc.setWorkingDirectory(vFolderNm);

	QString vProgramName = "V8UNPACK";
	QStringList params;
	params << "-U" << vFileNm << (vFileNmBase); 	//params << "-U" << addQts(vFileNm) << addQts(vFileNmBase);	//proc.execute("V8UNPACK", params);
	bool is77 = ui->for1Cv77->isChecked();
	if (is77) {
		vProgramName = "gcomp";
		params.clear();
		params << "-d" << "-F" << vFileNm << "-D" << "Reports";
		//	> gcomp -d -F TestReport.ert -D Reports
		//   Разберет содержимое TestReport.ert в каталог Reports\TestReport
	}


	proc.start(vProgramName, params);
	QProcess::ProcessState ps = proc.state();
	if (!proc.waitForFinished())
		return false;
	if (!is77) {

		//	vFilePath_1 = vFolderNm+getDelim(vFolderNm)+"unpac.bat";
		//	QFile vBatFile(vFilePath_1);
		//	if (vBatFile.exists()) {
		//		vBatFile.remove();
		//	}
		//	if (!vBatFile.open(QFile::WriteOnly /*| QFile::Truncate*/)) {
		//		return false;
		//	}
		//	QString vOutStr;
		//	QTextStream out(&vBatFile);
		//	out.setCodec("IBM 866");
		//	vOutStr = QString::fromUtf8("V8UNPACK -U ") + "\"" + vFileNm +  "\" \"" + vFileNmBase + "\"\n";
		//	out << vOutStr;

		QDir vFold2(vFolderNm2);
		QFileInfoList files = vFold2.entryInfoList(QStringList("*.data"), QDir::Files );
		if (files.isEmpty())
			return false;
		for (int cntr = 0; cntr < files.count(); ++cntr) {
			fi = files[cntr];
			vFilePath_1 = fi.absoluteFilePath();
			vFileNm = fi.fileName();
			//qDebug() << vFilePath_1;
			params.clear();
			vFName1 = vFileNmBase + "\\" + vFileNm;
			vFName2 = vFileNmBase + "\\" + vFileNm+".und";

			//params << "-undeflate" << addQts(vFName1) << addQts(vFName2) ;
			params << "-undeflate" << (vFName1) << (vFName2) ;
			proc.start("V8UNPACK", params);		//		proc.execute("V8UNPACK", params);
			if (!proc.waitForFinished())
				return false;

	//		vOutStr = QString::fromUtf8("V8UNPACK -undeflate ") + "\"" + vFName1 +  "\" \"" + vFName2 + "\"\n";		out << vOutStr;

			params.clear();
			vFName2 = vFileNmBase + "\\" + vFileNm + ".und"; // а этот удалить надо потом.
			vFName3 = vFileNmBase + "\\" + vFileNm + ".unp";
			params << "-unpack" << (vFName2) << (vFName3);

			proc.start("V8UNPACK", params); //		proc.execute("V8UNPACK", params);
			if (!proc.waitForFinished())
				return false;

		//		vOutStr = QString::fromUtf8("V8UNPACK -unpack ") + "\"" + vFName2 +  "\" \"" + vFName3 + "\"\n"; 		out << vOutStr;

		}
	}

	proc.close();
//	vBatFile.flush();
//	vBatFile.close();
	showStatus(QString::fromUtf8("Разобрали: ")+src);

//	proc.
//	V8UNPACK -U ВыгрузкаВ_УСН.epf ВыгрузкаВ_УСН
//	V8UNPACK -undeflate ВыгрузкаВ_УСН\6117cbc8-c149-403b-bd48-f95b22e0a49b.0.data ВыгрузкаВ_УСН\6117cbc8-c149-403b-bd48-f95b22e0a49b.0.data.und
//	V8UNPACK -unpack ВыгрузкаВ_УСН\6117cbc8-c149-403b-bd48-f95b22e0a49b.0.data.und ВыгрузкаВ_УСН\6117cbc8-c149-403b-bd48-f95b22e0a49b.0.data.unp


	return true;
}

void Widget::on_fileName_tb_clicked()
{
	QFileInfo fi(ui->fileName_le->text());
	QString folderName = fi.dir().path();
	QString vExtens = tr("1Cv8 (*.epf *.erf)");
	if (ui->for1Cv77->isChecked()) {
		vExtens = "1Cv77 (*.ert)";
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),	folderName,	vExtens);
	if (!fileName.isEmpty()) {
		fileName = fileName.replace("/","\\");
		ui->fileName_le->setText(fileName);
		qDebug() << fileName << folderName;
		updateFolderCB();
	}
}

void Widget::on_pushButton_clicked()
{

}

void Widget::on_toolButton_2_clicked()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(ui->directoryComboBox->currentText()));
}

void Widget::on_resetSettings_tb_clicked()
{
	QSettings set(gMyName,gProgName);
	set.setValue("v8UnpackQ4/total_folders",QStringList());
	set.setValue("v8UnpackQ4/list_dirs",QStringList());
	settingsLoad();
}
