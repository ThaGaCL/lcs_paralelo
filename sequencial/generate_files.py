# Used to generate example files to test the code
# Recieves the options to generate examples
# original = for the original problem described by the professor
# big = for testing the code with big files
# noRepeat = for testing the code with no repeated characters
# noRepeatBig = for testing the code with big files and no repeated characters
import sys
import os

# Check if the user provided the correct number of arguments
if len(sys.argv) != 2:
    print("Usage: python generate_files.py <option>")
    print("Options: original, big, noRepeat, noRepeatBig")
    sys.exit(1)
option = sys.argv[1]
# Check if the option is valid
if option not in ["original", "big", "noRepeat", "noRepeatBig"]:
    print("Invalid option. Options are: original, big, noRepeat, noRepeatBig")
    sys.exit(1)


# Create the files
if option == "original":
    # Original
    with open("fileA.in", "w") as f:
        f.write("heagawghee")

    with open("fileB.in", "w") as f:
        f.write("pawheae")

    print("Original example files created. EXPECTED OUTPUT: 5")

elif option == "noRepeat":
    # No Repeat
    with open("fileA.in", "w") as f:
        f.write("ABCD")

    with open("fileB.in", "w") as f:
        f.write("XY")
    
    print("No Repeat example files created. EXPECTED OUTPUT: 0")

elif option == "noRepeatBig":
    # No Repeat Big
    with open("fileA.in", "w") as f:
        f.write("ABCD" * 2500) # 10,000 characters
    with open("fileB.in", "w") as f:
        f.write("XY" * 5000)

    print("No Repeat Big example files created. EXPECTED OUTPUT: 0")
elif option == "big":
    with open("fileA.in", "w") as f:
        f.write("ABCD" * 2500)  # 10,000 characters

    with open("fileB.in", "w") as f:
        f.write("ABCDXY" * 2500)
    print("Big example files created. EXPECTED OUTPUT: 10000")
    
# Big





