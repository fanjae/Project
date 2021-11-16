from bs4 import BeautifulSoup
from pprint import pprint
from operator import itemgetter
import urllib.request
import time
import datetime
import schedule


data_list = []
file = open("list.txt",'w') # 쓰기 모드로 엶
def get_soup(target_url):
        html = urllib.request.urlopen(target_url).read() # urllib 라이브러리로 target_url 읽음
        soup = BeautifulSoup(html,'html.parser')
        return soup

def extract_data(soup): # rank_table로 부터 길드원 list 읽어옴
        table = soup.find('table',{'class': 'rank_table'}) # html에서 rank_table이 각 길드원의 list를 의미함을 확인.
        trs = table.find_all('tr')
        for idx, tr in enumerate(trs):
            if idx > 0: # 0번째는 각 행의 의미를 지칭하므로 제외
                tds = tr.find_all('td') # tr로부터 각 열을 찾음
                tr_list = [] # list 초기화
                t = tds[0].text.strip() # Position
                t1 = tds[1].text.strip().split('\n')[0] # Name
                #t2 = tds[2].text.strip() # Job
                #t3 = tds[3].text.strip() # Exp
                #t4 = tds[4].text.strip() # Popular

                
                if(len(trs) == 1): # 1인 경우 해당 데이터 존재하지 않음
                        break
                else:
                        tr_list.append(t)  # 등급값 리스트에 넣음
                        tr_list.append(t1) # 이름값 리스트에 넣음
                        data_list.append(tr_list) # 이둘을 합쳐서 하나의 리스트에 넣음(하나의 사탕봉지)
                        
def print_list_write(data_list): # 리스트 출력 및 파일에 쓰는 과정(출력은 현재 지운 상태)
        data_list.sort(key=itemgetter(1)) # Name Sort 특정 인덱스를 기준으로 정렬
        for x in data_list:
                data = "%s %s\n" %(x[0],x[1]) # File 형식에 맞게 등급, 이름
                file.write(data) #파일에 씀
        file.close() # 파일 닫음


# 실질적인 프로그램 시작 부분
def start():
        for i in range(1,15): # 14page까지 가는 것. (숫자 적당히 조절 가능)
                target_url = 'https://maplestory.nexon.com/Common/Guild?gid=12262&wid=45&page={}'.format(i) # 해당 형식에 맞춰서 target_url 지정
                soup = get_soup(target_url)
                extract_data(soup) # url_data 파싱
    
        print_list_write(data_list)

schedule.every().day.at("00:00:00").do(start)

while True:
        schedule.run_pending()
        time.sleep(1)
