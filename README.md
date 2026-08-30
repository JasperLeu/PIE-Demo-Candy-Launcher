**Steps to run the thing on your computer**

1. Install python 3.11 or 3.12. Add python.exe to PATH
2. Install Visual Studio Code
3. Open command prompt in terminal
4. Run commands:        \
  mkdir my_project      \
  cd my_project         \
  python -m venv .env   \
  .env\Scripts\activate                 \
  pip install ultralytics opencv-python numpy
6. Make the file "tracker.py" inside my_project, either importing the file inside this repo or copy the code over.
7. [ctrl + shift + p] -> select interpreter -> the option with .env
8. Just press the run button in vs code.
9. If you don't have vs code, just put the file into your project folder and run using: python tracker.py
