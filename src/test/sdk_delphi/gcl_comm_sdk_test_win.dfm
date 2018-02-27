object gcl_comm_sdk_test_frm: Tgcl_comm_sdk_test_frm
  Left = 327
  Top = 180
  Width = 1028
  Height = 579
  Caption = #27979#35797'ICS'#19982'PE'#30340#36890#35759
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 0
    Top = 317
    Width = 1012
    Height = 3
    Cursor = crVSplit
    Align = alBottom
  end
  object infoEd: TMemo
    Left = 0
    Top = 320
    Width = 1012
    Height = 221
    Align = alBottom
    Lines.Strings = (
      'out info')
    TabOrder = 0
  end
  object PageControl2: TPageControl
    Left = 0
    Top = 0
    Width = 1012
    Height = 317
    ActivePage = TabSheet3
    Align = alClient
    TabOrder = 1
    object TabSheet7: TTabSheet
      Caption = #24120#35268#27979#35797
      ImageIndex = 6
      object GroupBox3: TGroupBox
        Left = 153
        Top = 0
        Width = 722
        Height = 289
        Align = alClient
        Caption = #21457#36865#21629#20196
        TabOrder = 0
        object param_ed: TMemo
          Left = 2
          Top = 65
          Width = 718
          Height = 222
          Align = alClient
          Lines.Strings = (
            'A=B')
          TabOrder = 0
        end
        object Panel1: TPanel
          Left = 2
          Top = 15
          Width = 718
          Height = 50
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 1
          object Label4: TLabel
            Left = 333
            Top = 16
            Width = 7
            Height = 13
            Caption = 'X'
          end
          object command_ed: TComboBox
            Left = 1
            Top = 12
            Width = 328
            Height = 21
            ItemHeight = 13
            TabOrder = 0
            Text = 'ticket'
            Items.Strings = (
              'ticket'
              'reply_desample'
              'reply_sample'
              'quest_user_info'
              'quest_user_power'
              'quest_user_check'
              'quest_test_result_commit_status'
              'post_error')
          end
          object message_command_send_bn: TButton
            Left = 384
            Top = 0
            Width = 105
            Height = 41
            Caption = #21457#36865
            TabOrder = 1
            OnClick = message_command_send_bnClick
          end
          object SpinEdit2: TSpinEdit
            Left = 542
            Top = 10
            Width = 57
            Height = 26
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -15
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            MaxValue = 99999
            MinValue = 10
            ParentFont = False
            TabOrder = 2
            Value = 50
          end
          object Button2: TButton
            Left = 600
            Top = 0
            Width = 113
            Height = 41
            Caption = '(ms) '#24320#22987#23450#26102#21457
            TabOrder = 3
            OnClick = Button2Click
          end
          object SpinEdit3: TSpinEdit
            Left = 344
            Top = 11
            Width = 41
            Height = 22
            MaxValue = 999
            MinValue = 1
            TabOrder = 4
            Value = 1
          end
        end
      end
      object GroupBox1: TGroupBox
        Left = 0
        Top = 0
        Width = 153
        Height = 289
        Align = alLeft
        Caption = #21021#22987#21270' '#19982' '#21453#21021#22987#21270
        TabOrder = 1
        object initBn: TButton
          Left = 15
          Top = 24
          Width = 121
          Height = 52
          Caption = '*** '#21021#22987#21270' ***'
          TabOrder = 0
          OnClick = initBnClick
        end
        object cleanupBn: TButton
          Left = 15
          Top = 232
          Width = 121
          Height = 49
          Caption = #28165#38500#20197#19979
          TabOrder = 1
          OnClick = cleanupBnClick
        end
        object isconnectedBn: TButton
          Left = 15
          Top = 86
          Width = 121
          Height = 57
          Caption = #35810#38382#36830#25509#29366#24577
          TabOrder = 2
          OnClick = isconnectedBnClick
        end
        object erroridBn: TButton
          Left = 15
          Top = 149
          Width = 121
          Height = 52
          Caption = #26597#35810#26412#32456#31471#20449#24687
          TabOrder = 3
          OnClick = erroridBnClick
        end
      end
      object Panel3: TPanel
        Left = 875
        Top = 0
        Width = 129
        Height = 289
        Align = alRight
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 2
        object Label1: TLabel
          Left = 20
          Top = 224
          Width = 8
          Height = 13
          Caption = 'id'
        end
        object Label2: TLabel
          Left = 24
          Top = 64
          Width = 41
          Height = 13
          Caption = 'sourceId'
        end
        object Label3: TLabel
          Left = 24
          Top = 120
          Width = 36
          Height = 13
          Caption = 'targetId'
        end
        object testBn: TButton
          Left = 12
          Top = 243
          Width = 105
          Height = 30
          Caption = 'call : dll callback id'
          TabOrder = 0
          OnClick = testBnClick
        end
        object delphiTestBn: TButton
          Left = 13
          Top = 12
          Width = 104
          Height = 29
          Caption = 'delphi test'
          TabOrder = 1
          OnClick = delphiTestBnClick
        end
        object SpinEdit1: TSpinEdit
          Left = 33
          Top = 219
          Width = 84
          Height = 22
          MaxValue = 0
          MinValue = 0
          TabOrder = 2
          Value = 1
        end
        object sourceIdEd: TEdit
          Left = 24
          Top = 80
          Width = 89
          Height = 21
          TabOrder = 3
          Text = '0x1830101'
        end
        object targetIdEd: TEdit
          Left = 24
          Top = 136
          Width = 89
          Height = 21
          TabOrder = 4
          Text = '0x1830101'
        end
      end
    end
    object TabSheet6: TTabSheet
      Caption = #25991#20214#21457#36865
      ImageIndex = 5
      object GroupBox2: TGroupBox
        Left = 0
        Top = 0
        Width = 977
        Height = 58
        Align = alTop
        Caption = #21457#36865#25991#20214
        TabOrder = 0
        object file_transfer_send_bn: TButton
          Left = 848
          Top = 16
          Width = 113
          Height = 28
          Caption = #21457#36865
          TabOrder = 0
          OnClick = file_transfer_send_bnClick
        end
        object file_transfer_send_ed: TEdit
          Left = 8
          Top = 24
          Width = 833
          Height = 21
          TabOrder = 1
          Text = 'c:\0.txt'
        end
      end
    end
    object TabSheet1: TTabSheet
      Caption = #36965#20449#21457#36865
      object StringGrid1: TStringGrid
        Left = 0
        Top = 0
        Width = 870
        Height = 289
        Align = alClient
        TabOrder = 0
        ColWidths = (
          64
          76
          64
          64
          64)
      end
      object Panel2: TPanel
        Left = 870
        Top = 0
        Width = 134
        Height = 289
        Align = alRight
        BevelOuter = bvNone
        TabOrder = 1
        object Button1: TButton
          Left = 16
          Top = 8
          Width = 105
          Height = 137
          Caption = #25353#34920#26684#20869#23481#21457
          TabOrder = 0
          OnClick = Button1Click
        end
        object yx_sendBn: TButton
          Left = 9
          Top = 197
          Width = 120
          Height = 36
          Caption = #31616#21333#26679#20363#36965#20449
          TabOrder = 1
          OnClick = yx_sendBnClick
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = #36965#20449#25509#25910
      ImageIndex = 1
    end
    object TabSheet3: TTabSheet
      Caption = #36965#27979#21457#36865
      ImageIndex = 2
      object yc_send_bn: TButton
        Left = 401
        Top = 73
        Width = 120
        Height = 36
        Caption = #31616#21333#26679#20363#36965#27979
        TabOrder = 0
        OnClick = yc_send_bnClick
      end
    end
    object TabSheet4: TTabSheet
      Caption = #36965#27979#25509#25910
      ImageIndex = 3
    end
    object TabSheet5: TTabSheet
      Caption = #36965#25511#20219#21153
      ImageIndex = 4
    end
  end
  object timerSendMessage: TTimer
    Enabled = False
    OnTimer = timerSendMessageTimer
    Left = 656
    Top = 48
  end
end
