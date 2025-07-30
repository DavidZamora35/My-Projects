import requests
import time
from pymongo import MongoClient
from bs4 import BeautifulSoup

#Loop the program infinitely, performing 1 loop every 3 minutes.
url = "https://finance.yahoo.com/most-active"
while(True):
    #Use beautiful soup to parse the html of the given URL.
    response = requests.get(url)

    if (response.status_code != 200):
        response = requests.get(url)
        if (response.status_code != 200):
            print("Request failed, exiting program.")
            exit()

    soup = BeautifulSoup(response.text, "html.parser")

    #Store every row of stock information in the stocks variable.
    stocks = soup.find_all("tr", {"class":"simpTblRow"})

    #Obtain the symbol, name, price(intraday), change and volume of the stocks.
    query = []
    for stock in stocks:
        symbol = stock.find("td", {"aria-label":"Symbol"}).text
        name = stock.find("td", {"aria-label":"Name"}).text
        price = stock.find("td", {"aria-label":"Price (Intraday)"}).text
        change = stock.find("td", {"aria-label":"Change"}).text
        volume = stock.find("td", {"aria-label":"Volume"}).text

        query.append({"Symbol":symbol, "Name":name, "Price (Intraday)":price, "Change":change, "Volume":volume})

    #print(query)

    #Insert the gathered data into the database.
    client = MongoClient()
    db = client['data']
    collection = db['stocks']

    collection.delete_many({});
    collection.insert_many(query)

    #Tell program to sleep for 3 minutes.
    print("Data gathered, sleeping for 3 minutes.")
    time.sleep(180)
