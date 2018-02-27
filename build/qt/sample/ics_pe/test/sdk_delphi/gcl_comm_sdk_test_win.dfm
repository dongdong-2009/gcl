object gcl_comm_sdk_test_frm: Tgcl_comm_sdk_test_frm
  Left = 128
  Top = 213
  Width = 1081
  Height = 480
  Caption = #27979#35797'ICS'#19982'PE'#30340#36890#35759
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object infoEd: TMemo
    Left = 0
    Top = 273
    Width = 1065
    Height = 169
    Align = alClient
    Lines.Strings = (
      'out info')
    TabOrder = 0
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1065
    Height = 273
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object register_error_bn: TButton
      Left = 568
      Top = 32
      Width = 120
      Height = 36
      Caption = 'register_error'
      TabOrder = 0
      OnClick = register_error_bnClick
    end
    object initBn: TButton
      Left = 16
      Top = 16
      Width = 108
      Height = 52
      Caption = 'initBn'
      TabOrder = 1
      OnClick = initBnClick
    end
    object cleanupBn: TButton
      Left = 936
      Top = 216
      Width = 121
      Height = 49
      Caption = 'cleanupBn'
      TabOrder = 2
      OnClick = cleanupBnClick
    end
    object erroridBn: TButton
      Left = 297
      Top = 224
      Width = 100
      Height = 36
      Caption = 'show error id'
      TabOrder = 3
      OnClick = erroridBnClick
    end
    object testBn: TButton
      Left = 24
      Top = 221
      Width = 105
      Height = 36
      Caption = 'call : dll callback id'
      TabOrder = 4
      OnClick = testBnClick
    end
    object SpinEdit1: TSpinEdit
      Left = 133
      Top = 229
      Width = 60
      Height = 22
      MaxValue = 0
      MinValue = 0
      TabOrder = 5
      Value = 1
    end
    object connectBn: TButton
      Left = 168
      Top = 32
      Width = 120
      Height = 36
      Caption = 'connect'
      TabOrder = 6
      OnClick = connectBnClick
    end
    object disconnectBn: TButton
      Left = 288
      Top = 32
      Width = 120
      Height = 36
      Caption = 'disconnect'
      TabOrder = 7
      OnClick = disconnectBnClick
    end
    object isconnectedBn: TButton
      Left = 408
      Top = 32
      Width = 120
      Height = 36
      Caption = 'isconnectedBn'
      TabOrder = 8
      OnClick = isconnectedBnClick
    end
    object yx_sendBn: TButton
      Left = 168
      Top = 176
      Width = 120
      Height = 36
      Caption = 'yx_send'
      TabOrder = 9
      OnClick = yx_sendBnClick
    end
    object register_message_command_bn: TButton
      Left = 688
      Top = 32
      Width = 120
      Height = 36
      Caption = 'message_command'
      TabOrder = 10
      OnClick = register_message_command_bnClick
    end
    object register_file_transfer_bn: TButton
      Left = 808
      Top = 32
      Width = 120
      Height = 36
      Caption = 'file_transfer'
      TabOrder = 11
      OnClick = register_file_transfer_bnClick
    end
    object register_realtime_container_data_bn: TButton
      Left = 928
      Top = 32
      Width = 129
      Height = 36
      Caption = 'realtime_container_data'
      TabOrder = 12
      OnClick = register_realtime_container_data_bnClick
    end
    object yc_send_bn: TButton
      Left = 288
      Top = 176
      Width = 120
      Height = 36
      Caption = 'yc_send'
      TabOrder = 13
      OnClick = yc_send_bnClick
    end
    object file_transfer_send_bn: TButton
      Left = 168
      Top = 112
      Width = 120
      Height = 36
      Caption = 'file_transfer_send'
      TabOrder = 14
      OnClick = file_transfer_send_bnClick
    end
    object message_command_send_bn: TButton
      Left = 568
      Top = 112
      Width = 145
      Height = 36
      Caption = 'message_command_send'
      TabOrder = 15
      OnClick = message_command_send_bnClick
    end
  end
end
