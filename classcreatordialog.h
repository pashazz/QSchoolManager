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


#ifndef CLASSCREATORDIALOG_H
#define CLASSCREATORDIALOG_H

#include <QtGui>
#include <QtSql>

namespace Ui {
    class ClassCreatorDialog;
}

class ClassCreatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClassCreatorDialog(QWidget *parent = 0);
    ~ClassCreatorDialog();
        void setEditableMode (const int classId);
       int classId () {return _id;}

       QString getTitleData();

    private slots:
       void appendPossibleClassnames(const QString &num);

       void on_btnActions_accepted();

       void on_btnActions_rejected();

private:
    Ui::ClassCreatorDialog *ui;
    QSqlDatabase _db;
    void write();
    QStringList possibleClassnames;
    bool editable;
    int _id;
};

#endif // CLASSCREATORDIALOG_H
