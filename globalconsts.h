#ifndef GLOBALCONSTS_H
#define GLOBALCONSTS_H

//#define BUTTON_COLOR QString("QPushButton {background-color: #A98332;} QPushButton:pressed {background-color: #544119;}") //Button-Color FBH-Gold
//#define BUTTON_COLOR QString("QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #0d5ca6, stop: 1 #2198c0);} QPushButton:pressed {background-color: #544119;}") //Button-Color FBH-Gold
//#define BUTTON_COLOR QString("QPushButton {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #A98332, stop: 1 #544119);} QPushButton:pressed {background-color: #544119;}") //Button-Color FBH-Gold
//#define BUTTON_COLOR QString("QPushButton {border-style: solid; border-width: 2px; border-radius: 5px; border-color: rgba(254,197,75,255); background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(254,197,75,255), stop: 1 rgba(85,65,25,255))}" "QPushButton:pressed {background-color: #rgba(85,65,25,255);}") //Button-Color FBH-Gold
//#define BUTTON_COLOR QString("QPushButton {border-style: solid; border-width: 1px; border-radius: 5px; border-color: rgba(254,254,254,255); background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(200,200,200,255), stop: 1 rgba(80,80,80,255))}" "QPushButton:pressed {background-color: #rgba(80,80,80,255);}")
#define BUTTON_COLOR                 "QPushButton {color:rgb(250,250,250);border-style: solid; border-width: 1px; border-radius: 5px; border-color: rgba(254,254,254,255); background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(180,180,180), stop: 1 rgb(20,20,20))}"
#define BUTTON_COLOR_PRESSED "QPushButton:pressed {border-style: solid; border-width: 1px; border-radius: 5px; border-color: rgba(254,254,254,255); background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(20,20,20), stop: 1 rgb(180,180,180))}"
#define BUTTON_FONT_12PX "QPushButton {font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:12pt;}"
#define BUTTON_FONT_14PX    "QPushButton {font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:14pt;}"
#define BUTTON_FONT_7PX         "QPushButton {font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:7pt;}"
#define BUTTON_FONT         "QPushButton {font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:11pt;}"
#define CUSTOMER_BUTTON_FONT         "QPushButton {font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:20pt;}"

//#define GROUPBOX_STYLE QString("QGroupBox {color:rgba(254,197,75,255);border-style: solid; border-width: 1px; border-radius: 5px; border-color: rgba(254,197,75,255); }")
//#define GROUPBOX_STYLE QString("QGroupBox {color:rgba(254,197,75,255);border-style: groove; border-width: 2px;border-radius: 5px; border-color: rgba(254,197,75,255); }")
//#define GROUPBOX_STYLE QString("QGroupBox {color:rgba(254,197,75,255);border-style: solid; border-width: 1px; border-color: rgba(254,197,75,255); }")
//#define GROUPBOX_STYLE QString("QGroupBox {color:rgba(254,197,75,255);border-style: solid; border-width: 1px;border-radius: 5px; margin-top: 2ex; /* leave space at the top for the title */border-color: rgba(169,131,50,255); }")
#define GROUPBOX_STYLE "QGroupBox { background-color : rgba(0,0,0,0); color : rgba(255,255,255,255); border: 1px solid gray;border-radius: 9px;margin-top: 1em;}"
#define GROUPBOX_TITLE "QGroupBox::title {subcontrol-origin: margin; left: 10px; padding: -10px 3px -10px 3px; color: rgb(204,204,204);}"

#define CHKBOX_STYLE "QRadioButton {color:rgb(204,204,204);}"

//#define EDIT_BOX_STYLE "QLineEdit{background-color:rgba(255,255,255,65);color:rgba(255,255,255,255);font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:14pt;border:1px solid #FFFFFF;border-radius: 1px;}")
#define EDIT_BOX_STYLE "QLineEdit{background-color:rgba(255,255,255,65);color:rgba(200,200,200,200);font-size:14pt;border:1px solid #FFFFFF;border-radius: 5px;}"

//#define SPIN_BOX_STYLE "QSpinBox{background-color:rgba(255,255,255,50);color:rgba(255,255,255,255);font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:11pt;border:1px solid #FFFFFF;border-radius: 5px;}"
//#define CUSTOMER_SPIN_BOX_STYLE "QSpinBox{background-color:rgba(255,255,255,50);color:rgba(255,255,255,255);font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:20pt;border:1px solid #FFFFFF;border-radius: 5px;}"
#define SPIN_BOX_STYLE "QSpinBox{background-color:rgba(255,255,255,50);color:rgba(255,255,255,255);border:1px solid #FFFFFF;border-radius: 5px;}"
#define CUSTOMER_SPIN_BOX_STYLE "QSpinBox{background-color:rgba(255,255,255,50);color:rgba(255,255,255,255);border:1px solid #FFFFFF;border-radius: 5px;}"

//#define COMBOBOX_STYLE "QComboBox{background-color:rgba(255,255,255,50);color:rgba(255,255,255,255);font-family:\"Lucida Grande\",Lucida,Verdana,sans-serif;font-size:11pt;border:1px solid #FFFFFF;border-radius: 5px;}"
#define COMBOBOX_STYLE "QComboBox{background-color:rgba(255,255,255,50);color:rgba(255,255,255,255);border:1px solid #FFFFFF;border-radius: 5px;selection-background-color: #111; selection-color: yellow;}"
#define COMBOBOX_EDITABLE "QAbstractItemView{background-color:rgba(120,120,120,255);color:rgba(255,255,255,255);}"

#endif // GLOBALCONSTS_H
