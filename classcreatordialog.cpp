/*
    QSchoolManager
    Copyright © 2011 Pavel Zinin <pzinin@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/


#include "classcreatordialog.h"
#include "ui_classcreatordialog.h"

ClassCreatorDialog::ClassCreatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClassCreatorDialog), editable(false)
{
    ui->setupUi(this);
    QSettings s ("Pashazz", "QSchoolManager");
     possibleClassnames = s.value("Settings/PossibleClassNames").toStringList();
     _db =  QSqlDatabase::database("schmgr");
     //заполним номера классов
     int min = s.value("Settings/LowestLevel").toInt();
     if (min == 0) {min = 1;}
     int max = s.value("Settings/HighestLevel").toInt();
     for (int i = min; i <= max; i++ )
     {
         ui->cbYear->addItem(QVariant(i).toString());
     }
     appendPossibleClassnames(QVariant(min).toString());
     connect(ui->cbYear, SIGNAL(activated(QString)), this, SLOT(appendPossibleClassnames(QString)));
     setWindowTitle("Создать класс");
}


void ClassCreatorDialog::write()
{

        QSqlQuery q (_db);
    if (editable)
    {
        q.prepare("UPDATE Grades SET num=:num, letter=:letter, description=:description WHERE id=:id");
      q.bindValue(":id", _id);
    }
    else
        q.prepare("INSERT INTO Grades (num, letter, description) VALUES (:num, :letter, :description)");
    q.bindValue(":num", ui->cbYear->currentText());
    q.bindValue(":letter", ui->cbLetter->currentText());
    q.bindValue(":description" ,ui->txtDesc->toPlainText());
    if (!q.exec())
    {
       QMessageBox::critical(this, tr("Ошибка"), tr ("Не могу отдать класс: %1, запрос %2").arg(q.lastError().text(), q.lastQuery()));
        return;
    }
    else
    {
        if (!editable)
    {
        //получаем ID вновь созданного класса
        q.prepare("SELECT id FROM Grades ORDER BY id DESC");
        if (!q.exec())
        {
            qDebug() << "cannot get ID!! " << q.lastError().text();
            return;
        }
        q.first();
        _id = q.value(0).toInt();
        qDebug() << "ID got:" << _id;

        //создаем таблицу журнала
        q.prepare(tr("CREATE TABLE Journal%1 (id INTEGER PRIMARY KEY, subject INTEGER, date TEXT, mark INTEGER, pupil INTEGER)").arg(_id));
        if (!q.exec())
        {
            QMessageBox::critical(this, "err", tr ("Не могу отдать классный журнал: %1, query %2").arg(q.lastError().text(), q.lastQuery()));
            return;
        }
    }
        else if (!_db.tables().contains(tr("Journal%1").arg(_id)))
        {
            qDebug() << "Creating Journal for " << _id << " hmmm";
            q.prepare(tr("CREATE TABLE Journal%1 (id INTEGER PRIMARY KEY, subject INTEGER, date  TEXT, mark INTEGER, pupil INTEGER)").arg(_id));
            if (!q.exec())
            {
                QMessageBox::critical(this, tr("Ошибка"), tr ("Не могу отдать классный журнал: %1").arg(q.lastError().text()));
                return;
            }
        }


}
}
void ClassCreatorDialog::setEditableMode(const int classId)
{
    _id = classId;
    editable = true;

    QSqlQuery q(_db);
     q.prepare("SELECT num, letter, description FROM Grades WHERE id=:id");
     q.bindValue(":id", _id);
     if (!q.exec())
     {
         qDebug() << "cannot get ID!! " << q.lastError().text();
         return;
     }
     q.first();
     ui->txtDesc->setPlainText(q.value(2).toString());
     appendPossibleClassnames(q.value(0).toString());
     ui->cbYear->setCurrentIndex(q.value(0).toInt() -1);
     QString letter  = q.value(1).toString();
     for (int i = 0; i<= ui->cbLetter->count() - 1; i++)
     {
         ui->cbLetter->setCurrentIndex(i);
         if (ui->cbLetter->currentText() == letter)
             break;
     }
         this->setWindowTitle(tr("Редактирование класса %1 \"%2\"").arg(q.value(0).toString(), letter));
}

void ClassCreatorDialog::appendPossibleClassnames(const QString &num)
{ //дабы не возникало дубликатов
    QSqlQuery q (_db);
  QStringList myNames = possibleClassnames;
  q.prepare("SELECT letter FROM Grades WHERE num=:num");
  q.bindValue(":num", num);
  if (!q.exec())
 {
      qDebug() << "Не могу сформировать список классов!"  << num << q.lastError().text();
    return;
  }
  while (q.next())
      myNames.removeOne(q.value(0).toString());

  if (editable)
  {//добавляем нашу букву
      q.prepare("SELECT letter FROM Grades WHERE id=:num");
      q.bindValue(":num", _id);
      if (!q.exec())
     {
          qDebug() << "Не могу сформировать список классов!"  << num << q.lastError().text();
        return;
      }
      if(!q.first())
          return;
      myNames.append(q.value(0).toString());
  }
  myNames.sort();
  ui->cbLetter->clear();
  ui->cbLetter->addItems(myNames);

}

ClassCreatorDialog::~ClassCreatorDialog()
{
    delete ui;
}


void ClassCreatorDialog::on_btnActions_accepted()
{
    qDebug() << "Writing!";
    write();
    accept();
}

void ClassCreatorDialog::on_btnActions_rejected()
{
    if (QMessageBox::warning(this, tr("QSchoolManager"),tr("Действительно отменить?"), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
    {
     reject();
    }
}

QString ClassCreatorDialog::getTitleData()
{//for editable mode :)
    QString labelTitle = tr("%1 \"%2\" класс").arg(ui->cbYear->currentText(), ui->cbLetter->currentText());
    QString dsc = ui->txtDesc->toPlainText();
    if (!dsc.isEmpty())
        labelTitle.append(tr(" ‒ %1").arg(dsc));
    return labelTitle;
}
