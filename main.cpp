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


#include <QtGui/QApplication>
#include <QtSql>
#include "mainwindow.h"

void setMyConfig ()
{
    QSettings s ("Pashazz", "QSchoolManager");
    if (s.value("HaveConfig").isNull())
    {
        s.setValue("HaveConfig", true);
        //российская школа
        s.setValue("Settings/LowestLevel", 1);
        s.setValue("Settings/HighestLevel", 11);
        s.setValue("Settings/LowestMark", 1);
        s.setValue("Settings/HighestMark", 5);
        QStringList letters = QStringList() << QObject::tr("А") << QObject::tr("Б") << QObject::tr("В") << QObject::tr ("Г") << QObject::tr ("Д") << QObject::tr("К");
       s.setValue("Settings/PossibleClassNames",letters );

    }
}

bool createBase () //создание всей табличной структуры базы данных SQLite default.db
{
    QSqlDatabase db = QSqlDatabase::database("schmgr", false);
    if (!db.open())
    {
        qDebug() << "failed to open database" << db.databaseName() << db.lastError().text();
        return false;
    }
    QSqlQuery q (db);
    //создаем таблицу классов
    q.prepare("CREATE TABLE Grades  (id INTEGER PRIMARY KEY, num INTEGER, letter TEXT, description TEXT)");
    if (!q.exec())
    {
        qDebug() << "Failed to create table Grades " << q.lastError().text();
        return false;
    }
    q.prepare("CREATE TABLE Pupils (id INTEGER PRIMARY KEY, firstname TEXT, middlename TEXT, surname TEXT, grade INTEGER)");
    if (!q.exec())
    {
        qDebug() << "Failed to create table Pupils " << q.lastError().text();
        return false;
    }
    //пока все.

    db.close();
    return true;
}
bool upgradeBase (const QString &oldVersion)
{ //временный метод, на дорелизное время
    QFile file (QSqlDatabase::database("schmgr").databaseName());
    if (!file.exists())
        return false;
    file.remove(QObject::tr("default.db.%1").arg(oldVersion));
     if (!file.rename(QObject::tr("default.db.%1").arg(oldVersion)))
         return false;
     return createBase();
}

bool openBase ()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "schmgr");
    //проверяем файл версии
    QFile  dbFile ( QDir::current().filePath("version"));
     db.setDatabaseName(QDir::current().filePath("default.db"));
    if (!dbFile.exists())
    {
        //заполняем его
        if (createBase())
        {
            dbFile.open( QIODevice::WriteOnly);
            QTextStream s (&dbFile);
            s << qApp->applicationVersion();
            dbFile.close();
        }
        else
            return false;
    }
    else
    {
        dbFile.open(QIODevice::ReadOnly);
        QTextStream s (&dbFile);
        QString v = s.readLine();
        if (v != qApp->applicationVersion())
           {
            qDebug() << "Need to upgrade database!";
            dbFile.close();
            if (!upgradeBase(v))
            {
                qDebug() << "Failed to upgrade database!";
                   return false;
            }

            dbFile.open(QIODevice::WriteOnly | QIODevice::Truncate); //очищаем файл
            s << qApp->applicationVersion();
            dbFile.close();

        } //иначе ничего не делать
        else
            dbFile.close();
    }


    if (db.open())
    {
        qDebug() << "db opened! Going for a work";
        return true;
    }
    else
    {
        qDebug() << "db failed to open because " << db.lastError().text();
        return false;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("QSchoolManager");
    a.setApplicationVersion("0.002");
    //создаем свою директорию
    QDir myDir (QDir::home());
    const QString dirn = ".schmgr";
    if (!myDir.exists(dirn))
        myDir.mkdir(myDir.absoluteFilePath(dirn));
    QDir::setCurrent(myDir.absoluteFilePath(dirn));

    //set Linux console encoding to UTF-8
#ifndef Q_WS_WIN //no windows
     QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
     QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#else //windows encodings
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
#endif
    setMyConfig(); //устанавливаем общую конфигурацию
    if (!openBase())
    {
        qDebug() << "EPIC FAIL!";
        return -1;
    }
    MainWindow w;
    w.show();

    return a.exec();
}
