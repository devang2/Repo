object Form1: TForm1
  Left = 411
  Top = 150
  Caption = 'QR'#20108#32500#30721#32534#30721'(build 20090130_'#38472#24191#36827')'
  ClientHeight = 473
  ClientWidth = 446
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Img: TImage
    Left = 8
    Top = 135
    Width = 329
    Height = 335
  end
  object Label1: TLabel
    Left = 345
    Top = 135
    Width = 72
    Height = 13
    Caption = #28857#30340#35937#32032#22823#23567
  end
  object Label2: TLabel
    Left = 343
    Top = 224
    Width = 48
    Height = 13
    Caption = #32416#38169#32423#21035
  end
  object Label3: TLabel
    Left = 343
    Top = 406
    Width = 36
    Height = 13
    Caption = #27169#22359#25968
  end
  object Label4: TLabel
    Left = 343
    Top = 270
    Width = 24
    Height = 13
    Caption = #29256#26412
  end
  object Label5: TLabel
    Left = 343
    Top = 318
    Width = 24
    Height = 13
    Caption = #31867#22411
  end
  object Label6: TLabel
    Left = 343
    Top = 366
    Width = 72
    Height = 13
    Caption = #22270#20687#35937#32032#22823#23567
  end
  object lbl_Pixcels: TLabel
    Left = 343
    Top = 385
    Width = 30
    Height = 13
    Caption = '10x10'
  end
  object Button1: TButton
    Left = 343
    Top = 182
    Width = 92
    Height = 25
    Caption = #37325#26032#29983#25104
    TabOrder = 0
    OnClick = Button1Click
  end
  object memo: TMemo
    Left = 8
    Top = 8
    Width = 427
    Height = 121
    Lines.Strings = (
      'p=12345678;a=342848234;w=234556.23;n=234;u=2342.65;t=287234.234;')
    ScrollBars = ssBoth
    TabOrder = 1
    OnChange = memoChange
  end
  object ScaleXY: TCSpinEdit
    Left = 343
    Top = 154
    Width = 92
    Height = 22
    MaxValue = 10
    MinValue = 1
    TabOrder = 2
    Value = 6
    OnChange = ScaleXYChange
  end
  object chkAutoVersion: TCheckBox
    Left = 373
    Top = 269
    Width = 97
    Height = 17
    Caption = #33258#21160#25193#24352
    Checked = True
    State = cbChecked
    TabOrder = 3
    OnClick = ScaleXYChange
  end
  object cboCorrectLevel: TComboBox
    Left = 345
    Top = 243
    Width = 90
    Height = 21
    ItemHeight = 13
    TabOrder = 4
    Text = 'M(15%)'
    OnChange = ScaleXYChange
    Items.Strings = (
      'L(7%)'
      'M(15%)'
      'Q(25%)'
      'H(30%)')
  end
  object spinModal: TCSpinEdit
    Left = 343
    Top = 425
    Width = 92
    Height = 22
    Enabled = False
    MaxValue = 20
    MinValue = 1
    TabOrder = 5
    Value = 4
  end
  object cboVersion: TComboBox
    Left = 343
    Top = 291
    Width = 90
    Height = 21
    ItemHeight = 13
    TabOrder = 6
    Text = #33258#21160
    OnChange = ScaleXYChange
    Items.Strings = (
      #33258#21160
      '1'
      '2'
      '3'
      '4'
      '5'
      '6'
      '7'
      '8'
      '9'
      '10'
      '11'
      '12'
      '13'
      '14'
      '15'
      '16'
      '17'
      '18'
      '19'
      '20'
      '21'
      '22'
      '23'
      '24'
      '25'
      '26'
      '27'
      '28'
      '29'
      '30'
      '31'
      '32'
      '33'
      '34'
      '35'
      '36'
      '37'
      '38'
      '39'
      '40')
  end
  object cboMaskingNo: TComboBox
    Left = 345
    Top = 339
    Width = 90
    Height = 21
    ItemHeight = 13
    TabOrder = 7
    Text = #33258#21160
    OnClick = ScaleXYChange
    Items.Strings = (
      #33258#21160
      '0'
      '1'
      '2'
      '3'
      '4'
      '5'
      '6'
      '7')
  end
end
