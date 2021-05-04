
import json
from datetime import datetime, timedelta
from suntime import Sun, SunTimeException
from metno_locationforecast import Place, Forecast
import requests
import time


"""
Skrive strømforbruk, forbruk og pris per uke til CoT!
"""

key_sunproduction = "YOUR_KEY"
token = "YOUR_TOKEN"

def write_to_cot(value):
    data = {"Key": key_sunproduction, "Token": token, "Value": value }
    headers = {"Content-Type": "application/json"}
    response = requests.put("https://circusofthings.com/WriteValue",
                            data = json.dumps(data), headers = headers,)


while True:
    

    #NTNU Gløshaugen sine koordinater
    latitude = 63.418
    longitude = 10.406
    sunproduction = 0
    sun = Sun(latitude, longitude)    
    
    # Nåværende tid
    today = datetime.today()
    d1 = today.strftime("%d.%m.%Y")    
    
    # Finner ut av når solen står opp og går ned
    sunrise = sun.get_local_sunrise_time(today)
    sunset = sun.get_local_sunset_time(today)
    print ("Sunrise at ", sunrise.strftime( "%H:%M"))
    print ("Sunset at", sunset.strftime( "%H:%M"))
    
    suntime = (sunset-sunrise)
    print(suntime, "timer er solen oppe i løpet av en hel dag")
    
    sun_mid = (suntime/2) 
    suntop = sun_mid + sunrise
    suntop = suntop.strftime("%Y-%m-%d %H:%M:%S")
    print("Solen er på sitt høyeste", suntop)
    
    
    """
    Henter værdata fra metno!
    """
    
    user_agent = "metno_locationforecast/1.0 chrbjon@stud.ntnu.no"
    ntnu = Place("NTNU", 63.418, 10.406, 50)
    ntnu_forecast = Forecast(ntnu, user_agent)
    ntnu_forecast.update()
    interval = ntnu_forecast.data.intervals[1]
    clod_frac = interval.variables["cloud_area_fraction"]
    clod_frac = clod_frac.value
    print(clod_frac, "% skyer er det nå ")
    
    
    """
    150 kvm med tak. Heller 20 grader nord --> sør. Beregn oppladning av dette ved utnyttelse av sola.
    Med 75 kvm takflate beregner man 44 solcellepaneler. Effekt i kW er 13 420. Estimert årsproduksjon er 10 736 kWh.
    Premium svart 305 W panel. Ganger dette med 2 fordi det er 150 kvm tilgjengelig takoverflate.
    
    Setter forskjellige konstanter avhengig av hvor mye solenergi som produserers med x antall timer +/- når solen er på sitt høyeste.
    Dette beregnes utifra skyprosenten.
    """
    
    
    #Beregne solens produksjon utifra høyde og skyprosent
    two_add = datetime.today() + timedelta(hours = 2)
    two_sub = datetime.today() - timedelta(hours = 2)
    four_add = datetime.today() + timedelta(hours = 4)
    four_sub = datetime.today() - timedelta(hours = 4)
    six_add = datetime.today() + timedelta(hours = 6)
    six_sub = datetime.today() - timedelta(hours = 6)
    eight_add = datetime.today() + timedelta(hours = 8)
    eight_sub = datetime.today() - timedelta(hours = 8)
    
    
    if 0 < clod_frac and clod_frac < 25 and str(two_sub) < suntop and suntop < str(two_add):
        sunproduction = 100
    
    elif 25 < clod_frac and clod_frac < 50 and str(four_sub) < suntop and suntop < str(four_add):
        sunproduction = 75
    
    elif 50 < clod_frac and clod_frac < 75 and str(six_sub) < suntop and suntop < str(six_add):
        sunproduction = 50
    
    elif 75 < clod_frac and clod_frac < 100 and str(eight_sub) < suntop and suntop < str(eight_add):
        sunproduction = 25
    
    """
    Beregner maksimal effekt fra sola i løpet av en time. 
    """
    
    sun_prod = ((13420 * 2)/365)/24          #Toppverdien for 88 panel for en time
    
    if sunproduction == 100:
        print("Sola produserer", round(sun_prod, 3), "kWh)")
        write_to_cot(round(sun_prod, 3))
    elif sunproduction == 75:
        print("Sola produserer", round(sun_prod * 0.75), "kWh")
        write_to_cot(round(sun_prod * 0.75, 3))
    elif sunproduction == 50:
        print("Sola produserer", round(sun_prod * 0.5, 3), "kWh")
        write_to_cot(round(sun_prod * 0.5, 3))
    elif sunproduction == 25:
        print("Sola produserer", round(sun_prod * 0.25, 3), "kWh")
        write_to_cot(round(sun_prod * 0.25, 3))
    
    time.sleep(1800)
