import cv2
from ultralytics import YOLO
import numpy as np
import serial
import time

# load models
detector = YOLO("yolov8n.pt")
depth = YOLO("yolo26n-depth.pt")
cam = cv2.VideoCapture(0)

selectedPerson = 1
white = (255, 255, 255)
red = (0, 0, 255)
FOV = 75.8

# arduino communication
arduino = serial.Serial(port='COM3', baudrate=9600, timeout=1)
time.sleep(2)

# Exit prompt and error check
if not cam.isOpened():
    print("Error: Could not open webcam.")
    exit()
print("Press 'q' to quit.")

while True:
    return_val, frame = cam.read()
    if not return_val:
        print("Error: Could not read frame.")
        break

    detector_results = detector(frame, classes=[0], verbose=False, stream=True)
    depth_results = depth(frame, verbose=False, stream=True)

    for result in depth_results:
        depth_map = result.depth.data.cpu().numpy()

    # loop through all the results and boxes
    for r in detector_results:
        boxes = [box for box in r.boxes if box.conf > 0.6]
        boxes.sort(key=lambda box: box.conf, reverse=True)
        boxes = boxes[:1]

        currNum = 0
        if 0 < len(boxes) < selectedPerson:
            selectedPerson = len(boxes)
        for box in boxes:
            x1, y1, x2, y2 = map(int, box.xyxy[0].int().tolist())
            centerX = int((x1 + x2) / 2)
            centerY = int((y1 + y2) / 2)

            centerX_clipped = np.clip(centerX, 0, depth_map.shape[1] - 1)
            centerY_clipped = np.clip(centerY, 0, depth_map.shape[0] - 1)
            distance = depth_map[centerY_clipped, centerX_clipped] * 3.28

            # Labeling person
            currNum += 1
            currColor = white
            angle = (centerX/cam.get(cv2.CAP_PROP_FRAME_WIDTH) * FOV - FOV/2)
            if currNum == selectedPerson:
                currColor = red
                arduino.write(str(f"{angle:.2f},{distance:.2f}").encode('utf-8'))

            cv2.rectangle(frame, (x1, y1), (x2, y2), currColor, 2)
            cv2.circle(frame, (centerX, centerY), 5, currColor, -1)
            label = f"Person {str(currNum)}, {angle:.2f}deg, {distance:.2f}feet "
            cv2.putText(frame, label, (x1, y1 + 15), cv2.FONT_HERSHEY_SIMPLEX, 0.6, white, 2)
    cv2.imshow("Person Depth Tracker", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cam.release()
cv2.destroyAllWindows()