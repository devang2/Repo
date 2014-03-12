object Form1: TForm1
  Left = 192
  Top = 130
  Width = 523
  Height = 465
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 14
  object Button1: TButton
    Left = 312
    Top = 296
    Width = 75
    Height = 25
    Caption = #25171#24320#25968#25454#24211
    TabOrder = 0
    OnClick = Button1Click
  end
  object Memo1: TMemo
    Left = 8
    Top = 304
    Width = 289
    Height = 105
    Lines.Strings = (
      'Memo1')
    TabOrder = 1
  end
  object Button2: TButton
    Left = 312
    Top = 328
    Width = 75
    Height = 25
    Caption = #35835#25968#25454#24211
    TabOrder = 2
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 312
    Top = 360
    Width = 75
    Height = 25
    Caption = #20889#25968#25454#24211
    TabOrder = 3
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 312
    Top = 392
    Width = 75
    Height = 25
    Caption = #25968#25454#24211#20445#23384
    TabOrder = 4
    OnClick = Button4Click
  end
  object DBGrid1: TDBGrid
    Left = 8
    Top = 8
    Width = 489
    Height = 257
    DataSource = DataSource1
    TabOrder = 5
    TitleFont.Charset = ANSI_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -12
    TitleFont.Name = 'Tahoma'
    TitleFont.Style = []
  end
  object Button5: TButton
    Left = 136
    Top = 272
    Width = 75
    Height = 25
    Caption = #26597#35810
    TabOrder = 6
    OnClick = Button5Click
  end
  object Edit1: TEdit
    Left = 48
    Top = 272
    Width = 81
    Height = 22
    TabOrder = 7
    Text = 'usa'
  end
  object ADOTable1: TADOTable
    Left = 56
    Top = 336
  end
  object DataSource1: TDataSource
    DataSet = ADOTable1
    Left = 88
    Top = 336
  end
end
