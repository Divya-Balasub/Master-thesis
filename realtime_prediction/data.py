port = '/dev/cu.usbmodem1101'  # Replace with the appropriate port name
baudrate = 115200

participant_id = 13
additional_column_value =0

# Dictionary to map random numbers to folder names
folder_mapping = {
    1: '1.stop',
    2: '2.speed_up',
    3: '3.slow_down',
    4: '4.change_direction',
    5: '5.repeat',
    6: '6.reset',
    0: '0.default',
    10: '10.bin'
}

binFile = "10.bin."
defaultFile = "0.default."

file_path = "serial_number.txt"
parent_folder_path = "./dataset"

header = ['class','time', 'fsr1', 'fsr2', 'fsr3', 'fsr4','angle', 'accx', 'accy', 'accz',
          'gyro1', 'gyro2', 'gyro3','orient1', 'orient2', 'orient3','acc2x', 'acc2y', 'acc2z', 'participant', 'serialnumber']