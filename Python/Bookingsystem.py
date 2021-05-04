import pandas as pd
import datetime
import requests
import json
from time import sleep


#Definisjon på hvordan vi skal laste inn kontakten med ESP32
# "1, 2, 3, 1, 2, 3, 4"
# [1, 2, 3, 1, 2, 3, 4]
# [Romnr., ID, Antallgjester, Time, Time, Kvarter i timer, Antall kvarter booking]

zeroset = 0         # Tall for å nullstille
booking_ok = 1      # If booking == OK, feedback 1 to ESP32
fully_booked = 2    # If booking == full, feedback 2 to ESP32
error_numb = 3      # If booking == errornumb, feedback 3 to ESP32
path = "/home/pi/Python/test.csv"
c = 1

pingid = "YOUR_KEY" # Key til pinge-signal fra ESP32
pongid = "YOUR_KEY2" # Key til feedback til ESP32
token = "YOUR_TOKEN" # Token brukeren til Lindb


# Grunnet dagens GDPR reglement, ønsket ikke gruppen å lagre informasjon om hustanden, og sletter derfor
# alle dagens bookinger ved midnatt hver kveld.
midnight = datetime.datetime.now()
midnight = midnight.strftime("%Y-%m-%d 00:00:00")

if datetime.datetime.now() == midnight:
    c = 1
   

# Funksjon for å opprette et dataframe som skal brukes til bookingsystemet
def booking_df():
    today = datetime.datetime.today()  
    today = str(today.strftime("%Y-%m-%d 00:00:00"))
    today_late = datetime.datetime.today()  
    today_late = str(today_late.strftime("%Y-%m-%d 23:45:00"))
    booking = pd.DataFrame(columns=["ID1_Kj", "ID2_Kj", "ID3_Kj", "ID4_Kj", "TOT_Kj",
                                "ID1_Stue", "ID2_Stue", "ID3_Stue", "ID4_Stue", "TOT_Stue",
                                "ID1_Sov", "ID2_Sov", "ID3_Sov", "ID4_Sov", "TOT_Sov",
                                "ID1_Bad", "ID2_Bad", "ID3_Bad", "ID4_Bad", "TOT_Bad",
                                "TOT_antallpersoner i kollektivet"],
                       index=pd.date_range(today, today_late,
                                           freq='15T'))
    booking = booking.fillna(0)
    booking = booking.transpose()
    return booking


# Funksjon for å skrive til circusofthings
def write_to_cot(keyinput, value):
    #key = keyinput
    data = {"Key": keyinput, "Token": token, "Value": value }
    headers = {"Content-Type": "application/json"}
    response = requests.put("https://circusofthings.com/WriteValue", 
                            data = json.dumps(data), 
                            headers = headers) 


# Henter ut 7-sifret kode fra Circusofthings og returnerer verdien.
def read_cot(keyinput):
    response_read = requests.get("https://circusofthings.com/ReadValue", 
                                 params = {"Key": keyinput, "Token": token})
    response_read = json.loads(response_read.content)
    value_read = response_read.get("Value")
    return value_read


# Converterer tiden slik at klokkeslettene blir slått sammen.
def convertion(inputnum):
    cot_numb = str(inputnum)
    cot_numb = list(cot_numb)
    cot_numb[3 : 5] = ["".join(cot_numb[3 : 5])]
    return cot_numb


# Ganger ut tiden av ønskete bookede kvarter og lager en slutt-tid for ønsket booking.
def end_booking_time(convertedtime, cot_bit):
    end_time = convertedtime
    antall_tider = int(cot_bit[5]) * 15
    slutt_tid = end_time + datetime.timedelta(minutes = antall_tider)
    return slutt_tid


# Funksjon for å sjekke om klokkeslettet du har tastet inn er gyldig klokkeslett fra 00:00 --> 23:00
def check_valid_time(time):
    if time[3] > str(23):
        write_to_cot(pongid, error_numb) # Skriver feilkode "3" - "Error, du har tastet feil tall"
    return time


# Sjekker om antallet gjester er lovlig.
#def check_valid_guests(cot_bit):
#    if cot_bit[2] >= str(3):
#        write_to_cot(pongid, error_numb) # Skriver feilkode "3" - "Error, du har tastet feil tall"
        

def iterate_time_livingroom(dataframe, start_tid, slutt_tid, cot_bit):
    interval = datetime.timedelta(minutes = 15)
    for i in dataframe:
        if i == start_tid:
            if cot_bit[0] == str(1) and cot_bit[1] == str(1):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID1_Stue"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(1) and cot_bit[1] == str(2):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID2_Stue"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(1) and cot_bit[1] == str(3):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID3_Stue"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(1) and cot_bit[1] == str(4):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID4_Stue"] = cot_bit[2]                     
                    start_tid += interval


# Funksjon som itererer dato og klokkeslett til dataframe og legger inn ønsket personer.
def iterate_time_kitchen(dataframe, start_tid, slutt_tid, cot_bit):
    interval = datetime.timedelta(minutes = 15)
    for i in dataframe:
        if i == start_tid:
            if cot_bit[0] == str(3) and cot_bit[1] == str(1):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID1_Kj"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(3) and cot_bit[1] == str(2):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID2_Kj"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(3) and cot_bit[1] == str(3):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID3_Kj"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(3) and cot_bit[1] == str(4):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID4_Kj"] = cot_bit[2]                     
                    start_tid += interval
                    
                    
def iterate_time_bathroom(dataframe, start_tid, slutt_tid, cot_bit):
    interval = datetime.timedelta(minutes = 15)
    for i in dataframe:
        if i == start_tid:
            if cot_bit[0] == str(2) and cot_bit[1] == str(1):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID1_Sov"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(2) and cot_bit[1] == str(2):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID2_Sov"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(2) and cot_bit[1] == str(3):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID3_Sov"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(2) and cot_bit[1] == str(4):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID4_sov"] = cot_bit[2]                     
                    start_tid += interval
                    
def iterate_time_bedroom(dataframe, start_tid, slutt_tid, cot_bit):
    interval = datetime.timedelta(minutes = 15)
    for i in dataframe:
        if i == start_tid:
            if cot_bit[0] == str(4) and cot_bit[1] == str(1):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID1_Sov"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(4) and cot_bit[1] == str(2):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID2_Sov"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(4) and cot_bit[1] == str(3):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID3_Sov"] = cot_bit[2]                     
                    start_tid += interval
            elif cot_bit[0] == str(4) and cot_bit[1] == str(4):
                while start_tid <= slutt_tid:
                    dataframe[start_tid]["ID4_Sov"] = cot_bit[2]                     
                    start_tid += interval
                     
                    
# Funksjon for å sjekke om tiden er innenfor OK - tiTsramme
def check_valid_booking(dataframe, start_tid, slutt_tid, cot_bit, inumb):
    interval = datetime.timedelta(minutes = 15)
    for i in dataframe:
        if i == start_tid:
            while start_tid <= slutt_tid:
                if cot_bit[0] == str(1):
                    if int(cot_bit[2]) + int(dataframe[start_tid]["TOT_Stue"]) > 4:
                        inumb = 1
                        print(int(cot_bit[2]) + int(dataframe[start_tid]["TOT_Stue"]))
                        break
                    start_tid += interval
                if start_tid == slutt_tid:
                    inumb = 2
                    
                if cot_bit[0] == str(2):
                    if int(cot_bit[2]) + int(dataframe[start_tid]["TOT_Bad"]) > 1:
                        inumb = 1
                        print(int(cot_bit[2]) + int(dataframe[start_tid]["TOT_Bad"]))
                        break
                    start_tid += interval
                if start_tid == slutt_tid:
                    inumb = 2                   
                               
                if cot_bit[0] == str(3):
                    if int(cot_bit[2]) + int(dataframe[start_tid]["TOT_Kj"]) > 3:
                        inumb = 1
                        print(int(cot_bit[2]) + int(dataframe[start_tid]["TOT_Kj"]))
                        break
                    start_tid += interval
                if start_tid == slutt_tid:
                    inumb = 2
                    
                if cot_bit[0] == str(4):
                    if int(cot_bit[2]) + int(dataframe[start_tid]["TOT_Sov"]) > 10:
                        inumb = 1
                        print(int(cot_bit[2]) + int(dataframe[start_tid]["TOT_Sov"]))
                        break
                    start_tid += interval
                if start_tid == slutt_tid:
                    inumb = 2
    return inumb


# Koverterer starttiden til sendt signal fra CoT
def time_convertion(time, hour):
    if time[4] == "0":
        date = datetime.datetime.today()
        date = str(date.strftime("%Y-%m-%d " + hour + ":00:00"))
        
    elif time[4] == "1":
        date = datetime.datetime.today()
        date = str(date.strftime("%Y-%m-%d " + hour + ":15:00"))
        
    elif time[4] == "2":
        date = datetime.datetime.today()
        date = str(date.strftime("%Y-%m-%d " + hour + ":30:00"))
        
    elif time[4] == "3":
        date = datetime.datetime.today()
        date = str(date.strftime("%Y-%m-%d " + hour + ":45:00"))
    else:
        write_to_cot(pongid, error_numb) # Skriver feilkode "3" - "Error, du har tastet feil tall"
    return date


# Main-funksjon, fordi man kan.       
def main():
    number = read_cot(pingid)
    if len(str(number)) == 7:
        conv = convertion(number)
        print("Sjekker om riktig antall")
        if conv[2] <= str(3):   
            valid_t = check_valid_time(conv)
            time_conv = time_convertion(conv, valid_t[3])
            date_time_str = time_conv
            date_time_obj = datetime.datetime.strptime(date_time_str, "%Y-%m-%d %H:%M:%S")
            end_time_obj = end_booking_time(date_time_obj, conv) 
            inumb = 0
            valid_book = check_valid_booking(booking, date_time_obj, end_time_obj , conv, inumb)
            print(valid_book)
            print("Sjekker...")
            if valid_book == 1:
                write_to_cot(pingid, zeroset)
                write_to_cot(pongid, fully_booked)
                print("Fullbooket")
            elif valid_book == 2:
                iterate_time_kitchen(booking, date_time_obj, end_time_obj , conv)
                iterate_time_livingroom(booking, date_time_obj, end_time_obj , conv)
                iterate_time_bedroom(booking, date_time_obj, end_time_obj , conv)
                iterate_time_bathroom(booking, date_time_obj, end_time_obj , conv)
                write_to_cot(pongid, booking_ok)
                write_to_cot(pingid, zeroset)
                print("Du har booket")
        elif number != 0:         
             write_to_cot(pongid, error_numb)
             write_to_cot(pingid, zeroset)       

# Objektorientert del, skal kjøre konstant så lenge 
while True:    
    if __name__ == "__main__":
        while c == 1:
            booking = booking_df()
            c += 1
        main()
        booking = booking.transpose()
        booking["TOT_Kj"] = booking["ID1_Kj"] + booking["ID2_Kj"] + booking["ID3_Kj"] + booking["ID4_Kj"]
        booking["TOT_Stue"] = booking["ID1_Stue"] + booking["ID2_Stue"] + booking["ID3_Stue"] + booking["ID4_Stue"]
        booking["TOT_Sov"] = booking["ID1_Sov"] + booking["ID2_Sov"] + booking["ID3_Sov"] + booking["ID4_Sov"]
        booking["TOT_Bad"] = booking["ID1_Bad"] + booking["ID2_Bad"] + booking["ID3_Bad"] + booking["ID4_Bad"]
        booking["TOT_antallpersoner i kollektivet"] = booking["TOT_Kj"] + booking["TOT_Stue"] + booking["TOT_Sov"] + booking["TOT_Bad"]
        booking = booking.transpose()
        booking.to_csv(path, index=False)
        print("jobber ...")
        sleep(8)


