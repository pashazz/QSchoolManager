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


#include "movedialog.h"
#include "ui_movedialog.h"

MoveDialog::MoveDialog(QWidget *parent, const QString& curYear, const QString &curLetter) :
    QDialog(parent),
    ui(new Ui::MoveDialog), cy(curYear), cl(curLetter), _db(QSqlDatabase::database("schmgr"))
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog);
    fillBox();
    for (int i = 0; i<= ui->cbYear->count() -1; i++)
    {
        ui->cbYear->setCurrentIndex(i);
        if (ui->cbYear->currentText() == cy)
            break;
    }
    fillLetter(ui->cbYear->currentText());
}

MoveDialog::~MoveDialog()
{
    delete ui;
}

void MoveDialog::fillBox()
{
    QSqlQuery q(_db);
    q.prepare("SELECT num FROM Grades");

    if(!q.exec())
        qDebug() << "Failed to fill yearBox" << q.lastError().text();
    QStringList years;
    while (q.next())
        years.append(q.value(0).toString());

    years.removeDuplicates();
    years.sort();
    ui->cbYear->addItems(years);
}

void MoveDialog::fillLetter(const QString& year)
{
    QSqlQuery q(_db);
    q.prepare("SELECT letter FROM Grades WHERE num=:yr");
    q.bindValue(":yr", year);

    if(!q.exec())
        qDebug() << "Failed to fill letterBox" << q.lastError().text();
    QStringList myNames;
    while (q.next())
        myNames.append(q.value(0).toString());

    myNames.removeDuplicates();
    if (year == cy)
        myNames.removeOne(cl);
    myNames.sort();
    ui->cbLetter->clear();
    ui->cbLetter->addItems(myNames);
}

int MoveDialog::year()
{
    return ui->cbYear->currentText().toInt();
}

QString MoveDialog::letter()
{
    return ui->cbLetter->currentText();
}

void MoveDialog::on_cbYear_currentIndexChanged(const QString &arg1)
{
    fillLetter(arg1);
}
