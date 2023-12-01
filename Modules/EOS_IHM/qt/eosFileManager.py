# -*- coding: utf-8 -*-
#****************************************************************************
# Copyright (c) 2015 - 2016, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************


# Form implementation generated from reading ui file 'filemanager.ui'
#
# Created: Wed Aug 26 16:07:02 2009
#      by: PyQt4 UI code generator 4.3
#

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
try :
   from PyQt5 import QtCore, QtGui # python 3
   from PyQt5.QtWidgets import QMainWindow, QWidget, QApplication, QListWidget, \
                               QAbstractItemView, QLabel, QFrame, QPushButton, \
                               QGroupBox, QRadioButton, QMenuBar, QStatusBar
except :
   from PyQt4 import QtCore, QtGui # python 2
   from PyQt4.QtGui import QMainWindow, QWidget, QApplication, QListWidget, \
                           QAbstractItemView, QLabel, QFrame, QPushButton, \
                           QGroupBox, QRadioButton, QMenuBar, QStatusBar

import sys, os, eosPrint4

class Ui_FileManager(QMainWindow):
    """The EOS GUI writes several files. To, by default, keep the files,
    the filenames end with a number which is automatically increased
    by one at each writing. The {Ui_FileManager} class is the manager
    of the written files (PostScript files .ps, thermodynamic property
    function data files .data, MED files .med). The class opens
    a window, creates lists of files through a filter using the
    suffixes and allows to remove one or several files.
    All the selection modes are possible (ExtendedSelection mode)
    using the keys Shift and Ctrl.""" 
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupUi(self)

    def setupUi(self, FileManager):
        FileManager.setObjectName("FileManager")
        FileManager.resize(QtCore.QSize(QtCore.QRect(0,0,460,452).size()).expandedTo(FileManager.minimumSizeHint()))
        FileManager.setMinimumSize (460,452)

        palette = QtGui.QPalette()

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Window,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Window,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Window,brush)
        FileManager.setPalette(palette)

        self.centralwidget = QWidget(FileManager)
        self.centralwidget.setObjectName("centralwidget")

        self.listWidget = QListWidget(self.centralwidget)
        self.listWidget.setGeometry(QtCore.QRect(30,90,241,251))
        self.listWidget.setObjectName("listWidget")
        self.listWidget.setSelectionMode(QAbstractItemView.ExtendedSelection)

        self.label = QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(90,20,271,31))

        palette = QtGui.QPalette()

        brush = QtGui.QBrush(QtGui.QColor(0,0,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Text,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Window,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.AlternateBase,brush)

        brush = QtGui.QBrush(QtGui.QColor(0,0,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Text,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Window,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.AlternateBase,brush)

        brush = QtGui.QBrush(QtGui.QColor(115,115,115))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Text,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Window,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.AlternateBase,brush)
        self.label.setPalette(palette)

        font = QtGui.QFont()
        font.setPointSize(11)
        font.setWeight(75)
        font.setBold(True)
        self.label.setFont(font)
        self.label.setFrameShape(QFrame.Box)
        self.label.setFrameShadow(QFrame.Sunken)
        self.label.setLineWidth(3)
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setObjectName("label")

        self.pushButtonDel = QPushButton(self.centralwidget)
        self.pushButtonDel.setGeometry(QtCore.QRect(120,360,75,26))

        palette = QtGui.QPalette()

        brush = QtGui.QBrush(QtGui.QColor(255,128,124))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Button,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,128,124))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Button,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,128,124))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Button,brush)
        self.pushButtonDel.setPalette(palette)
        self.pushButtonDel.setObjectName("pushButtonDel")

        self.pushButtonFilter = QPushButton(self.centralwidget)
        self.pushButtonFilter.setGeometry(QtCore.QRect(320,210,75,26))

        palette = QtGui.QPalette()

        brush = QtGui.QBrush(QtGui.QColor(131,255,241))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Button,brush)

        brush = QtGui.QBrush(QtGui.QColor(131,255,241))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Button,brush)

        brush = QtGui.QBrush(QtGui.QColor(131,255,241))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Button,brush)
        self.pushButtonFilter.setPalette(palette)
        self.pushButtonFilter.setObjectName("pushButtonFilter")

        self.groupBoxType = QGroupBox(self.centralwidget)
        self.groupBoxType.setGeometry(QtCore.QRect(289,80,181,111))

        palette = QtGui.QPalette()

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,237,210))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Window,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,255))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,237,210))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Window,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,237,210))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Base,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,237,210))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Window,brush)
        self.groupBoxType.setPalette(palette)
        self.groupBoxType.setObjectName("groupBoxType")

        self.radioButtonData = QRadioButton(self.groupBoxType)
        self.radioButtonData.setGeometry(QtCore.QRect(10,50,181,22))
        self.radioButtonData.setObjectName("radioButtonData")

        self.radioButtonMed = QRadioButton(self.groupBoxType)
        self.radioButtonMed.setGeometry(QtCore.QRect(10,80,161,22))
        self.radioButtonMed.setObjectName("radioButtonMed")

        self.radioButtonPS = QRadioButton(self.groupBoxType)
        self.radioButtonPS.setGeometry(QtCore.QRect(10,20,161,22))
        self.radioButtonPS.setObjectName("radioButtonPS")

        self.pushButtonQuit = QPushButton(self.centralwidget)
        self.pushButtonQuit.setGeometry(QtCore.QRect(360,360,75,26))

        palette = QtGui.QPalette()

        brush = QtGui.QBrush(QtGui.QColor(255,255,127))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Active,QtGui.QPalette.Button,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,127))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Inactive,QtGui.QPalette.Button,brush)

        brush = QtGui.QBrush(QtGui.QColor(255,255,127))
        brush.setStyle(QtCore.Qt.SolidPattern)
        palette.setBrush(QtGui.QPalette.Disabled,QtGui.QPalette.Button,brush)
        self.pushButtonQuit.setPalette(palette)

        font = QtGui.QFont()
        font.setPointSize(10)
        self.pushButtonQuit.setFont(font)
        self.pushButtonQuit.setObjectName("pushButtonQuit")
        FileManager.setCentralWidget(self.centralwidget)

        self.menubar = QMenuBar(FileManager)
        self.menubar.setGeometry(QtCore.QRect(0,0,450,28))
        self.menubar.setObjectName("menubar")
        FileManager.setMenuBar(self.menubar)

        self.statusbar = QStatusBar(FileManager)
        self.statusbar.setObjectName("statusbar")
        FileManager.setStatusBar(self.statusbar)

        self.retranslateUi(FileManager)
        if sys.version_info[0] < 3:  # python 2
           QtCore.QObject.connect(self.pushButtonQuit,   QtCore.SIGNAL("clicked()"), self.QuitButton_clicked)
           QtCore.QObject.connect(self.pushButtonFilter, QtCore.SIGNAL("clicked()"), self.FilterButton_clicked)
           QtCore.QObject.connect(self.pushButtonDel,    QtCore.SIGNAL("clicked()"), self.DelButton_clicked)
        else:  # python 3
           self.pushButtonQuit.clicked.connect(self.QuitButton_clicked)
           self.pushButtonFilter.clicked.connect(self.QuitButton_clicked)
           self.pushButtonDel.clicked.connect(self.QuitButton_clicked)

        QtCore.QMetaObject.connectSlotsByName(FileManager)

        self.listWidget.clear()
        filelist=["use the filter button to list files"]
        for sss in filelist:
            self.listWidget.addItem(QApplication.translate("centralwidget",sss,None,0)) # UnicodeUTF8
        

    def retranslateUi(self, FileManager):
        FileManager.setWindowTitle(QApplication.translate("FileManager", "FileManager", None, 0))
        self.listWidget.setToolTip(QApplication.translate("FileManager", "list of the existing files of a given type", None, 0)) # UnicodeUTF8
        self.label.setText(QApplication.translate("FileManager", "EOS generated files manager", None, 0)) # UnicodeUTF8
        self.pushButtonDel.setToolTip(QApplication.translate("FileManager", "push to delete the selected files in the main list", None, 0)) # UnicodeUTF8
        self.pushButtonDel.setText(QApplication.translate("FileManager", "Delete", None, 0)) # UnicodeUTF8
        self.pushButtonFilter.setToolTip(QApplication.translate("FileManager", "push to list the by-type selected files", None, 0)) # UnicodeUTF8
        self.pushButtonFilter.setText(QApplication.translate("FileManager", "Filter", None, 0)) # UnicodeUTF8
        self.groupBoxType.setTitle(QApplication.translate("FileManager", "File types", None, 0)) # UnicodeUTF8
        self.radioButtonData.setToolTip(QApplication.translate("FileManager", "two or three columns data files", None, 0)) # UnicodeUTF8
        self.radioButtonData.setText(QApplication.translate("FileManager", "Data Files (.data)", None, 0)) # UnicodeUTF8
        self.radioButtonMed.setToolTip(QApplication.translate("FileManager", "MED formated files", None, 0)) # UnicodeUTF8
        self.radioButtonMed.setText(QApplication.translate("FileManager", "MED File (.med)", None, 0)) # UnicodeUTF8
        self.radioButtonPS.setToolTip(QApplication.translate("FileManager", "PostScript files", None, 0)) # UnicodeUTF8
        self.radioButtonPS.setText(QApplication.translate("FileManager", "PS files (.ps)", None, 0)) # UnicodeUTF8
        self.pushButtonQuit.setText(QApplication.translate("FileManager", "QUIT", None, 0)) # UnicodeUTF8

    def QuitButton_clicked(self):
        self.close()
        pass

    def FilterButton_clicked(self):
        if self.radioButtonPS.isChecked():
            suf="ps"
        elif self.radioButtonData.isChecked():
            suf="data"
        elif self.radioButtonMed.isChecked():
            suf="med"
        else:
            theout = eosPrint4.Ui_Form4(parent=self,thetext="No file type selected")
            theout.show()
            return

        self.thefilelist=[]
        self.get_the_files(".",self.thefilelist,suf)
        self.thefilelist.sort()
        # print self.thefilelist
        self.listWidget.clear()
        for sss in self.thefilelist:
            self.listWidget.addItem(QApplication.translate("centralwidget",sss,None,0)) # UnicodeUTF8
        pass

    def get_the_files(self,hdl,laliste,suf):
        ld= os.listdir(hdl)
        for f in ld:
            a = f.split(".")
            if len(a) >= 2:
                if a[1] == suf:
                    laliste.append(f)

    def DelButton_clicked(self):
        if self.listWidget.selectedItems() == []:
            return
        for k in self.listWidget.selectedItems():
            # print k.text(),self.listWidget.row(k)
            self.thefilelist.remove( k.text())
            os.remove( k.text())
        # print self.thefilelist
        self.listWidget.clear()
        for sss in self.thefilelist:
            self.listWidget.addItem(QApplication.translate("centralwidget",sss,None,0)) # UnicodeUTF8
        pass
