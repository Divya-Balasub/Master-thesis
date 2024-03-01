import re

def generate_code(input_string):
    # Split the string using '__' as the delimiter
    split_parts = re.split("__", input_string)

    part1 = split_parts[0]
    part2 = split_parts[1]

    optional_params = {}

    for part in split_parts[2:]:
        key, value = part.rsplit("_", 1)
        if value.isdigit():
            value = int(value)  # Convert to float if it's a number
        elif value.lower() == "true":
            value = True
        elif value.lower() == "false":
            value = False
        # if key == "f_agg":
        #  value = f'"{value.replace("_", "=")}"'
        optional_params[key] = value

    # Create the code snippet
    code = (
        f"test.groupby('serialnumber')['{part1}'].apply(lambda x: "
        f"tsfresh_fc.{part2}(x, {', '.join([f'{key}={value}' for key, value in optional_params.items()])})).reset_index()"
    )

    return code


import sklearn
print(sklearn.__version__)