import pymysql
import json
import datetime
import pandas as pd

connection = pymysql.connect(host="sql6.freesqldatabase.com", user="sql6422421", passwd="9su1EKKgZE",database="sql6422421", autocommit=True )
cursor = connection.cursor()

spo2=98
bpm=85


def last_tendata():
   retrive = ("SELECT * FROM (SELECT * FROM healthData ORDER BY Timestamp DESC LIMIT 10)Var1 ORDER BY Timestamp ASC;")
   cursor.execute(retrive)
   rows = cursor.fetchall()
   inner_dict=[{'Timestamp':rows[i][0].strftime("%d%b %H:%M"), 'SpO2':rows[i][1], 'bpm':rows[i][2]}
              for i,_ in enumerate(rows)]
   dictionary={'data':inner_dict}
   return dictionary

def check_anamoly():
   retrive = ("SELECT * FROM healthData;")
   cursor.execute(retrive)
   rows = cursor.fetchall()
   dict_=[{'Timestamp':rows[i][0].strftime("%d%m%y %H:%M:%S"), 'SpO2':rows[i][1], 'bpm':rows[i][2]}
              for i,_ in enumerate(rows)]
   df=pd.DataFrame(dict_)
   fd=df[(df['SpO2']<spo2) | (df['bpm']<bpm)]
   return fd

#cursor.close()
#connection.close()