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
	m_listArgs = qApp->arguments();
	ui->directoryComboBox->blockSignals(true);
	settingsLoad();
	ui->directoryComboBox->blockSignals(false);
	if (m_listArgs.count() > 1) {
		setSelectedFileName(m_listArgs.at(1)); //		ui->fileName_le->setText(m_list.at(1));
		updateFolderCB();
	}
	showStatus("Run...");
	m_serviseMenu = new QMenu(this);
	ui->servise_pb->setMenu(m_serviseMenu);

	m_actFoundGU = new QAction(QString::fromUtf8("Найти v8unpack и gcomp"),this);
	m_serviseMenu->addAction(m_actFoundGU);
	connect(m_actFoundGU,SIGNAL(triggered()),this,SLOT(on_find_v8unpack()));
	m_serviseMenu->addSeparator();

	m_actKillFileItem = new QAction(QString::fromUtf8("Удалить из списка текущий файл"),this);	m_serviseMenu->addAction(m_actKillFileItem);
	connect(m_actKillFileItem,SIGNAL(triggered()),this,SLOT(on_kill_item_file_tb_clicked()));

	m_actKillFolderItem = new QAction(QString::fromUtf8("Удалить из списка ТЕКУЩУЮ ДИРЕКТОРИЮ"),this);	m_serviseMenu->addAction(m_actKillFolderItem);
	connect(m_actKillFolderItem,SIGNAL(triggered()),this,SLOT(on_kill_item_folder_tb_clicked()));
	m_serviseMenu->addSeparator();

	m_actCliarSettings = new QAction(QString::fromUtf8("Очистить настройки"),this);
	m_serviseMenu->addAction(m_actCliarSettings);
	connect(m_actCliarSettings,SIGNAL(triggered()),this,SLOT(on_resetSettings_tb_clicked()));

//	m_serviseMenu->addSeparator();
	m_actWhereIAm = new QAction(QString::fromUtf8("КаковЪ путь?"),this);
	m_serviseMenu->addAction(m_actWhereIAm);
	connect(m_actWhereIAm,SIGNAL(triggered()),this,SLOT(on_show_path_to_me()));
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

	m_list_dirs = set.value("v8UnpackQ4/dirs_list_map").toStringList();
	QStringList total_folders = set.value("v8UnpackQ4/folders_save").toStringList();
	QStringList vals_sub;
	QString val;
	QString vFolderPs = getFolderFromFile(selectedFileName()/*ui->fileName_le->text()*/);

	QString vDir = ui->directoryComboBox->currentText();
	ui->directoryComboBox->clear();

	if (!vDir.isEmpty()) {
		if (ui->directoryComboBox->findText(vDir) == -1) {
			ui->directoryComboBox->addItem(vDir);
		}
	}
	foreach (val, total_folders) {
		if (ui->directoryComboBox->findText(val) == -1) {
			ui->directoryComboBox->addItem(val);
		}
	}
	total_folders = set.value("v8UnpackQ4/list_files").toStringList();
	ui->fileName_cb->blockSignals(true);
	foreach (val, total_folders) {
		if (!val.isEmpty()) {
			if (ui->fileName_cb->findText(val) == -1) {
				ui->fileName_cb->addItem(val);
			}
		}
	}
	ui->fileName_cb->insertItem(0,"");
	ui->fileName_cb->setCurrentIndex(0);
	ui->fileName_cb->blockSignals(false);
}

void Widget::settingsSave(){
	QSettings set(gMyName,gProgName);
	QStringList dirs_map, dirs_map_old = set.value("v8UnpackQ4/dirs_list_map").toStringList();
	QStringList total_folders = set.value("v8UnpackQ4/folders_save").toStringList();

	QString vFolderPs, vFolderPs_c = getFolderFromFile(selectedFileName()/*ui->fileName_le->text()*/);
	vFolderPs = vFolderPs_c;
	QString vFileSep = getDelim(vFolderPs);
	QString vDestFolder = ui->directoryComboBox->currentText();
	QString val, vDestFolder_c = vDestFolder;
	if (dirs_map_old.count() > 0) {
		// Чистим от сарых пар..
		vFolderPs_c += "	";
		foreach (val, dirs_map_old) {
			if (!val.contains(vFolderPs_c)) {
				dirs_map.append(val);
			}
		}
	}
	if (vFolderPs.length() > 2) {
		if (vFolderPs.at(vFolderPs.count()-1) != vFileSep.at(vFileSep.count()-1)) {
			vFolderPs += vFileSep;
		}
		vFolderPs += "	";
		vFolderPs += vDestFolder;
		if (dirs_map.indexOf(vFolderPs) == -1) {
			dirs_map.insert(0,vFolderPs);
		}
	}
	set.setValue("v8UnpackQ4/dirs_list_map",dirs_map);
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

	set.setValue("v8UnpackQ4/folders_save",total_folders);
	set.setValue("v8UnpackQ4/forVCS",ui->forVCS_cb->isChecked());
	set.setValue("v8UnpackQ4/for1Cv77",ui->for1Cv77->isChecked());

	QStringList vals;
	for (int var = 0; var < ui->fileName_cb->count(); ++var) {
		val = ui->fileName_cb->itemText(var);
		if (val.isEmpty()) {
			continue;
		}
		if (!vals.contains(val)) {
			vals.push_back(val);
		}
		if (var >= 30) {
			break;
		}
	}
	set.setValue("v8UnpackQ4/list_files",vals);


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

QStringList Widget::parseFileName(QString fName){
	QStringList list;
	QString vFile = fName;
	m_fileNameBaseName = "";
	m_fileNameExt = "";
	m_fileNameFolder = "";
	if (!vFile.isEmpty()) {
		QString vSpl = getDelim(vFile);
		QStringList li = vFile.split(vSpl);
		vFile = "";
		for (int var = 0; var < li.count()-1; ++var) {
			vFile += li.at(var);
			vFile += vSpl;
		}
		m_fileNameBaseName = li.at(li.count()-1);
		li = m_fileNameBaseName.split(".");
		m_fileNameBaseName = "";
		for (int var = 0; var < li.count()-1; ++var) {
			if (!m_fileNameBaseName.isEmpty()) {
				m_fileNameBaseName += ".";
			}
			m_fileNameBaseName += li.at(var);
		}
		m_fileNameFolder = vFile;
		m_fileNameExt = li.back();

	}

	return list;
}
//label_folder_target
void Widget::updateFolderCB(){
	QString vFile = selectedFileName()/*ui->fileName_le->text()*/;
	parseFileName(vFile);
	if (!vFile.isEmpty() && !m_fileNameFolder.isEmpty()) {
		QString vRf = getRelationFolder(m_fileNameFolder);
		if (vRf.isEmpty()) {
			vRf = m_fileNameFolder;
		}
		int ft = ui->directoryComboBox->findText(vRf);
		if (ft == -1) {
			ui->directoryComboBox->addItem(vRf);
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
	vPath += m_fileNameBaseName;
	m_folder_target = vPath;
	ui->label_folder_target->setText(m_folder_target);
}

void Widget::showStatus(QString stat){
	//ui->label_status->setText(stat);
	QDateTime dt =  QDateTime::currentDateTime();
	QString vStat = QString("%1 %2").arg(dt.toString("hh:mm:ss")).arg(stat);
	ui->list_status->addItem(vStat);
	QListWidgetItem* item = ui->list_status->item(ui->list_status->count()-1);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
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

// меняем QLineEdit на QComboBox для сохранения истории.
QString Widget::selectedFileName(){
	return ui->fileName_cb->currentText();
//	return ui->fileName_le->text();
}

void Widget::setSelectedFileName(QString f_name){
	int vPos = 0;
	QStringList li;  li << f_name << "";
	QString fndStr;
	for (int var = 0; var < li.count(); ++var) {
		fndStr = li.at(var);
		if (ui->fileName_cb->count() > 0) {
			vPos = ui->fileName_cb->findText(fndStr);
			while (vPos != -1) {
				ui->fileName_cb->removeItem(vPos);
				vPos = ui->fileName_cb->findText(fndStr);
			}
		}
	}
	ui->fileName_cb->insertItem(0,f_name);
	ui->fileName_cb->setCurrentIndex(0);
	//ui->fileName_le->setText(f_name);
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
	QString vFilePath = selectedFileName()/*ui->fileName_le->text()*/;
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

	QDateTime vF1Lm, vF2Lm;
	for (int cntr = 0; cntr < files.count(); ++cntr) {
		fi = files[cntr];
		vFilePath_1 = fi.absoluteFilePath();
		showStatus(QString::fromUtf8("Анализ: ")+vFilePath_1);
		vFName = fi.fileName();
		vFilePath_2 = vFolderCopy + vFName;
		vFi.setFile(vFilePath_2);
		if (vFi.exists()) {
			// сравним время.
			vF2Lm = vFi.lastModified();
			vF1Lm = fi.lastModified();
			if (vF2Lm == vF1Lm) {
				int y = 2;
				showStatus(QString::fromUtf8("Файл не менялся... пропускаем"));
				continue;
			}
			QFile vFlObj(vFilePath_2);
			vFlObj.remove();
		}
		QFile::copy(vFilePath_1,vFilePath_2);
		unpackFile(vFilePath_2);

	}
	// надо удалить файлы: FileHeader,.data, .header если для VCS
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

QString Widget::getRelationFolder(QString &folder){
	QString vVV, retVal = ui->directoryComboBox->currentText();
	vVV = folder + "	";
	if (m_list_dirs.count() > 0) {
		QString val;
		foreach (val, m_list_dirs) {
			if (val.contains(vVV)) {
				QStringList li = val.split("	");
				if (li.size() == 2) {
					retVal = li.at(1);
				}
				break;
			}
		}
	}
	return retVal;
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
	QFileInfo fi(selectedFileName() /*ui->fileName_le->text()*/);
	QString folderName = fi.dir().path();
	QString vExtens = tr("1Cv8 (*.epf *.erf)");
	if (ui->for1Cv77->isChecked()) {
		vExtens = "1Cv77 (*.ert)";
	}
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),	folderName,	vExtens);
	if (!fileName.isEmpty()) {
		fileName = fileName.replace("/","\\");
		setSelectedFileName(fileName);
		/*ui->fileName_le->setText(fileName)*/;
		qDebug() << fileName << folderName;
		updateFolderCB();
	}
}

void Widget::on_find_v8unpack(){
	QSettings setting( "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", QSettings::NativeFormat );
	QString pathVal = setting.value("Path", "no-path").toString();
	if (!pathVal.contains(";")) {
		return;
	}
	QStringList liPath = pathVal.split(";");
	QString val, val_, dirSep, vFilePath;
	foreach (val, liPath) {
		if (!val.contains("%")) {
			dirSep = getDelim(val);
			if (val.mid(val.count()-1,1) != dirSep) {
				val += dirSep;
			}
			vFilePath = val + "v8unpack.exe";
			if (QFile::exists(vFilePath)) {
				val_ = "v8unpack.exe - found in: "+vFilePath;
				showStatus(val_);
			}

			vFilePath = val + "gcomp.exe";
			if (QFile::exists(vFilePath)) {
				val_ = "gcomp.exe - found in: "+vFilePath;
				showStatus(val_);
			}

			//showStatus(val);
		}
	}
	//qDebug() << pathVal;
}

void Widget::on_toolButton_2_clicked()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(ui->directoryComboBox->currentText()));
}

void Widget::on_resetSettings_tb_clicked()
{
	QMessageBox msgBox;
	msgBox.setText(QString::fromUtf8("Настройки будут очищены!"));
	msgBox.setInformativeText(QString::fromUtf8("Передумали?"));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	if (ret == QMessageBox::No) {
		QSettings set(gMyName,gProgName);
	ui->directoryComboBox->clear();
	ui->fileName_cb->clear();
	set.setValue("v8UnpackQ4/folders_save",QStringList());
		set.setValue("v8UnpackQ4/dirs_list_map",QStringList());
	set.setValue("v8UnpackQ4/list_files",QStringList());
		settingsLoad();
	}

}

void Widget::on_fileName_cb_currentTextChanged(const QString &arg1)
{
	//setSelectedFileName(arg1);
	updateFolderCB();

}

void Widget::on_fileName_cb_currentIndexChanged(int index)
{
	//setSelectedFileName(arg1);
	updateFolderCB();

}

void Widget::on_kill_item_file_tb_clicked(){
	if (ui->fileName_cb->count() == 0) {
		return;
	}
	int vCurIndex = ui->fileName_cb->currentIndex();
	if (vCurIndex != -1) {
		ui->fileName_cb->removeItem(vCurIndex);
	}
}


void Widget::on_kill_item_folder_tb_clicked(){
	if (ui->directoryComboBox->count() == 0) {
		return;
	}
	int vCurIndex = ui->directoryComboBox->currentIndex();
	if (vCurIndex != -1) {
		ui->directoryComboBox->removeItem(vCurIndex);
	}


}

void Widget::on_show_path_to_me()
{
	QString vWhere = qApp->applicationFilePath();
	vWhere = vWhere.replace("/","\\");
	vWhere = QString::fromUtf8("Таков путь: ")+vWhere;
	showStatus(vWhere);

}
