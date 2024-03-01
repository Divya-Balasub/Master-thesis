import serial
import csv
import matplotlib.pyplot as plt
import keyboard
import joblib
import sys
from sklearn.ensemble import GradientBoostingClassifier
from matplotlib.animation import FuncAnimation
import os
import serialNO
import data
from tslearn.neighbors import KNeighborsTimeSeriesClassifier
import pandas as pd
from feature_realtime import generate_code
from tsfresh.feature_extraction import feature_calculators as tsfresh_fc
from tslearn.preprocessing import TimeSeriesScalerMeanVariance
from tslearn import metrics
import numpy as np


ser = serial.Serial(data.port, data.baudrate)
loaded_rf_classifier = joblib.load('gb_classifier.joblib')
os.makedirs(data.parent_folder_path, exist_ok=True)

additional_column_value = 0
prediction =[]
result_list = []
new_gesture = True
t=30

serial_number = serialNO.read_serialNO()
data_by_serial = {}

gesture_mapping = {
    1: 'Stop',
    2: 'Speed_Up',
    3: 'Slow_Down',
    4: 'Change_Direction',
    5: 'Repeat',
    6: 'Reset'
}

feature_names = ['gyro3__mean_abs_change',
       'gyro3__change_quantiles__f_agg_"mean"__isabs_True__qh_1.0__ql_0.0',
       'acc2x__variance', 'acc2x__standard_deviation',
       'acc2x__mean_abs_change',
       'acc2x__change_quantiles__f_agg_"mean"__isabs_True__qh_1.0__ql_0.0',
       'gyro3__absolute_sum_of_changes',
       'gyro3__change_quantiles__f_agg_"var"__isabs_False__qh_1.0__ql_0.0',
       'gyro3__cid_ce__normalize_False',
       'acc2y__change_quantiles__f_agg_"mean"__isabs_True__qh_1.0__ql_0.0',
       'acc2y__mean_abs_change', 'gyro3__standard_deviation',
       'gyro3__variance', 'acc2y__variance', 'acc2y__standard_deviation',
       'acc2x__change_quantiles__f_agg_"var"__isabs_False__qh_1.0__ql_0.0',
       'gyro3__root_mean_square', 'acc2x__cid_ce__normalize_False',
       'gyro3__abs_energy',
       'gyro3__change_quantiles__f_agg_"var"__isabs_True__qh_1.0__ql_0.0',
       'gyro3__mean_n_absolute_max__number_of_maxima_7',
       'acc2y__change_quantiles__f_agg_"var"__isabs_False__qh_1.0__ql_0.0',
       'acc2y__cid_ce__normalize_False', 'gyro3__maximum',
       'gyro1__mean_n_absolute_max__number_of_maxima_7',
       'acc2x__change_quantiles__f_agg_"var"__isabs_True__qh_1.0__ql_0.0',
       'gyro3__absolute_maximum',
       'acc2y__change_quantiles__f_agg_"var"__isabs_True__qh_1.0__ql_0.0']

def detach_motor():
    ser.write(b'd')

def attach_motor():
    ser.write(b'a')

def execute_gesture(value):
    value_str = str(value).encode()
    ser.write(value_str)

def update():
    global additional_column_value
    global serial_number
    global new_gesture
    global t
    if ser.in_waiting > 0:
        readings = ser.readline().decode().strip()
        values = readings.split(',')

        values = [float(val) for val in values]

        if keyboard.is_pressed(47):  #  Press 'dot .' for 47
            print("Detaching the motor...")
            detach_motor()
        if keyboard.is_pressed(44):  # Press 'underscore -' for 44
            print("attaching the motor...")
            attach_motor()

        values.append(serial_number)
        result_list.append(values.copy())
        sliding_window = result_list[-15:]

        data_headings = ['time', 'fsr1', 'fsr2', 'fsr3', 'fsr4', 'angle', 'accx', 'accy', 'accz',
                         'gyro1', 'gyro2', 'gyro3', 'orient1', 'orient2', 'orient3', 'acc2x', 'acc2y', 'acc2z', 'serialnumber']
        df = pd.DataFrame(sliding_window, columns=data_headings)
        test = df.drop(columns=['orient1', 'orient2', 'orient3'])
        result_df = pd.DataFrame()


        if new_gesture == True:

            for feature in feature_names:
                # Generate the code for the feature
                if feature == 'gyro3__agg_linear_trend__attr_"intercept"__chunk_len_10__f_agg_"var"':
                    continue
                code = generate_code(feature)

                # Execute the code and store the result in a DataFrame
                feature_result = eval(code)

                # Rename the column to match the feature name
                feature_result.rename(columns={feature_result.columns[-1]: feature}, inplace=True)

                # Merge the result into the main DataFrame
                if result_df.empty:
                    result_df = feature_result
                else:
                    result_df = pd.merge(result_df, feature_result, on='serialnumber', how='inner')

            # Set 'serialnumber' as the index of the result DataFrame
            result_df.set_index('serialnumber', inplace=True)
            result_df = result_df.dropna()


            try:
                ans = loaded_rf_classifier.predict(result_df)
                prediction.append(ans[0])
            except ValueError as e:
                # Check if the error message indicates an empty array
                if "Found array with 0 sample(s)" in str(e):
                    print("", file=sys.stderr)
                else:
                    print(f"Error: {e}", file=sys.stderr)

        if len(prediction) >= 3 and np.all(prediction[-3:] == prediction[-1]) and prediction[-1] != 0:
            gesture=prediction[-1]
            execute_gesture(gesture)
            prediction.append(8)
            new_gesture=False
            mapped_gesture = gesture_mapping[gesture]
            print("Detected Gesture: ",mapped_gesture,"")
            print("------------------------------------------")
            t=25

        t=t-1
        if t == 0:
            new_gesture= True



if __name__ == '__main__':
    while True:
        update()
