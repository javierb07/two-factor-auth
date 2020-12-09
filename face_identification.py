# IE-59000 Electromechanical Robotic Systems
# Face identification - Javier Belmonte

# Import necessary libraries
import cv2
import serial
import os
import time
import pickle

def identifyFace(nameSent):
    cv2.namedWindow("Webcam", cv2.WINDOW_NORMAL)
    vc = cv2.VideoCapture(0)    # Start video capture from webcam
    # Convert the resolutions from float to integer
    frame_width = int(vc.get(3))
    frame_height = int(vc.get(4))
    # Define the codec and create VideoWriter object.The output is stored in 'outpy.avi' file.
    out = cv2.VideoWriter('outpy.avi', cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'), 10, (frame_width, frame_height))
    if vc.isOpened():  # try to get the first frame
        rval, frame = vc.read()
    else:
        rval = False
    while rval:
        # Write the frame into the file 'output.avi'
        out.write(frame)
        cv2.imshow("Webcam", frame)
        rval, frame = vc.read()
        key = cv2.waitKey(20)
        if key == 27:  # exit on ESC
            vc.release()
            out.release()
            cv2.destroyWindow("Webcam")
            return
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  # Convert to gray scale
        faces = faceCascade.detectMultiScale(gray, scaleFactor=1.21, minNeighbors=5)  # Detect faces in the image
        for (x, y, w, h) in faces:  # Iterate through the found faces
            # Get the region of interest in grayscale
            roi_gray = gray[y:y + h, x:x + w]
            # Recognize the face
            id__, conf = recognizer.predict(roi_gray)
            # Draw rectangle
            color = (255, 0, 0)
            stroke = 2
            end_cord_x = x + w
            end_cord_y = y + h
            cv2.rectangle(frame, (x, y), (end_cord_x, end_cord_y), color, stroke)
            # If the correct person is recognized
            if conf >= 45 and conf <= 80:
                # Write the name in the frame
                font = cv2.FONT_HERSHEY_SIMPLEX
                name = labels[id__].upper()
                color = (255, 255, 255)  # Black text
                stroke = 2
                cv2.putText(frame, name, (x, y - 10), font, 1, color, stroke, cv2.LINE_AA)
                # Send the name through serial to the Arduino only once
                if not nameSent:
                    serialComm.write(name.encode())  # Send the number of faces through serial to Arduino
                    nameSent = True
            # If an unauthorized person is recognized
            else:
                # Send intruder to Arduino only once
                if not nameSent:
                    intruder = "intruder"
                    serialComm.write(intruder.encode())  # Send the number of faces through serial to Arduino
                    nameSent = True


serialComm = serial.Serial("COM7", 9600, timeout=0.01)  # Create an object to read the serial channel
# load cascade classifier training file for haarcascade
faceCascade = cv2.CascadeClassifier("./haarcascade_frontalface_default.xml")
time.sleep(3)   # Wait for serial communication with Arduino to be established
recognizer = cv2.face.LBPHFaceRecognizer_create()
recognizer.read("trainer.yml")  # Load the trained recognizer
labels = {} # Diccionary for our labels
nameSent = False

with open("labels.pickle", 'rb') as f:  # 'wb' stands for writing bytes
    og_labels = pickle.load(f)  # Get the labels from the training file
    labels = {v:k for k,v in og_labels.items()} # Reverse the key value order

waitingForCom = True    # Variable to keep the loop running only when needed

while waitingForCom:
    data = serialComm.readline().decode('ascii')  # Read the serial channel and store data
    if data == "correct_pass":  # If the correct password was received from Arduino
        identifyFace(nameSent)  # Call the function to identify faces
        waitingForCom = False   # Break out of the loop now that the identification is done

serialComm.close()  # close the serial channel, it's no longer needed

