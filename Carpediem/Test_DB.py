import pymysql #pymysql 가져옴
import schedule
import time
import datetime
import os

file = open('list.txt','r')

Guild_DB = pymysql.connect( # DB 연결
      user='root',
      passwd='gkatn0512',
      host='127.0.0.1',
      db='test1',
      charset='utf8'
)

mycursor = Guild_DB.cursor() # cursor 객체 생성(연결 DB와 상호 작용)

for line in file:
      position = line.split()[0] # 길드원 직위
      name = line.split()[1] # 길드원 이름
      insert_db_data = (position, Name)
      delete_db_data = (Name, Name)
      #sql = "TRUNCATE TABLE copy_lists"
      #sql2 = "Insert into Management_lists(Position, Life, Name) values ('부마스터','0','오렌쥐냐')"
      #sql3 = "delete Name from Manage_lists a where not exists ( select Name from Copy_lists Name where %s = %s)"
      
      sql = "TRUNCATE TABLE copy_lists"
      mycursor.execute(sql)
      Guild_DB.commit()
      sql2 = "Insert into copy_lists(Position, Name) values ('%s','0','%s')"
      mycursor.execute(sql2,insert_db_data)
      Guild_DB.commit()
      sql3 = "delete Name from Manage_lists a where not exists ( select Name from Copy_lists Name where %s = %s)"
      mycursor.execute(sql3,delete_db_data)
      Guild_DB.commit()
      sql4 = "Insert IGNORE into Manage_lists(Position, Life, Name) values (%s, 0, %s)"
      mycursor.execute(sql4,insert_db_data)
      Guild_DB.commit()

      
##
##      sql2 = "delete a from Manage_lists a where not exists ( select Name from Copy_lists Name where '%s' = '%s');"
##      data = (name,name) # 튜플에 담음
              
##      mycursor.execute(sql2,data) # sql 및 데이터로 쿼리 실행 
##      Guild_DB.commit() # commit을 이용한 DB반영

Guild_DB.close() # DB close
if os.path.isfile('list.txt'):
      os.remove('list.txt')
else:
      print('Error : File not Exist')
      
              
