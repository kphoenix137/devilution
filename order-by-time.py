import sys

# Check if the file path is provided as an argument
if len(sys.argv) != 2:
    print("Usage: python script.py <file_path>")
    sys.exit(1)

# Get the file path from the command line argument
file_path = sys.argv[1]

# Open the file and read the lines
try:
    with open(file_path, 'r') as file:
        time_codes = file.readlines()
except FileNotFoundError:
    print("File not found!")
    sys.exit(1)

# Extract time codes
time_codes = [entry[-9:-1] for entry in time_codes]

# Sort the list by time codes
time_codes.sort()

# Print the sorted list
for code in time_codes:
    print(code)
