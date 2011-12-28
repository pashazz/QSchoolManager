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


#ifndef PUPILDIALOG_H
#define PUPILDIALOG_H

#include <QtGui>

namespace Ui {
class PupilDialog;
}

class PupilDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PupilDialog(QWidget *parent, const QString &grade);
    ~PupilDialog();
    void setName (const QString &name);
    void setMiddlename (const QString &name);
    void setSurname (const QString &name);

    QString middleName();
    QString surName ();
    QString name();
    
private slots:
    void on_cmd_accepted();
    void modify(QString);
private:
    Ui::PupilDialog *ui;
};

#endif // PUPILDIALOG_H
