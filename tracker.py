import cv2
from ultralytics import YOLO
import numpy as np


#load models
detector = YOLO("yolov8n.pt")
depth = YOLO("yolo26n-depth.pt")

cam = cv2.VideoCapture(0)

if not cam.isOpened():
    print("Error: Could not open webcam.")
    exit()

print("Press 'q' to quit.")

while True:
    return_val, frame = cam.read()
    if not return_val:
        print("Error: Could not read frame.")
        break

    detector_results = detector(frame, classes=[0], verbose = False, stream=True)
    depth_results = depth(frame, verbose = False, stream = True)

    for result in depth_results:
        depth_map = result.depth.data.cpu().numpy()

    #loop through all the results and boxes
    for r in detector_results:
        boxes = [box for box in r.boxes if box.conf > 0.6]
        boxes.sort(key=lambda box: box.conf, reverse=True)
        boxes = boxes[:1]

        for box in boxes:
            x1, y1, x2, y2 = map(int, box.xyxy[0].int().tolist())
            centerX = int((x1 + x2) / 2)
            centerY = int((y1 + y2) / 2)

            centerX_clipped = np.clip(centerX, 0, depth_map.shape[1] - 1)
            centerY_clipped = np.clip(centerY, 0, depth_map.shape[0] - 1)
            distance = depth_map[centerY_clipped, centerX_clipped]

            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.circle(frame, (centerX, centerY), 5, (0, 0, 255), -1)

            label = f"Person: {distance * 3.28:.2f}feet "
            cv2.putText(frame, label, (x1, y1 + 15), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

    cv2.imshow("Person Depth Tracker", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cam.release()
cv2.destroyAllWindows()