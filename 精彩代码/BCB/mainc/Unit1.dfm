object Form1: TForm1
  Left = 192
  Top = 130
  AutoScroll = False
  Caption = 'Form1'
  ClientHeight = 260
  ClientWidth = 554
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -21
  Font.Name = #24494#36719#38597#40657
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 28
  object Label1: TLabel
    Left = 16
    Top = 24
    Width = 66
    Height = 28
    Caption = 'Label1'
  end
  object Label2: TLabel
    Left = 16
    Top = 232
    Width = 102
    Height = 16
    Caption = 'Version:1234567890'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = #24494#36719#38597#40657
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 16
    Top = 160
    Width = 6
    Height = 28
  end
  object Edit1: TEdit
    Left = 24
    Top = 96
    Width = 121
    Height = 36
    TabOrder = 0
  end
  object Button1: TButton
    Left = 165
    Top = 95
    Width = 57
    Height = 41
    Caption = #36755#20837
    TabOrder = 1
    OnClick = Button1Click
  end
  object Timer1: TTimer
    Interval = 100
    OnTimer = Timer1Timer
    Left = 408
    Top = 152
  end
end
