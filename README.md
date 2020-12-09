# Two Factor Authentication

I developed this system when I took IE5900 Electromechanical Robotic Systems at Purdue.

It's a project to establish two-factor authentication using a password and a camera to gain access to a system.

It was developed using an Arduino board, three push-buttons, a buzzer, and an LCD display. Additionally, a computer with access to a camera is also needed

The first form of authentication is done by typing a 3-digit numerical code through the buttons and the LCD display. The password and the user name can be set in the Arduino file.

The second authentication is done by face detection through a Haar Cascade classifier. See example below:

https://opencv-python-tutroals.readthedocs.io/en/latest/py_tutorials/py_objdetect/py_face_detection/py_face_detection.html

To train a new face put frontal pictures of the face of the user in the images folder and run the training script.

Link to demonstration of the project: https://www.youtube.com/watch?v=NnVcloafM6U
