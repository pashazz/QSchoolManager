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


#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QtGui>
#include <QtSql>

namespace Ui {
class MoveDialog;
}

class MoveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MoveDialog(QWidget *parent, const QString &curYear, const QString &curLetter);
    ~MoveDialog();

    int year ();
    QString letter();
    
private slots:
    void on_cbYear_currentIndexChanged(const QString &arg1);

private:
    Ui::MoveDialog *ui;
    void fillBox();
    void fillLetter(const QString&);
    QString cy, cl;
    QSqlDatabase _db;

};

#endif // MOVEDIALOG_H
