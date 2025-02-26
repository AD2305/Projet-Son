import sys
from PyQt5.QtCore import Qt
#from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QGridLayout, QSlider, QLabel
from functools import partial
import serial
import time





# Connexion avec teensy  ( COM3 sous Windows, /dev/ttyUSB0 sous Linux/Mac)
"""
Faire test avec println

arduino = serial.Serial(port='COM3', baudrate=9600, timeout=1)

def send_command(command):
    # Envoie une commande à l'Arduino et affiche la réponse.
    arduino.write((command + "\n").encode())  # Envoie la commande
    response = arduino.readline().decode().strip()  # Lit la réponse
    print("Réponse de l'Arduino:", response)
"""


"""
rajouter volume globale

feuille qss

Variable à envoyer :
-piste séléctionnée
-slider volume -> gérer changement en direct
-record -> comment gérer les info niveau teensy

"""

class Fenetre(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        #initialisation connexion
        self.serial_port = None
        self.init_serial()

        self.slidersvol = []
        self.sliderseff = []
        self.labelsvol = []
        self.labelseff = []
        self.boutons = []

        layout = QGridLayout()

        self.record=QPushButton(f"Record")
        self.record.clicked.connect(partial(self.record_bouton))
        layout.addWidget(self.record,0,0,alignment=Qt.AlignmentFlag.AlignCenter)

        self.stoprecord=QPushButton(f"Stop Record")
        self.stoprecord.clicked.connect(partial(self.stop_record_bouton))
        layout.addWidget(self.stoprecord,0,1,alignment=Qt.AlignmentFlag.AlignCenter)
        
        self.stop=QPushButton(f"Stop")
        self.stop.clicked.connect(partial(self.stop_bouton))
        layout.addWidget(self.stop,0,2,alignment=Qt.AlignmentFlag.AlignCenter)

        self.start=QPushButton(f"Start")
        self.start.clicked.connect(partial(self.start_bouton))
        layout.addWidget(self.start,0,3,alignment=Qt.AlignmentFlag.AlignCenter)

        #self.status_label = QLabel("Status: Prêt", self)
        #layout.addWidget(self.status_label)


        for i in range(4):

            # creation boutons
            bouton = QPushButton(f"Piste {i+1}")
            bouton.setProperty("active", False)
            
            # creation sliders volume
            volume= QSlider(Qt.Vertical)
            volume.setRange(0,100)
            volume.setSingleStep(5)
            volume.setTickPosition(QSlider.TicksBelow)
            volume.setTickInterval(10)
            volume.setValue(50)

            # creation slider effet
            effet= QSlider(Qt.Vertical)
            effet.setRange(0,100)
            effet.setSingleStep(5)
            effet.setTickPosition(QSlider.TicksBelow)
            effet.setTickInterval(10)

            # Label volume
            labelvol = QLabel(f"Volume: {volume.value()}", self)

            # Label effet
            labeleff = QLabel(f"Effet: {effet.value()}", self)

            # on connecte le signal "clicked" a la methode appui_bouton + envoi teensy
            bouton.clicked.connect(partial(self.piste_bouton, i+1))

            # connexion label & slider + envoi teensy
            volume.valueChanged.connect(partial(self.update_label, label=i))

            # connexion label & slider + envoi teensy                                 ajouter fonction
            effet.valueChanged.connect(partial(self.update_label, label=i))
            
            # Affichage 
            layout.addWidget(bouton,1,i,alignment=Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(volume,2,i,alignment=Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(labelvol,3,i,alignment=Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(effet,4,i,alignment=Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(labeleff,5,i,alignment=Qt.AlignmentFlag.AlignCenter)

            self.slidersvol.append(volume)
            self.labelsvol.append(labelvol)
            self.sliderseff.append(effet)
            self.labelseff.append(labeleff)
            self.boutons.append(bouton)



        self.setLayout(layout)

        self.setWindowTitle("Looper")
    

    # gestion connexion teensy
    def init_serial(self):
        """Initialise la connexion série avec le Teensy"""
        try:
            self.serial_port = serial.Serial("COM3",115200, timeout=1) 
            time.sleep(2) 
            print("Connexion établie")
        except serial.SerialException:
            print("Impossible de se connecter")

    def closeEvent(self, event):
        """Ferme proprement la connexion série"""
        if self.serial_port and self.serial_port.isOpen():
            try:
                message = "reset\n"  
                self.serial_port.write(message.encode())  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")
            self.serial_port.close()
            print("Connexion fermée")
        event.accept()



    # gestion bouton
    def record_bouton(self):
        print("Appui sur le bouton record")

        for i in range(len(self.boutons)):
            self.boutons[i].setProperty("active", False)

        for i in self.boutons:
            self.style().unpolish(i)
            self.style().polish(i)

        if self.serial_port and self.serial_port.isOpen():
            try:
                self.serial_port.write(b"PISTE:record\n")  
                print(f"Envoyé : record")
            except serial.SerialException:
                print("Erreur d'envoi des données")



    def stop_record_bouton(self):
        print("Appui sur le bouton stop record")

        for i in range(len(self.boutons)):
            self.boutons[i].setProperty("active", False)

        for i in self.boutons:
            self.style().unpolish(i)
            self.style().polish(i)

        if self.serial_port and self.serial_port.isOpen():
            try:
                message = "stop\n"  
                self.serial_port.write(b"PISTE:stopRecord\n")  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")
        


    def stop_bouton(self):
        print("Appui sur le bouton stop")

        for i in range(len(self.boutons)):
            self.boutons[i].setProperty("active", False)

        for i in self.boutons:
            self.style().unpolish(i)
            self.style().polish(i)

        if self.serial_port and self.serial_port.isOpen():
            try:
                message = "PISTE:stop\n"  
                self.serial_port.write(message.encode())  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")



    def start_bouton(self):
        print("Appui sur le bouton start")

        for i in range(len(self.boutons)):
            self.boutons[i].setProperty("active", False)

        for i in self.boutons:
            self.style().unpolish(i)
            self.style().polish(i)

        if self.serial_port and self.serial_port.isOpen():
            try:
                message = "PISTE:start\n"  
                self.serial_port.write(b"PISTE:start\n")  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")



    def piste_bouton(self, bouton):
        print(f"Appui sur le bouton {bouton}")
        bouton=bouton-1
        
        if self.boutons[bouton].property("active") == True:
            self.boutons[bouton].setProperty("active", False)
        else:
            self.boutons[bouton].setProperty("active", True)

        for i in range(len(self.boutons)):
            if i!=(bouton):
                self.boutons[i].setProperty("active", False)

        for i in self.boutons:
            self.style().unpolish(i)
            self.style().polish(i)
        
        if self.serial_port and self.serial_port.isOpen():
            try:
                message =f"PISTE:{bouton+1}\n"  
                self.serial_port.write(message.encode())  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")

    

    def update_label(self, value,label):
        """Met à jour le texte du label avec la valeur du slider."""
        self.labelsvol[label].setText(f"Volume: {value}")
        time.sleep(0.02)


        if self.serial_port and self.serial_port.isOpen():
            try:
                message = f"VOLUME:{label},{value}\n"
                self.serial_port.write(message.encode())  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")

    

app = QApplication.instance() 
if not app:
    app = QApplication(sys.argv)

with open("style.qss", "r") as fichier:
    style = fichier.read()
    app.setStyleSheet(style)
    
fen = Fenetre()
fen.show()

app.exec_()

