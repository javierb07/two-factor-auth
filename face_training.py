# IE-59000 Electromechanical Robotic Systems
# Two-Factor Authentication - Javier Belmonte

# Import necessary libraries
import os
import cv2
import numpy as np
import pickle
from PIL import Image

base_dir = os.path.dirname(os.path.abspath(__file__))
image_dir = os.path.join(base_dir, "images")
faceCascade = cv2.CascadeClassifier("./my_haarcascade.xml") # Face Classifier
recognizer = cv2.face.LBPHFaceRecognizer_create()   # Face recognizer

current_id = 0
label_ids = {}  # dictionary containing all the people I have identified
y_labels = []   # list of numbers related to the labels
x_train = []    # picture values

for root, dirs, files in os.walk(image_dir):
    for file in files:
        if file.endswith("jpg") or file.endwith("png"):
            path = os.path.join(root, file)
            label = os.path.basename(os.path.dirname(path)).replace(".jpg"," ").replace(".png"," ").replace(" ", "-").lower()
            #print(label, path)

            if label not in label_ids:
                label_ids[label] = current_id
                current_id += 1
            id_now = label_ids[label]
            #print(label_ids)

            pil_image = Image.open(path).convert("L")  # open image and convert it to grayscale.
            size = (500,500)
            final_image = pil_image.resize(size, Image.ANTIALIAS)
            image_array = np.array(final_image, "uint8")
            #print(image_array)

            # Detect faces in the image
            faces = faceCascade.detectMultiScale(
                image_array,
                scaleFactor=1.04,  # 1.25
                minNeighbors=5,  # 5
                minSize=(20, 20),  # 30, 30
                flags=cv2.CASCADE_SCALE_IMAGE
            )
            print(faces)
            # Draw a rectangle around the faces
            for (x, y, w, h) in faces:
                roi = image_array[y:y+h, x:x+w]
                x_train.append(roi)  # this is our training data
                y_labels.append(id_now)

# save all labels as a labels.pickle file (in the same directory)
with open("labels.pickle", 'wb') as file:  # 'wb' stands for writing bytes
    pickle.dump(label_ids, file)
# Train and save the recognizer
recognizer.train(x_train, np.array(y_labels))
recognizer.save("trainer.yml")