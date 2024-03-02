import os
from tabulate import tabulate

# Specify the folder path to count
root_folder = '/Users/divya/Documents/Data_collection/final_dataset'

# Specify the filenames to count
filenames_to_count = ['1.stop', '2.speed_up', '3.slow_down', '4.change_direction', '5.repeat', '6.reset']

# Dictionary to store the counts for each folder and filename
folder_file_counts = {folder: {filename: 0 for filename in filenames_to_count} for folder in range(1, 15)}

# Count CSV files
def count_csv_files(folder_path):
    for folder_number in range(1, 15):
        folder_subpath = os.path.join(folder_path, str(folder_number), 'dataset')
        for root, dirs, files in os.walk(folder_subpath):
            for file in files:
                if file.lower().endswith('.csv'):
                    for filename_to_count in filenames_to_count:
                        if file.startswith(filename_to_count):
                            folder_file_counts[folder_number][filename_to_count] += 1

count_csv_files(root_folder)

# Calculate row and column totals
row_totals = {folder: sum(file_counts.values()) for folder, file_counts in folder_file_counts.items()}
column_totals = {filename: sum(folder_file_counts[folder][filename] for folder in range(1, 15)) for filename in filenames_to_count}

# Display the results in table format
table_data = []
for folder, file_counts in folder_file_counts.items():
    row_data = [folder] + [file_counts[filename] for filename in filenames_to_count] + [row_totals[folder]]
    table_data.append(row_data)

column_total_row = ['Total'] + [column_totals[filename] for filename in filenames_to_count] + [sum(row_totals.values())]
table_data.append(column_total_row)

headers = ['Folder'] + filenames_to_count + ['Row Total']
table = tabulate(table_data, headers, tablefmt='pretty', numalign='center')

print(table)
