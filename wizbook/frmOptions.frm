VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmOptions 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Options"
   ClientHeight    =   3980
   ClientLeft      =   2560
   ClientTop       =   1500
   ClientWidth     =   8150
   Icon            =   "frmOptions.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3980
   ScaleWidth      =   8150
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3780
      Index           =   3
      Left            =   -20000
      ScaleHeight     =   3780
      ScaleWidth      =   5690
      TabIndex        =   5
      TabStop         =   0   'False
      Top             =   480
      Width           =   5685
      Begin VB.Frame fraSample4 
         Caption         =   "Sample 4"
         Height          =   1785
         Left            =   2100
         TabIndex        =   8
         Top             =   840
         Width           =   2055
      End
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3780
      Index           =   2
      Left            =   -20000
      ScaleHeight     =   3780
      ScaleWidth      =   5690
      TabIndex        =   4
      TabStop         =   0   'False
      Top             =   480
      Width           =   5685
      Begin VB.Frame fraSample3 
         Caption         =   "Sample 3"
         Height          =   1785
         Left            =   1545
         TabIndex        =   7
         Top             =   675
         Width           =   2055
      End
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3780
      Index           =   1
      Left            =   -20000
      ScaleHeight     =   3780
      ScaleWidth      =   5690
      TabIndex        =   3
      TabStop         =   0   'False
      Top             =   480
      Width           =   5685
      Begin VB.Frame fraSample2 
         Caption         =   "Sample 2"
         Height          =   1785
         Left            =   645
         TabIndex        =   6
         Top             =   300
         Width           =   2055
      End
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3250
      Index           =   0
      Left            =   120
      ScaleHeight     =   3250
      ScaleWidth      =   8290
      TabIndex        =   0
      TabStop         =   0   'False
      Top             =   120
      Width           =   8290
      Begin VB.Frame OutputFont 
         Caption         =   "Output font and character set translation"
         Height          =   1790
         Left            =   4440
         TabIndex        =   17
         Top             =   1080
         Width           =   3370
         Begin VB.CheckBox ExtChars 
            Caption         =   "Input uses extended characters"
            Height          =   250
            Left            =   360
            TabIndex        =   24
            Top             =   1440
            Width           =   3250
         End
         Begin VB.OptionButton CP1251 
            Caption         =   "Prompt for font and translate CP1251 cyrillic"
            Height          =   176
            Left            =   363
            TabIndex        =   22
            Top             =   1210
            Width           =   3520
         End
         Begin VB.OptionButton KOI8R 
            Caption         =   "Prompt for font and translate KOI8R cyrillic"
            Height          =   176
            Left            =   363
            TabIndex        =   21
            Top             =   968
            Width           =   3520
         End
         Begin VB.OptionButton PromptForFont 
            Caption         =   "Prompt for font"
            Height          =   176
            Left            =   363
            TabIndex        =   20
            Top             =   726
            Width           =   2552
         End
         Begin VB.OptionButton CustomFont 
            Caption         =   "Alpha-BK font"
            Height          =   176
            Left            =   360
            TabIndex        =   19
            Top             =   480
            Width           =   3762
         End
         Begin VB.OptionButton WizardFont 
            Caption         =   "Wizard font"
            Height          =   176
            Left            =   363
            TabIndex        =   18
            Top             =   242
            Value           =   -1  'True
            Width           =   3278
         End
      End
      Begin VB.Frame fraSample1 
         Caption         =   "Input options"
         Height          =   2140
         Left            =   121
         TabIndex        =   14
         Top             =   0
         Width           =   4210
         Begin VB.CheckBox DictionaryMode 
            Caption         =   "Dictionary mode"
            Height          =   250
            Left            =   360
            TabIndex        =   27
            Top             =   1680
            Width           =   3490
         End
         Begin VB.TextBox TOC 
            Height          =   250
            Left            =   2160
            TabIndex        =   25
            Top             =   1320
            Width           =   1810
         End
         Begin VB.OptionButton EachCR 
            Caption         =   "Each carriage return ends paragraph"
            Height          =   250
            Left            =   363
            TabIndex        =   23
            Top             =   960
            Width           =   3641
         End
         Begin VB.OptionButton DoubleNewline 
            Caption         =   "Recognize paragraph end by double newline"
            Height          =   370
            Left            =   363
            TabIndex        =   16
            Top             =   240
            Value           =   -1  'True
            Width           =   3278
         End
         Begin VB.OptionButton DoubleSpace 
            Caption         =   "Recognize paragraph end by tab or two spaces or more indentation"
            Height          =   370
            Left            =   363
            TabIndex        =   15
            Top             =   600
            Width           =   3730
         End
         Begin VB.Label Label1 
            Caption         =   "Table of contents entry start"
            Height          =   250
            Left            =   360
            TabIndex        =   26
            Top             =   1320
            Width           =   1690
         End
      End
      Begin VB.Frame Frame2 
         Caption         =   "Processing options"
         Height          =   850
         Left            =   121
         TabIndex        =   10
         Top             =   2280
         Width           =   4210
         Begin VB.CheckBox SaveBin 
            Caption         =   "Save binary file"
            Height          =   250
            Left            =   363
            TabIndex        =   12
            Top             =   480
            Width           =   3520
         End
         Begin VB.CheckBox Downloader 
            Caption         =   "Automatically start downloader"
            Height          =   250
            Left            =   363
            TabIndex        =   11
            Top             =   242
            Value           =   1  'Checked
            Width           =   3883
         End
      End
      Begin VB.Frame Frame1 
         Caption         =   "Output paragraph indent"
         Height          =   737
         Left            =   4440
         TabIndex        =   9
         Top             =   120
         Width           =   3370
         Begin MSComctlLib.Slider IndentParaInOutput 
            Height          =   253
            Left            =   363
            TabIndex        =   13
            ToolTipText     =   "Number of characters to indent paragraph"
            Top             =   363
            Width           =   1584
            _ExtentX        =   2790
            _ExtentY        =   462
            _Version        =   393216
            LargeChange     =   1
            Max             =   6
            SelectRange     =   -1  'True
            TextPosition    =   1
         End
      End
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Defaults"
      Height          =   375
      Left            =   6720
      TabIndex        =   2
      Top             =   3480
      Width           =   1095
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Height          =   375
      Left            =   5040
      TabIndex        =   1
      Top             =   3480
      Width           =   1095
   End
End
Attribute VB_Name = "frmOptions"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdCancel_Click()
    Unload Me
End Sub

Private Sub cmdOK_Click()
    'Unload Me
    frmOptions.Hide
End Sub


Private Sub Form_Load()
    'center the form
    Me.Move (Screen.Width - Me.Width) / 2, (Screen.Height - Me.Height) / 2
End Sub

Private Sub Text1_Change()

End Sub

