from SQLquerying import check_anamoly
import pandas as pd
import numpy as np
import smtplib


def send_msg():
    df=check_anamoly()
    #print(df)
    s = smtplib.SMTP('smtp.gmail.com', 587)
  
    s.starttls()
    s.login("oxytracker@gmail.com", "miniproject2021")
    message = "SpO2 levels have reached" # + 'spo2 levles'
    s.sendmail("oxytracker@gmail.com", "gurujeet_shetty@gmail.com", message)