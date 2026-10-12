import os
import sys
import subprocess

def create_contest_directory(contest_name, problem_count):
    # Use the current working directory as the base path
    base_path = os.getcwd()
    contest_path = os.path.join(base_path, contest_name)

    # Create contest directory if it doesn't exist
    if not os.path.exists(contest_path):
        os.makedirs(contest_path)
        # print(f"Directory created: {contest_path}")
    else:
        print(f"Directory already exists: {contest_path}")

    # List to store the paths of the created files
    created_files = []

    # Create files for each problem
    for i in range(problem_count):
        problem_name = chr(ord('A') + i)  # Generates A, B, C, ...
        file_path = os.path.join(contest_path, f"{problem_name}.cpp")
        if not os.path.exists(file_path):
            with open(file_path, "w") as f:
                f.write("// Problem " + problem_name + "\n")
            created_files.append(file_path)  # Add file path to the list
            # print(f"File created: {file_path}")
        else:
            print(f"File already exists: {file_path}")

    # Open created files in Sublime Text
    if created_files:
        subprocess.run(["subl", *created_files])  # `subl` opens files in the same window

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 setup_contest.py <contest_name> <number_of_problems>")
        sys.exit(1)

    contest_name = sys.argv[1]
    problem_count = int(sys.argv[2])

    create_contest_directory(contest_name, problem_count)

