#!/bin/bash
GREEN='\033[0;32m'
RED='\033[0;31m'
NOCOLOR='\033[0m'

function checkexitstatus {
   if [ $? -eq 0 ]; then
      printf "${GREEN}PASS${NOCOLOR}\n"
   else
      printf "${RED}FAIL${NOCOLOR}\n"
      make clean
      exit
   fi
}

make

# create tables
./a.out "CREATE TABLE a (b INT, PRIMARY KEY(b));"
checkexitstatus

./a.out "CREATE TABLE a (b INT, c INTEGER, d FLOAT, e VARCHAR(50), f DATETIME, g BOOLEAN, PRIMARY KEY(b));"
checkexitstatus

./a.out "CREATE TABLE a (b INT, c INT, PRIMARY KEY(b), FOREIGN KEY(c) REFERENCES d);"
checkexitstatus

./a.out "CREATE TABLE a (b INT, c INT, d INT, PRIMARY KEY(b), FOREIGN KEY(c) REFERENCES cc, FOREIGN KEY (d) REFERENCES dd);"
checkexitstatus

# drop tables
./a.out "DROP TABLE x;"
checkexitstatus

./a.out "CREATE INDEX x ON x (a);"
checkexitstatus

./a.out "CREATE INDEX x ON x (a, b, c, d);"
checkexitstatus

./a.out "DROP INDEX x ON x;"
checkexitstatus

# insert tables
./a.out "INSERT INTO x VALUES (1);"
checkexitstatus

./a.out "INSERT INTO x VALUES (1, \"a\", 123.45);"
checkexitstatus

# delete tables
./a.out "DELETE FROM x WHERE true;"
checkexitstatus

./a.out "DELETE FROM x WHERE 1 > 10;"
checkexitstatus

./a.out "DELETE FROM x WHERE 1 > 10 AND true AND NOT false;"
checkexitstatus

./a.out "UPDATE x SET a = \"hello\" WHERE true;"
checkexitstatus

# Dekhtyar Examples
./a.out "CREATE TABLE list(
      LastName VARCHAR(16),
      FirstName VARCHAR(16),
      grade INT,
      classroom INT,
      PRIMARY KEY(FirstName,LastName));"
checkexitstatus

./a.out "CREATE TABLE teachers(
      Last VARCHAR(16),
      First VARCHAR(16),
      classroom INT, 
      PRIMARY KEY(classroom));"
checkexitstatus


./a.out "INSERT INTO list VALUES('AMY', 'PATRINA', 1, 102);"
checkexitstatus
./a.out "INSERT INTO list VALUES('AREHART', 'VERTIE', 3, 107);"
checkexitstatus
./a.out "INSERT INTO list VALUES('ATWOOD', 'BETHEL', 5, 109);"
checkexitstatus
./a.out "INSERT INTO list VALUES('BALBOA', 'MEL', 1, 103);"
checkexitstatus
./a.out "INSERT INTO list VALUES('BARTKUS', 'REYNALDO', 1, 102);"
checkexitstatus
./a.out "INSERT INTO list VALUES('BIBB', 'SHANAE', 1, 103);"
checkexitstatus
./a.out "INSERT INTO list VALUES('BRIGHTBILL', 'ANTONY', 1, 102);"
checkexitstatus
./a.out "INSERT INTO list VALUES('BRINE', 'FRANKLYN', 0, 106);"
checkexitstatus
./a.out "INSERT INTO list VALUES('BROMLEY', 'EVELINA', 1, 103);"
checkexitstatus
./a.out "INSERT INTO list VALUES('BUSTILLOS', 'HILMA', 0, 106);"
checkexitstatus
./a.out "INSERT INTO list VALUES('BYRUM', 'BENNIE', 0, 105);"
checkexitstatus
./a.out "INSERT INTO list VALUES('CAR', 'MAUDE', 2, 101);"
checkexitstatus
./a.out "INSERT INTO list VALUES('CHIARAMONTE', 'NOVELLA', 2, 101);"
checkexitstatus
./a.out "INSERT INTO list VALUES('CRANMER', 'CAREY', 5, 109);"
checkexitstatus
./a.out "INSERT INTO list VALUES('DANESE', 'JANEE', 4, 111);"
checkexitstatus
./a.out "INSERT INTO list VALUES('DROP', 'SHERMAN', 0, 104);"
checkexitstatus
./a.out "INSERT INTO list VALUES('FLACHS', 'JEFFRY', 5, 109);"
checkexitstatus
./a.out "INSERT INTO list VALUES('FULVIO', 'ELTON', 4, 111);"
checkexitstatus
./a.out "INSERT INTO list VALUES('GELL', 'TAMI', 0, 104);"
checkexitstatus
./a.out "INSERT INTO list VALUES('GERSTEIN', 'AL', 5, 109);"
checkexitstatus
./a.out "INSERT INTO list VALUES('GOODNOE', 'GAYLE', 4, 111);"
checkexitstatus
./a.out "INSERT INTO list VALUES('GRABILL', 'JULIENNE', 0, 106);"
checkexitstatus
./a.out "INSERT INTO list VALUES('GROENEWEG', 'CRYSTA', 3, 107);"
checkexitstatus
./a.out "INSERT INTO list VALUES('GRUNIN', 'EMILE', 5, 109);"
checkexitstatus
./a.out "INSERT INTO list VALUES('HANNEMANN', 'CHANTAL', 1, 102);"
checkexitstatus
./a.out "INSERT INTO list VALUES('HOESCHEN', 'LYNNETTE', 4, 108);"
checkexitstatus
./a.out "INSERT INTO list VALUES('HONES', 'GUILLERMINA', 0, 104);"
checkexitstatus
./a.out "INSERT INTO list VALUES('HOOSOCK', 'LANCE', 1, 103);"
checkexitstatus
./a.out "INSERT INTO list VALUES('HOUTCHENS', 'THEO', 0, 106);"
checkexitstatus
./a.out "INSERT INTO list VALUES('HUANG', 'TAWANNA', 5, 109);"
checkexitstatus
./a.out "INSERT INTO list VALUES('JAGNEAUX', 'ELVIRA', 6, 112);"
checkexitstatus
./a.out "INSERT INTO list VALUES('KRISTENSEN', 'STORMY', 6, 112);"
checkexitstatus
./a.out "INSERT INTO list VALUES('LAPLANT', 'SUMMER', 2, 101);"
checkexitstatus
./a.out "INSERT INTO list VALUES('LARKINS', 'GAYLE', 4, 110);"
checkexitstatus
./a.out "INSERT INTO list VALUES('LEAPER', 'ADRIAN', 4, 111);"
checkexitstatus
./a.out "INSERT INTO list VALUES('LUSKEY', 'BRITT', 4, 108);"
checkexitstatus
./a.out "INSERT INTO list VALUES('MACIAG', 'CHET', 5, 109);"
checkexitstatus
./a.out "INSERT INTO list VALUES('MADLOCK', 'RAY', 4, 110);"
checkexitstatus
./a.out "INSERT INTO list VALUES('MOWATT', 'KITTIE', 0, 105);"
checkexitstatus
./a.out "INSERT INTO list VALUES('NABOZNY', 'CHRISSY', 3, 107);"
checkexitstatus
./a.out "INSERT INTO list VALUES('NAKAHARA', 'SHERON', 0, 105);"
checkexitstatus
./a.out "INSERT INTO list VALUES('NETZEL', 'JODY', 0, 105);"
checkexitstatus
./a.out "INSERT INTO list VALUES('NOGODA', 'ISMAEL', 0, 105);"
checkexitstatus
./a.out "INSERT INTO list VALUES('PASSEY', 'RAYLENE', 4, 110);"
checkexitstatus
./a.out "INSERT INTO list VALUES('PINNELL', 'ROBBY', 3, 107);"
checkexitstatus
./a.out "INSERT INTO list VALUES('PREHM', 'SHANEL', 0, 104);"
checkexitstatus
./a.out "INSERT INTO list VALUES('RANSLER', 'RODGER', 1, 102);"
checkexitstatus
./a.out "INSERT INTO list VALUES('RODDEY', 'CYRUS', 4, 110);"
checkexitstatus
./a.out "INSERT INTO list VALUES('RUNKLE', 'MARCUS', 1, 102);"
checkexitstatus
./a.out "INSERT INTO list VALUES('SAADE', 'TOBIE', 4, 110);"
checkexitstatus
./a.out "INSERT INTO list VALUES('SANTORY', 'JORDON', 3, 107);"
checkexitstatus
./a.out "INSERT INTO list VALUES('SCHUTZE', 'LANELLE', 5, 109);"
checkexitstatus
./a.out "INSERT INTO list VALUES('SOLOMAN', 'BRODERICK', 0, 106);"
checkexitstatus
./a.out "INSERT INTO list VALUES('STIRE', 'SHIRLEY', 6, 112);"
checkexitstatus
./a.out "INSERT INTO list VALUES('SUDA', 'DARLEEN', 4, 110);"
checkexitstatus
./a.out "INSERT INTO list VALUES('TRAYWICK', 'KERI', 1, 102);"
checkexitstatus
./a.out "INSERT INTO list VALUES('VANDERWOUDE', 'SHERWOOD', 3, 107);"
checkexitstatus
./a.out "INSERT INTO list VALUES('VANVLIET', 'COLLIN', 0, 106);"
checkexitstatus
./a.out "INSERT INTO list VALUES('WIRTZFELD', 'DELORAS', 0, 106);"
checkexitstatus
./a.out "INSERT INTO list VALUES('YUEN', 'ANIKA', 1, 103);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('MACROSTIE', 'MIN', 101);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('COVIN', 'JEROME', 102);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('MOYER', 'OTHA', 103);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('NIBLER', 'JERLENE', 104);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('MARROTTE', 'KIRK', 105);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('TARRING', 'LEIA', 106);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('URSERY', 'CHARMAINE', 107);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('ONDERSMA', 'LORIA', 108);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('KAWA', 'GORDON', 109);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('SUMPTION', 'GEORGETTA', 110);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('KRIENER', 'BILLIE', 111);"
checkexitstatus
./a.out "INSERT INTO teachers VALUES('SUGAI', 'ALFREDA', 112);"
checkexitstatus


./a.out "SELECT *
FROM teachers;"
checkexitstatus

./a.out "SELECT *
FROM list
WHERE classroom = 103;"
checkexitstatus

./a.out "SELECT Last, First
FROM teachers;"
checkexitstatus

./a.out "SELECT LastName, FirstName
FROM list
WHERE grade = 2;"
checkexitstatus

./a.out "SELECT *
FROM list
WHERE grade < 2;"
checkexitstatus

./a.out "SELECT * 
FROM list
WHERE grade > 4;"
checkexitstatus

./a.out "SELECT *
FROM list
WHERE grade <= 2;"
checkexitstatus

./a.out "SELECT * 
FROM list
WHERE grade >=5;"
checkexitstatus

./a.out "
SELECT * 
FROM list
WHERE grade != 4;"
checkexitstatus

./a.out "SELECT *
FROM list
WHERE grade < 5 AND grade > 3;"
checkexitstatus

./a.out "SELECT DISTINCT grade
FROM list;"
checkexitstatus

./a.out "SELECT DISTINCT classroom
FROM list
WHERE grade = 1;"
checkexitstatus

./a.out "SELECT classroom
FROM list
WHERE LastName = 'BIBB';"
checkexitstatus

./a.out "SELECT * 
FROM teachers
WHERE Last = 'MOYER';"
checkexitstatus

./a.out "SELECT *
FROM teachers
WHERE Last <> 'MOYER';"
checkexitstatus

./a.out "SELECT COUNT(*)
FROM teachers;"
checkexitstatus

./a.out "SELECT COUNT(*) 
FROM list;"
checkexitstatus

./a.out "SELECT MAX(grade), MIN(grade), MAX(classroom), MIN(classroom)
FROM list;"
checkexitstatus

./a.out "SELECT grade, COUNT(*)
FROM list
GROUP BY grade;"
checkexitstatus

./a.out "SELECT classroom, COUNT(*)
FROM list
WHERE grade = 1
GROUP BY classroom;"
checkexitstatus

./a.out "SELECT grade, COUNT(*)
FROM list
GROUP BY grade
HAVING COUNT(*) > 8;"
checkexitstatus

./a.out "SELECT DISTINCT grade
FROM list
ORDER BY grade;"
checkexitstatus

./a.out "SELECT DISTINCT classroom
FROM list
ORDER BY grade;"
checkexitstatus

./a.out "SELECT FirstName, LastName, First, Last
FROM list, teachers
WHERE list.classroom = teachers.classroom;"
checkexitstatus

./a.out "SELECT l.FirstName, l.LastName, t.First, t.Last
FROM list l,  teachers t
WHERE l.classroom = t.classroom;"
checkexitstatus

./a.out "SELECT l.FirstName, l.LastName
FROM list l, list pa
WHERE pa.FirstName = 'AMY' AND pa.LastName = 'PATRINA' AND
      l.classroom = pa.classroom;"
checkexitstatus

./a.out "SELECT l.FirstName, l.LastName
FROM list l, list pa
WHERE pa.FirstName = 'AMY' AND pa.LastName = 'PATRINA' AND
      l.classroom = pa.classroom
ORDER BY l.LastName;"
checkexitstatus

./a.out "SELECT l.FirstName, l.LastName
FROM list l, teachers t
WHERE t.classroom = 103 AND t.classroom = l.classroom AND
      t.LastName <> 'BIBB';"
checkexitstatus

./a.out "SELECT l.classroom, COUNT(*)
FROM list l,  teachers t
WHERE l.classroom = t.classroom AND
      l.grade > 3
GROUP BY l.classroom;"
checkexitstatus

make clean
