# test for å laste ned strømdata

""" STRØMPRISENE ER OPPGITT I EURO OG MÅ ETTER DEN KONSTANTE DAGENS KURS REGNES OM,
    VERDIENE ER HENTET INN I MEGAWATTTIMER, SÅ HVIS MAN GANGER INN DAGENS KURS OG DELER PÅ 1000
    Strømprisene er oppgitt eks. nettleie og andre tillegg i pris, antar en nettleie på 44,54,- ØRE/KWH"""

from entsoe import EntsoePandasClient
import pandas as pd
import datetime
import requests
import json
import time

key_price = "14457"
key_week = "21796"
token = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1NTE0In0.aR3lg73MysbYXlePTMv9CMHVzHYOYLIj5QC_A2HmZSs"

def write_to_cot(key, units):
    data = {"Key": key, "Token": token, "Value": units }
    headers = {"Content-Type": "application/json"}
    response = requests.put("https://circusofthings.com/WriteValue",
                            data = json.dumps(data), headers = headers,)


while True:
    
    
    consumption_year = 20000        # 20 000 kWh i året.
    days = 365
    daily_consumption = consumption_year/days
    print(daily_consumption, "kWh hver dag for en gjennomsnittlig husstand")
    
    """
    Antar at en vaskemaskin, oppvaskmaskin, tørketrommel og komfyr bruker 1 kWh pr gang.
    Varmtvannsbereder bruker 7 kWh i løpet av en dag. Kjøleskap og fryser bruker 3 kWh hver i løpet av en dag.
    Varmeovner på soverom bruker 4 kWh i løpet av en dag. Andre hverdagslige el komponenter bruker 4 kWh i løpet av en dag.
    """

    
    # Bestemme konstanter for hver elektriske komponent og dens forbruk pr dag. Hver verdi er oppgitt i kWh
    dishwasher = 1
    stove = 1
    washing_machine = 1
    dryer = 1
    cooler = 3
    fridge = 3
    heaters = 6
    others = 4
    water_heater = 7
    
    # Kalkulere strømregning for en uke
    units = ((dishwasher + stove + washing_machine + dryer) * 7 + cooler + fridge + heaters + others + water_heater) * 7
    write_to_cot(key_week, units)
    
    week_consumption = print(units, "kWh er forbruket for en uke i kollektivet")
    
    
    """
    Beregning av pris for en dag og en uke for kollektivet
    """
    
    today = datetime.datetime.now()   
    today = str(today.strftime("%Y%m%d"))
    
    tomorrow = datetime.datetime.now() + datetime.timedelta(days=1)
    tomorrow = str(tomorrow.strftime("%Y%m%d"))
    


    df = pd.read_csv("https://data.norges-bank.no/api/data/EXR/M.EUR.NOK.SP?lastNObservations=1&format=csv",
                     sep=';')
    df = df.filter(items=["BASE_CUR", 
                          "QUOTE_CUR", 
                          "TIME_PERIOD", 
                          "OBS_VALUE"])
    eur = df["OBS_VALUE"]
    date = df["TIME_PERIOD"]
    
    client = EntsoePandasClient(api_key = "9e978604-41d5-4112-a3f2-09c836d4e5a5")
    
    # Fordelen ved å velge timezone er at den automatisk justerer CET til sommertidsudo 
    start = pd.Timestamp(today, tz = "Europe/Brussels")
    end = pd.Timestamp(tomorrow, tz = "Europe/Brussels")
    country_code = "NO_3"  # MidtNorge
    ts = client.query_day_ahead_prices(country_code, start=start, end=end).to_frame()
    price = ts.multiply(eur)
    price = round(price.div(10), 2) # Deler egentlig på 1000, da ender man opp i NOK.
     
                                    # Men det var mer natulig å dele på 10 for å få det i Øre.
    """
    Nettleie er 44,54 øre/kWh = 0,4454 kr/kWh.
    """
    
    price_day = float(sum(price)/1000) + float(0.4454)
    print(round(float(price_day), 5), "er kr/kwh prisen for en dag")
    
    
    price_week = float(price_day) * float(units)
    print(round(price_week, 2), "kr koster det for en uke i kollektivet")
    write_to_cot(key_price, price_week)
    
    
    test1 = ts
    test1 = test1.squeeze()
    #Elektrisitetsdata for trondheimsregionen, eller midt-norge som det blir kalt

    time.sleep(1800)