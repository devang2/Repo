object Form1: TForm1
  Left = 192
  Top = 130
  Width = 250
  Height = 282
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Timer1: TTimer
    Interval = 40
    OnTimer = Timer1Timer
    Left = 120
    Top = 112
  end
  object PopupMenu1: TPopupMenu
    Left = 152
    Top = 112
    object open1: TMenuItem
      Caption = '&open'
      OnClick = open1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object exit1: TMenuItem
      Caption = '&exit'
      OnClick = exit1Click
    end
  end
end
