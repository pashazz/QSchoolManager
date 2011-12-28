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


#include "pupildialog.h"
#include "ui_pupildialog.h"

PupilDialog::PupilDialog(QWidget *parent, const QString &grade) :
    QDialog(parent),
    ui(new Ui::PupilDialog)
{
    ui->setupUi(this);
    ui->lblClass->setText(tr("Класс: %1").arg(grade));
    connect(ui->txtMiddlename, SIGNAL(textChanged(QString)), this, SLOT(modify(QString)));
    connect(ui->txtName, SIGNAL(textChanged(QString)), this, SLOT(modify(QString)));
    connect(ui->txtSurname, SIGNAL(textChanged(QString)), this, SLOT(modify(QString)));
    ui->cmd->button(QDialogButtonBox::Ok)->setEnabled(false);
}

PupilDialog::~PupilDialog()
{
    delete ui;
}

void PupilDialog::on_cmd_accepted()
{
    qDebug() << "pupil saved";
}

void PupilDialog::setName(const QString &name)
{
    ui->txtName->setText(name);
}

void PupilDialog::setMiddlename(const QString &name)
{
    ui->txtMiddlename->setText(name);
}

void PupilDialog::setSurname(const QString &name)
{
    ui->txtSurname->setText(name);
}

QString PupilDialog::surName()
{
    return ui->txtSurname->text();
}

QString PupilDialog::middleName()
{
    return ui->txtMiddlename->text();
}

QString PupilDialog::name()
{
    return ui->txtName->text();
}

void PupilDialog::modify(QString s)
{
    bool empty;
    (ui->txtName->text().isEmpty() || ui->txtMiddlename->text().isEmpty()) || ui->txtSurname->text().isEmpty() ? empty = true : empty = false;


    setWindowModified(true);
    ui->cmd->button(QDialogButtonBox::Ok)->setEnabled(!empty);
}
