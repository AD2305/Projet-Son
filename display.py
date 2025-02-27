import sys
from PyQt5.QtCore import Qt
#from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QGridLayout, QSlider, QLabel, QGroupBox,QHBoxLayout, QVBoxLayout
from functools import partial
import serial
import time


"""
rajouter volume globale

feuille qss


"""

class Fenetre(QWidget):
    def __init__(self):
        QWidget.__init__(self)

        #initialisation connexion
        self.serial_port = None
        self.init_serial()

        self.slidersvol = []
        self.slidersfla = []
        self.sliderscho = []
        self.slidersgra = []
        self.labelsvol = []
        self.labelsfla = []
        self.labelscho = []
        self.labelsgra = []
        self.boutons = []
        
        enregistrement = QGroupBox("Enregistrement")
        enregistrement_layout = QHBoxLayout()
        start_stop = QGroupBox("")
        start_stop_layout = QHBoxLayout()
        piste = QGroupBox("Piste")
        piste_layout = QHBoxLayout()
        volume_group = QGroupBox("Contrôles de Volume")
        volume_layout = QGridLayout()
        effet_group = QGroupBox("Effet")
        effet_layout = QGridLayout()

        layout = QVBoxLayout()

        self.record=QPushButton(f"Record")
        self.record.setProperty("active", False)
        self.record.clicked.connect(partial(self.record_bouton))
        enregistrement_layout.addWidget(self.record)

        self.stoprecord=QPushButton(f"Stop Record")
        self.stoprecord.clicked.connect(partial(self.stop_record_bouton))
        enregistrement_layout.addWidget(self.stoprecord)
        
        self.stop=QPushButton(f"Stop")
        self.stop.setProperty("active", False)
        self.stop.clicked.connect(partial(self.stop_bouton))
        start_stop_layout.addWidget(self.stop)

        self.start=QPushButton(f"Start")
        self.start.setProperty("active", False)
        self.start.clicked.connect(partial(self.start_bouton))
        start_stop_layout.addWidget(self.start)

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
            volume.setEnabled(False)

            # creation slider effet
            flanger= QSlider(Qt.Vertical)
            flanger.setRange(0,100)
            flanger.setSingleStep(5)
            flanger.setTickPosition(QSlider.TicksBelow)
            flanger.setTickInterval(10)
            flanger.setEnabled(False)

            chorus= QSlider(Qt.Vertical)
            chorus.setRange(0,100)
            chorus.setSingleStep(5)
            chorus.setTickPosition(QSlider.TicksBelow)
            chorus.setTickInterval(10)
            chorus.setEnabled(False)

            granular= QSlider(Qt.Vertical)
            granular.setRange(0,100)
            granular.setSingleStep(5)
            granular.setTickPosition(QSlider.TicksBelow)
            granular.setTickInterval(10)
            granular.setEnabled(False)

            # Label volume
            labelvol = QLabel(f"Volume: {volume.value()}", self)

            # Label effet
            labelfla = QLabel(f"Flanger: {flanger.value()}", self)
            labelcho = QLabel(f"Chorus: {chorus.value()}", self)
            labelgra = QLabel(f"Granular: {granular.value()}", self)

            # on connecte le signal "clicked" a la methode appui_bouton + envoi teensy
            bouton.clicked.connect(partial(self.piste_bouton, i+1))

            # connexion label & slider + envoi teensy
            volume.valueChanged.connect(partial(self.update_label_volume, label=i))

            # connexion label & slider + envoi teensy                                 
            flanger.valueChanged.connect(partial(self.update_label_flange, label=i))
            chorus.valueChanged.connect(partial(self.update_label_chorus, label=i))
            granular.valueChanged.connect(partial(self.update_label_granular, label=i))

            # Affichage 
            volume_layout.addWidget(volume,0,i,alignment=Qt.AlignmentFlag.AlignCenter)
            volume_layout.addWidget(labelvol,1,i,alignment=Qt.AlignmentFlag.AlignCenter)
            piste_layout.addWidget(bouton)
            effet_layout.addWidget(flanger,0,i,alignment=Qt.AlignmentFlag.AlignCenter)
            effet_layout.addWidget(labelfla,1,i,alignment=Qt.AlignmentFlag.AlignCenter)
            effet_layout.addWidget(chorus,2,i,alignment=Qt.AlignmentFlag.AlignCenter)
            effet_layout.addWidget(labelcho,3,i,alignment=Qt.AlignmentFlag.AlignCenter)
            effet_layout.addWidget(granular,4,i,alignment=Qt.AlignmentFlag.AlignCenter)
            effet_layout.addWidget(labelgra,5,i,alignment=Qt.AlignmentFlag.AlignCenter)




            self.slidersvol.append(volume)
            self.labelsvol.append(labelvol)
            self.slidersfla.append(flanger)
            self.labelsfla.append(labelfla)
            self.sliderscho.append(chorus)
            self.labelscho.append(labelcho)
            self.slidersgra.append(granular)
            self.labelsgra.append(labelgra)
            self.boutons.append(bouton)

        volume_group.setLayout(volume_layout)
        piste.setLayout(piste_layout)
        effet_group.setLayout(effet_layout)
        start_stop.setLayout(start_stop_layout)
        volume_group.setLayout(volume_layout)
        enregistrement.setLayout(enregistrement_layout)
        
        layout.addWidget(enregistrement)
        layout.addWidget(start_stop)
        layout.addWidget(piste)
        layout.addWidget(volume_group)
        layout.addWidget(effet_group)
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
        
        self.record.setProperty("active",True)
        self.style().unpolish(self.record)
        self.style().polish(self.record)

        for button in [self.start, self.stop]:
            button.setProperty("active", False)
            self.style().unpolish(button)
            self.style().polish(button)

        for i in range(len(self.boutons)):
            self.boutons[i].setProperty("active", False)
            self.slidersvol[i].setEnabled(False)
            self.slidersfla[i].setEnabled(False)

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

        self.record.setProperty("active",False)
        self.style().unpolish(self.record)
        self.style().polish(self.record)

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

        self.stop.setProperty("active",True)
        self.style().unpolish(self.stop)
        self.style().polish(self.stop)

        self.start.setProperty("active",False)
        self.style().unpolish(self.start)
        self.style().polish(self.start)

        for i in range(len(self.boutons)):
            self.boutons[i].setProperty("active", False)
            self.slidersvol[i].setEnabled(False)
            self.slidersfla[i].setEnabled(False)
            self.sliderscho[i].setEnabled(False)
            self.slidersgra[i].setEnabled(False)

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

        self.start.setProperty("active",True)
        self.style().unpolish(self.start)
        self.style().polish(self.start)

        self.stop.setProperty("active",False)
        self.style().unpolish(self.stop)
        self.style().polish(self.stop)

        for i in range(len(self.boutons)):
            self.boutons[i].setProperty("active", False)
            self.slidersvol[i].setEnabled(False)
            self.slidersfla[i].setEnabled(False)
            self.sliderscho[i].setEnabled(False)
            self.slidersgra[i].setEnabled(False)

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
            self.slidersvol[bouton].setEnabled(False)
            self.slidersfla[bouton].setEnabled(False)
            self.sliderscho[bouton].setEnabled(False)
            self.slidersgra[bouton].setEnabled(False)
        else:
            self.boutons[bouton].setProperty("active", True)
            self.slidersvol[bouton].setEnabled(True)
            self.slidersfla[bouton].setEnabled(True)
            self.sliderscho[bouton].setEnabled(True)
            self.slidersgra[bouton].setEnabled(True)

        for button in [self.start, self.stop]:
            button.setProperty("active", False)
            self.style().unpolish(button)
            self.style().polish(button)

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

    

    def update_label_volume(self, value,label):
        """Met à jour le texte du label avec la valeur du slider."""
        self.labelsvol[label].setText(f"Volume: {value}")
             


        if self.serial_port and self.serial_port.isOpen():
            try:
                message = f"VOLUME:{value}\n"
                self.serial_port.write(message.encode())  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")
        
        time.sleep(0.1)
    


    def update_label_flange(self, value,label):
        """Met à jour le texte du label avec la valeur du slider."""
        self.labelsfla[label].setText(f"Flanger: {value}")
             


        if self.serial_port and self.serial_port.isOpen():
            try:
                message = f"FLANGER:{value}\n"
                self.serial_port.write(message.encode())  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")
        
        time.sleep(0.1)
    


    def update_label_chorus(self, value,label):
        """Met à jour le texte du label avec la valeur du slider."""
        self.labelscho[label].setText(f"Chorus: {value}")
             


        if self.serial_port and self.serial_port.isOpen():
            try:
                message = f"CHORUS:{value}\n"
                self.serial_port.write(message.encode())  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")
        
        time.sleep(0.1)
    


    def update_label_granular(self, value,label):
        """Met à jour le texte du label avec la valeur du slider."""
        self.labelsgra[label].setText(f"Granular: {value}")
             


        if self.serial_port and self.serial_port.isOpen():
            try:
                message = f"GRANULAR:{value}\n"
                self.serial_port.write(message.encode())  
                print(f"Envoyé : {message.strip()}")
            except serial.SerialException:
                print("Erreur d'envoi des données")
        
        time.sleep(0.1)

    

app = QApplication.instance() 
if not app:
    app = QApplication(sys.argv)

with open("style.qss", "r") as fichier:
    style = fichier.read()
    app.setStyleSheet(style)
    
fen = Fenetre()
fen.show()

app.exec_()

