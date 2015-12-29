VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmMain 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "WizBookConverter"
   ClientHeight    =   4775
   ClientLeft      =   110
   ClientTop       =   605
   ClientWidth     =   3718
   Icon            =   "Converter.frx":0000
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   ScaleHeight     =   4775
   ScaleWidth      =   3718
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   253
      Left            =   0
      TabIndex        =   1
      Top             =   3047
      Width           =   3718
      _ExtentX        =   6557
      _ExtentY        =   453
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Text            =   "Ready"
            TextSave        =   "Ready"
            Key             =   "Status"
         EndProperty
      EndProperty
   End
   Begin VB.CommandButton Convert 
      Caption         =   "Convert"
      DownPicture     =   "Converter.frx":27A2
      Height          =   2915
      Left            =   33
      Picture         =   "Converter.frx":64A5
      Style           =   1  'Graphical
      TabIndex        =   0
      Top             =   22
      Width           =   3641
   End
   Begin MSComDlg.CommonDialog dlgCommonDialog 
      Left            =   121
      Top             =   957
      _ExtentX        =   630
      _ExtentY        =   630
      _Version        =   393216
   End
   Begin MSComctlLib.ImageList imlToolbarIcons 
      Left            =   242
      Top             =   363
      _ExtentX        =   748
      _ExtentY        =   748
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   1
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "Converter.frx":A1C4
            Key             =   "Open"
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileOpen 
         Caption         =   "&Go..."
      End
      Begin VB.Menu mnuOptions 
         Caption         =   "&Settings"
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "E&xit"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Declare Function ShellExecute _
    Lib "shell32.dll" Alias "ShellExecuteA" ( _
        ByVal hwnd As Long, _
        ByVal lpOperation As String, _
        ByVal lpFile As String, _
        ByVal lpParameters As String, _
        ByVal lpDirectory As String, _
        ByVal nShowCmd As Long) As Long

   Private Declare Function GetExitCodeProcess Lib "kernel32" _
      (ByVal hProcess As Long, lpExitCode As Long) As Long

Private Declare Function CloseHandle Lib "kernel32" _
   (ByVal hObject As Long) As Long
   
Private Declare Function OpenProcess Lib "kernel32" _
   (ByVal dwDesiredAccess As Long, ByVal bInheritHandle As Long, _
    ByVal dwProcessId As Long) As Long
     
Private Sub Form_Load()
    Me.Left = GetSetting(App.Title, "Settings", "MainLeft", 1000)
    Me.Top = GetSetting(App.Title, "Settings", "MainTop", 1000)
    Me.Width = GetSetting(App.Title, "Settings", "MainWidth", 6500)
    Me.Height = GetSetting(App.Title, "Settings", "MainHeight", 6500)
End Sub



Private Sub Form_Unload(Cancel As Integer)
    Dim i As Integer


    'close all sub forms
    For i = Forms.Count - 1 To 1 Step -1
        Unload Forms(i)
    Next
    If Me.WindowState <> vbMinimized Then
        SaveSetting App.Title, "Settings", "MainLeft", Me.Left
        SaveSetting App.Title, "Settings", "MainTop", Me.Top
        SaveSetting App.Title, "Settings", "MainWidth", Me.Width
        SaveSetting App.Title, "Settings", "MainHeight", Me.Height
    End If
End Sub

Private Sub Go_Click()
    Call mnuFileOpen_Click
End Sub
Private Sub Convert_Click()
    Call mnuFileOpen_Click
End Sub

Private Sub mnuFileExit_Click()
    'unload the form
    Unload Me
End Sub

Private Sub mnuFileOpen_Click()

    Dim sFile As String
    Dim oFile As String
    Dim hProcess&
    Dim ExitCode&
    Dim ProcessId&
    Dim options As String
    Dim Command As String
    
    
    
    'MsgBox (frmOptions.IndentParaInOutput.Value)

    With dlgCommonDialog
        .DialogTitle = "Open"
        .CancelError = False
        'ToDo: set the flags and attributes of the common dialog control
        .Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*"
        .ShowOpen
        If Len(.FileName) = 0 Or .CancelError Then
            Exit Sub
        End If
        sFile = .FileName
    End With
    
    oFile = InputBox("Filename on Wizard?")
    If oFile = "" Then Exit Sub
    If Len(oFile) > 20 Then
        MsgBox ("Filename too long!")
        Exit Sub
    End If
    options = ""
    If (frmOptions.IndentParaInOutput.Value) Then
        options = Str(frmOptions.IndentParaInOutput.Value)
        options = "-" & Right(options, Len(options) - 1)
    End If
    If (frmOptions.DoubleSpace.Value) Then options = options & " -i "
    Command = """" & App.Path & "\wizcomp.exe"" " & options & """" & sFile & """ """ & oFile & """"
    'Print Command
    StatusBar.Panels(1).Text = "Compressing"
    ProcessId = Shell(Command)
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, False, ProcessId)
    Do
      Call GetExitCodeProcess(hProcess, ExitCode)
      DoEvents
    Loop While (ExitCode = 259)
    Call CloseHandle(hProcess)
    If ExitCode <> 0 Then
        MsgBox ("Error in processing file with WizComp")
        StatusBar.Panels(1).Text = "Ready"
        Exit Sub
    End If
    Command = """" & App.Path & "\tomemo.exe"" " & oFile & " 1005 wb """ & oFile & """"
    ProcessId = Shell(Command)
    StatusBar.Panels(1).Text = "Converting to memos"
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, False, ProcessId)
    Do
      Call GetExitCodeProcess(hProcess, ExitCode)
      DoEvents
    Loop While (ExitCode = 259)
    Call CloseHandle(hProcess)
    If ExitCode <> 0 Then
        MsgBox ("Error in processing file with ToMemo")
        StatusBar.Panels(1).Text = "Ready"
        Exit Sub
    End If
    StatusBar.Panels(1).Text = "Ready"
    If (1 <> frmOptions.SaveBin.Value) Then Kill (oFile)
    If (frmOptions.Downloader.Value) Then ProcessId = ShellExecute(hwnd, "Open", oFile & ".wzd", "", "", SW_SHOWNORMAL)
End Sub
Private Sub mnuOptions_Click()
    'Set fOptionsForm = New frmOptions
    'fOptionsForm.Show
    frmOptions.Show
End Sub

Private Sub tbToolBar_ButtonClick(ByVal Button As MSComctlLib.Button)
    Call mnuFileOpen_Click
End Sub

Private Sub W_Click()

End Sub
