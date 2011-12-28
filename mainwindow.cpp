/*
    QSchoolManager
    Copyright (C) 2011 Pavel Zinin <pzinin@gmail.com>

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


#include "mainwindow.h"
#include "ui_mainwindow.h"

//Dialogues
#include "classcreatordialog.h"
#include "pupildialog.h"
#include "movedialog.h"

#define THIS this

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), s (new QSettings("Pashazz", "QSchoolManager")), _currentId(-1)
{

    ui->setupUi(this);

    connect (ui->mnuQuit, SIGNAL(triggered()), this, SLOT(close())); //закрыть
    connect (ui->cbClass, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateClassesList(QString)));
    connect (ui->cmdCloseClass, SIGNAL(clicked()), this, SLOT(closeObject()));
    connect(ui->cmdCloseRemove, SIGNAL(clicked()), THIS, SLOT(removeObject()));
    connect (ui->lstPupils, SIGNAL(itemSelectionChanged()), SLOT (buttonCheck()));
    connect (ui->cmdOpenCard, SIGNAL(clicked()), THIS, SLOT(showInfo()));

    loadGeometry();
    ui->boxClass->setVisible(false);
    ui->lblTitle->setVisible(false);
    ui->lblTitle->setAlignment(Qt::AlignCenter);
    createClassesList();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete s;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e)
 writeGeometry();
}
void MainWindow::loadGeometry()
{
    restoreState(s->value("MainWindow/State").toByteArray());
    restoreGeometry(s->value("MainWindow/Geometry").toByteArray());

}
void MainWindow::writeGeometry()
{
    s->setValue("MainWindow/State", saveState());
    s->setValue("MainWindow/Geometry", saveGeometry());
}

void MainWindow::createClassesList()
{
    QSqlQuery q (QSqlDatabase::database("schmgr"));
    ui->cbClass->clear();
    q.prepare("SELECT num FROM Grades");
    if (!q.exec())
    {
        QMessageBox::critical(this, tr("Ошибка базы данных"), tr("Невозможно создать список классов: %1").arg(q.lastError().text()));
        return;
    }
    QStringList classDigits;
    while (q.next())
      classDigits.append(q.value(0).toString());

    classDigits.removeDuplicates();
    classDigits.sort();
    ui->cbClass->addItems(classDigits);
    updateClassesList(ui->cbClass->currentText());
}

void MainWindow::updateClassesList(const QString &n)
{
       QSqlQuery q (QSqlDatabase::database("schmgr"));
       q.prepare("SELECT letter, description, id FROM Grades WHERE num=:num ORDER BY letter");
       q.bindValue(":num", n);
       if (!q.exec())
       {
           QMessageBox::critical(this, tr("Ошибка базы данных"), tr("Невозможно создать список классов %2: %1").arg(q.lastError().text()).arg(n));
           return;
       }
    ui->cbLetter->clear(); //очищаем все буквы
       while (q.next())
       {
          QString letter = q.value(0).toString();
          QString desc = q.value(1).toString();
          QString id = q.value(2).toString();
          QString myText;
          if (desc.isEmpty())
              myText = tr("%1 \"%2\"").arg(n, letter);
          else
              myText = tr("%1 \"%2\" (%3)").arg(n, letter, desc);
          ui->cbLetter->addItem(myText, id);
       }
}

void MainWindow::on_cmdCreateClass_clicked()
{
    ClassCreatorDialog *dlg = new ClassCreatorDialog(this);
    if (dlg->exec() == QDialog::Accepted)
    {
        //open class byid
        createClassesList();
    }
}

void MainWindow::openObject(int id)
{ //открывает класс
    QSqlQuery q (QSqlDatabase::database("schmgr"));
    q.prepare("SELECT num, letter, description FROM Grades WHERE id=:id");
    q.bindValue(":id", id);
    if (!q.exec())
    {
        qDebug() << "OpenObject failed. Reason: " << q.lastError().text();
        return;
    }
    //so far we need to?
    if(!q.first())
    {
        qDebug() << "OpenObject: argument error ";
        return;
    }
    QString labelTitle = tr("%1 \"%2\" класс").arg(q.value(0).toString(), q.value(1).toString());
    QString dsc = q.value(2).toString();
    if (!dsc.isEmpty())
        labelTitle.append(tr(" ‒ %1").arg(dsc));
    ui->lblTitle->setText(labelTitle);
   //открываем фрейм
   ui->boxClass->setVisible(true);
   ui->lblTitle->setVisible(true);
   ui->taskBox->setVisible(false);
   _currentId = id;
   loadPupils();

}

void MainWindow::loadPupils()
{
    QSqlQuery q (QSqlDatabase::database("schmgr"));
    //загрузим список учеников в QListWidget, фильтрация не нужна (в окон. версии будет), и пр.
    q.prepare("SELECT id, surname, firstname, middlename FROM Pupils WHERE grade=:id");
    q.bindValue(":id", _currentId);
    if (!q.exec())
    {
        qDebug() << "LoadPupils: Pupil loading failed. Reason: " << q.lastError().text();
        return;
    }
    //so far we need to?
    ui->lstPupils->clear();
    while (q.next())
    {
        QListWidgetItem *it = new QListWidgetItem();
        QString pupilTitle = tr("%1 %2 %3").arg(q.value(1).toString(), q.value(2).toString(), q.value(3).toString());
        it->setText(pupilTitle);
        it->setData(Qt::UserRole, q.value(0));
        ui->lstPupils->addItem(it);
    }
    buttonCheck();
}

void MainWindow::on_cmdOpenClass_clicked()
{
    openObject(ui->cbLetter->itemData(ui->cbLetter->currentIndex()).toInt());
}

void MainWindow::on_cbLetter_currentIndexChanged(int index)
{
    ui->cmdOpenClass->setEnabled(index != -1);
}

void MainWindow::closeObject()
{
    int &id = _currentId;
    if (id == -1)
    {
        qDebug() << "CloseObject: incorrect call: no object opened for now";
        return;
    }
    ui->lblTitle->clear();
    ui->lblTitle->setVisible(false);
    ui->boxClass->setVisible(false);
    ui->lstPupils->clear();
    ui->taskBox->setVisible(true);
    id = -1;
}

void MainWindow::removeObject()
{
    int id = _currentId;
    if (id == -1)
    {
        qDebug() << "RemoveObject: incorrect call: no object opened for now";
        return;
    }
    if (QMessageBox::question(this, tr("Точно удалить?"), tr("Класс #%1 (%2) будет удален со всеми учениками!").arg(QVariant(id).toString(), ui->lblTitle->text()), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }
    closeObject();
    QSqlDatabase db = QSqlDatabase::database("schmgr");
    db.transaction();
    QSqlQuery q (db);
    q.prepare(tr("DROP TABLE Journal%1").arg(id));
    if (!q.exec())
    {
        db.rollback();
        qDebug() << "RemoveObject: can't drop table " << id << " : rollback...." << q.lastError().text();
    }
    q.prepare("DELETE FROM Pupils WHERE grade=:id");
    q.bindValue(":id", id);
    if (!q.exec())
    {
        db.rollback();
        qDebug() << "RemoveObject: can't delete pupils " << id << " : rollback...." << q.lastError().text();
    }
    q.prepare("DELETE FROM Grades WHERE id=:id");
    q.bindValue(":id", id);
    if (!q.exec())
    {
        db.rollback();
        qDebug() << "RemoveObject: can't delete Grade " << id << " : rollback...." << q.lastError().text();
    }
    db.commit();
    createClassesList();
}

void MainWindow::on_actOpenClass_triggered()
{
    if (_currentId != -1)
    {
        if (QMessageBox::question(this, tr("Класс уже открыт"), tr("Закрыть текущий класс?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
            return;
        else
        {
            closeObject();
        }


    }
    this->ui->tabWidget->setCurrentIndex(0);

}

void MainWindow::on_actCreateClass_triggered()
{
    if (_currentId != -1)
    {
        if (QMessageBox::question(this, tr("Класс уже открыт"), tr("Закрыть текущий класс?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
            return;
        else
        {
            closeObject();
        }
    }
    on_cmdCreateClass_clicked(); //the same
}

void MainWindow::on_lblTitle_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    ClassCreatorDialog *dlg = new ClassCreatorDialog(this);
    dlg->setEditableMode(_currentId);
    if (dlg->exec() == QDialog::Accepted)
    {
        //open class byid
        createClassesList();
    ui->lblTitle->setText(dlg->getTitleData());
    }
}

void MainWindow::on_cmdAdd_clicked()
{
    PupilDialog *dlg = new PupilDialog (this, ui->lblTitle->text());
    if (dlg->exec() == QDialog::Accepted)
    {
        QSqlDatabase db  = QSqlDatabase::database("schmgr");
        db.transaction();
        QSqlQuery q (db);
        q.prepare("INSERT INTO Pupils (firstname, middlename, surname, grade) VALUES (?, ?, ?, ?)");
        q.addBindValue(dlg->name());
        q.addBindValue(dlg->middleName());
        q.addBindValue(dlg->surName());
        q.addBindValue(_currentId);
        if (!q.exec())
        {
            qDebug() << "Can't add pupil, abort transaction";
            return;
        }
        db.commit();
        loadPupils();
    }


}

void MainWindow::on_cmdRemove_clicked()
{
    if (QMessageBox::warning(this, "Удаление", "Вы действительно хотите удалить выделенных учеников?", QMessageBox::YesAll, QMessageBox::NoAll) == QMessageBox::NoAll)
        return;
    foreach (QListWidgetItem *it, ui->lstPupils->selectedItems())
    {
        truncPupil(it->data(Qt::UserRole).toInt());
    }
}

void MainWindow::truncPupil(int id)
{
    QSqlDatabase db  = QSqlDatabase::database("schmgr");
    db.transaction();
    QSqlQuery q (db);
    q.prepare("DELETE FROM Pupils WHERE id=:id");
    q.bindValue(":id", id);
    if (!q.exec())
    {
        qDebug() << "Can't rm pupil, abort transaction";
        return;
    }
    db.commit();
    loadPupils();
}

void MainWindow::movePupil(int id, int to)
{
    QSqlDatabase db  = QSqlDatabase::database("schmgr");
    db.transaction();
    QSqlQuery q (db);
    q.prepare("UPDATE Pupils SET grade=? WHERE id=?");
    q.addBindValue(to);
    q.addBindValue(id);
    if (!q.exec())
    {
        qDebug() << "Can't move pupil, abort transaction... Reason: " << q.lastError().text();
        db.rollback();
        return;
    }
    db.commit();
    loadPupils();
}

void MainWindow::buttonCheck()
{
    bool enable = !ui->lstPupils->selectedItems().isEmpty();
    //включаем/отключаем кнопки действий со списком
    ui->cmdRemove->setEnabled(enable);
    ui->cmdMove->setEnabled(enable);

    ui->cmdOpenCard->setEnabled(ui->lstPupils->selectedItems().count() == 1);

}

void MainWindow::showInfo()
{
    int pupilId = ui->lstPupils->selectedItems().first()->data(Qt::UserRole).toInt();
    QSqlDatabase db  = QSqlDatabase::database("schmgr");
    QSqlQuery q (db);
    q.prepare ("SELECT firstname, middlename, surname FROM Pupils WHERE id=:id");
    q.bindValue(":id", pupilId);
    if (!q.exec())
    {
        qDebug() << "Cant get pupil info, abort";
        return;
    }
    q.first();
    PupilDialog *dlg = new PupilDialog(this, ui->lblTitle->text());
    dlg->setName(q.value(0).toString());
    dlg->setMiddlename(q.value(1).toString());
    dlg->setSurname(q.value(2).toString());
    if (dlg->exec() == QDialog::Accepted)
    {
        db.transaction();
        q.prepare("UPDATE Pupils SET firstname=?, middlename=?, surname=? WHERE id=?");
        q.addBindValue(dlg->name());
        q.addBindValue(dlg->middleName());
        q.addBindValue(dlg->surName());
        q.addBindValue(pupilId);
        if (!q.exec())
        {
            qDebug() << "Cant SET UP pupil info, abort";
            db.rollback();
            return;
        }
        db.commit();
        loadPupils();
    }
}

void MainWindow::on_lstPupils_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item)
    showInfo();
}


void MainWindow::on_cmdMove_clicked()
{
    QSqlQuery q(QSqlDatabase::database("schmgr"));
    q.prepare("SELECT num, letter FROM Grades WHERE id=:curid");
    q.bindValue(":curid", _currentId);
    if (!q.exec())
    {
        qDebug() << "Cant get class";
        return;
    }
        QString cy, cl;
        q.first();
        cy = q.value(0).toString();
        cl = q.value(1).toString();
        MoveDialog *dlg = new MoveDialog (this, cy, cl);
        if (dlg->exec() == QDialog::Accepted)
        {
            q.prepare("SELECT id FROM Grades WHERE num=? AND letter=?");
            q.addBindValue(dlg->year());
            q.addBindValue(dlg->letter());
            if (!q.exec())
            {
                qDebug() << "Cant get class" << q.lastError().text();
                return;
            }
            q.first();
            int x = q.value(0).toInt();

            foreach(QListWidgetItem *it, ui->lstPupils->selectedItems())
            {
                movePupil(it->data(Qt::UserRole).toInt(), x);
            }
            loadPupils();
        }

}
