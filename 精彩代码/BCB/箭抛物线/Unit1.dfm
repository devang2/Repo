object Form1: TForm1
  Left = 217
  Top = 151
  AutoScroll = False
  Caption = #25243#29289#26354#32447
  ClientHeight = 403
  ClientWidth = 625
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #24494#36719#38597#40657
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 17
  object Image1: TImage
    Left = 0
    Top = 0
    Width = 633
    Height = 409
  end
  object Label1: TLabel
    Left = 8
    Top = 371
    Width = 27
    Height = 17
    Caption = #21147#24230':'
    Transparent = True
  end
  object Label2: TLabel
    Left = 104
    Top = 371
    Width = 27
    Height = 17
    Caption = #35282#24230':'
    Transparent = True
  end
  object Label3: TLabel
    Left = 200
    Top = 371
    Width = 54
    Height = 17
    Caption = #22320#24179#32447':y='
    Transparent = True
  end
  object Button1: TButton
    Left = 368
    Top = 368
    Width = 73
    Height = 25
    Caption = #36712#36857#32447
    TabOrder = 0
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 40
    Top = 368
    Width = 49
    Height = 25
    TabOrder = 1
    Text = '60'
  end
  object Edit2: TEdit
    Left = 136
    Top = 368
    Width = 33
    Height = 25
    TabOrder = 2
    Text = '30'
  end
  object Edit3: TEdit
    Left = 256
    Top = 368
    Width = 41
    Height = 25
    TabOrder = 3
    Text = '300'
  end
  object Button2: TButton
    Left = 456
    Top = 368
    Width = 75
    Height = 25
    Caption = #39134#34892#31661
    TabOrder = 4
    OnClick = Button2Click
  end
  object Timer1: TTimer
    Interval = 20
    OnTimer = Timer1Timer
    Left = 448
    Top = 336
  end
end
