# Building Flask general app
import threading
from flask import Flask, render_template
import numpy
import pandas as pd
from SQLquerying import last_tendata
from SQLquerying import check_anamoly
from send_notification import send_msg

app = Flask(__name__, static_folder='static', template_folder='templates')

@app.route('/', methods=["GET", "POST"])
def main_type():
    datas = last_tendata()
    # print(last_tendata())
    # send_msg()
    return render_template('index.html', datas=datas)


@app.route('/about.html')
def about():
    return render_template('about.html')


if __name__ == "__main__":
    app.run(debug=True, port=5014)
