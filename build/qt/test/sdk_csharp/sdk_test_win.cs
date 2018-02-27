using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using gcl.comm;
using System.Runtime.InteropServices;

namespace sdk_csharp
{
    public partial class sdk_test_win : Form
    {
        public delegate void OutInfoDelegate(string sInfo);

        private OutInfoDelegate _outInfoDelegate;

        public sdk_test_win()
        {
            InitializeComponent();
            if (this.commandEd.Items.Count > 0)
            {
                this.commandEd.SelectedIndex = 0;
            }
            if (this.commandEd2.Items.Count > 0)
            {
                this.commandEd2.SelectedIndex = 0;
            }

            SdkNative.setMessageCommandDelegate(new SdkNative.MessageCommandDelegate(this.showMessageCommand));
            SdkNative.setFileTansferDelegate(new SdkNative.FileTansferDelegate(this.showFileTansfer));
            SdkNative.setYxReceivedAddressVqtText1Delegate(new SdkNative.YxReceivedAddressVqtText1Delegate(this.showYxReceivedAddressVqtText1));
            SdkNative.setYxReceivedAddressInt32VqtDelegate(new SdkNative.YxReceivedAddressInt32VqtDelegate(this.showYxReceivedAddressInt32Vqt));
            SdkNative.setYcReceivedAddressVqtText1Delegate(new SdkNative.YcReceivedAddressVqtText15Delegate(this.showYcReceivedAddressVqtText15));
            SdkNative.setYcReceivedAddressDoubleVqtDelegate(new SdkNative.YcReceivedAddressDoubleVqtDelegate(this.showYcReceivedAddressDoubleVqt));
            SdkNative.setYwReceivedAddressVqtString128Delegate(new SdkNative.YwReceivedAddressVqtString128Delegate(this.showYcReceivedAddressVqtString128));
            _outInfoDelegate = new OutInfoDelegate(doOutInfo);
        }

        private void doOutInfo(string sInfo)
        {
            this.InfoTextBox.AppendText(sInfo);
            this.InfoTextBox.AppendText("\n");
        }

        // 此是子线程的回调
        private void postInfo(string sInfo)
        {
            this.Invoke(_outInfoDelegate, sInfo); 
        }

        private void showError(int iErrorId)
        {
            postInfo("SDK Received Error - ErrorId : "  + iErrorId.ToString());
        }

        private void showMessageCommand(string sCommand, string sParam, ref SdkNative.PsmAttach attach)
        {
            postInfo("SDK Received MessageCommand - Command : " + sCommand + " ; Param : " + sParam
                + " ; attach.reason : " + attach.reason + " ; attach.containerId : " + attach.containerId + " ; attach.sourceUserId : " + attach.sourceUserId + " ; attach.targetUserID : " + attach.targetUserID + " ; attach.tag : " + attach.tag);
        }

        private void showFileTansfer(string sFilePath, ref SdkNative.PsmAttach attach)
        {
            postInfo("SDK Received FileTansfer - FilePath : " + sFilePath
                + " ; attach.reason : " + attach.reason + " ; attach.containerId : " + attach.containerId + " ; attach.sourceUserId : " + attach.sourceUserId + " ; attach.targetUserID : " + attach.targetUserID + " ; attach.tag : " + attach.tag);
        }

        private void showYxReceivedAddressVqtText1(SdkNative.AddressVqtIntText1[] yxs, ref SdkNative.PsmAttach attach)
        {
            if (yxs == null) { postInfo("YxReceivedAddressVqtText1 is null"); return; }
            postInfo("SDK Received FileTansfer - YX.Count : " + yxs.Length
                + " ; attach.reason : " + attach.reason + " ; attach.containerId : " + attach.containerId + " ; attach.sourceUserId : " + attach.sourceUserId + " ; attach.targetUserID : " + attach.targetUserID + " ; attach.tag : " + attach.tag);
        }

        private void showYxReceivedAddressInt32Vqt(SdkNative.AddressInt32Vqt[] yxs, ref SdkNative.PsmAttach attach)
        {
            if (yxs == null) { postInfo("YxReceivedAddressInt32Vqt is null"); return; }
            postInfo("SDK Received FileTansfer - YX.Count : " + yxs.Length
                + " ; attach.reason : " + attach.reason + " ; attach.containerId : " + attach.containerId + " ; attach.sourceUserId : " + attach.sourceUserId + " ; attach.targetUserID : " + attach.targetUserID + " ; attach.tag : " + attach.tag);
        }

        private void showYcReceivedAddressVqtText15(SdkNative.AddressVqtRealText15[] ycs, ref SdkNative.PsmAttach attach)
        {
            if (ycs == null) { postInfo("YcReceivedAddressVqtText15 is null"); return; }
            postInfo("SDK Received FileTansfer - YC.Count : " + ycs.Length
                + " ; attach.reason : " + attach.reason + " ; attach.containerId : " + attach.containerId + " ; attach.sourceUserId : " + attach.sourceUserId + " ; attach.targetUserID : " + attach.targetUserID + " ; attach.tag : " + attach.tag);
        }

        private void showYcReceivedAddressDoubleVqt(SdkNative.AddressDoubleVqt[] ycs, ref SdkNative.PsmAttach attach)
        {
            if (ycs == null) { postInfo("YcReceivedAddressDoubleVqt is null"); return; }
            postInfo("SDK Received FileTansfer - YC.Count : " + ycs.Length
                + " ; attach.reason : " + attach.reason + " ; attach.containerId : " + attach.containerId + " ; attach.sourceUserId : " + attach.sourceUserId + " ; attach.targetUserID : " + attach.targetUserID + " ; attach.tag : " + attach.tag);
        }

        private void showYcReceivedAddressVqtString128(SdkNative.AddressVqtString128[] yws, ref SdkNative.PsmAttach attach)
        {
            if (yws == null) { postInfo("YcReceivedAddressVqtString128 is null"); return; }
            postInfo("SDK Received FileTansfer - YC.Count : " + yws.Length
                + " ; attach.reason : " + attach.reason + " ; attach.containerId : " + attach.containerId + " ; attach.sourceUserId : " + attach.sourceUserId + " ; attach.targetUserID : " + attach.targetUserID + " ; attach.tag : " + attach.tag);
        }

        //sub thread invoke
        private void initBn_Click(object sender, EventArgs e)
        {
            string[] sLineArgs = Environment.GetCommandLineArgs();
            doOutInfo(sLineArgs.Length.ToString());
            doOutInfo(sLineArgs[0]);
            string r = SdkNative.init();
            doOutInfo(r);
        }

        private void message_command_sendBn_Click(object sender, EventArgs e)
        {
            string sCommand = this.commandEd.Text;
            string sParam =  SdkNative.joinToString(this.paramEd.Lines);
            int iResult = SdkNative.SendMessageCommand(sCommand, sParam);
            doOutInfo("Sdk.SendMessageCommand : " + Convert.ToString(iResult));
        }

        private void file_transfer_sendBn_Click(object sender, EventArgs e)
        {
            string sFilePath = this.fileTansferEd.Text;
            int iResult = SdkNative.SendFileTransfer(sFilePath);
            doOutInfo("Sdk.SendFileTransfer : " + Convert.ToString(iResult));
        }

        private void isconnectedBn_Click(object sender, EventArgs e)
        {
            string sMsg = "Connect And Heart : ";
            if (SdkNative.terminalStatus())
                sMsg += "true";
            else
                sMsg += "false";
            doOutInfo(sMsg);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string sMsg = SdkNative.terminalInfo();
            doOutInfo(sMsg);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.InfoTextBox.Clear();
        }

        private void message_command_sendBn2_Click(object sender, EventArgs e)
        {
            string sCommand = this.commandEd2.Text;
            string sParam = SdkNative.joinToString(this.paramEd2.Lines);
            int iResult = SdkNative.SendMessageCommand(sCommand, sParam);
            doOutInfo("Sdk.SendMessageCommand : " + Convert.ToString(iResult));
        }

        private void file_transfer_sendBn2_Click(object sender, EventArgs e)
        {
            string sFilePath = this.fileTansferEd2.Text;
            int iResult = SdkNative.SendFileTransfer(sFilePath);
            doOutInfo("Sdk.SendFileTransfer : " + Convert.ToString(iResult));
        }

        private void file_transfer_sendBn3_Click(object sender, EventArgs e)
        {
            string sFilePath = this.fileTansferEd3.Text;
            int iResult = SdkNative.SendFileTransfer(sFilePath);
            doOutInfo("Sdk.SendFileTransfer : " + Convert.ToString(iResult));
        }

        private void timerSendCb_CheckedChanged(object sender, EventArgs e)
        {
            this.timerSendTm.Interval = Convert.ToInt32( this.timerSendEd.Value );
            this.timerSendTm.Enabled = this.timerSendCb.Checked;
        }

        private void timerSendTm_Tick(object sender, EventArgs e)
        {
            int iIndex = this.timerSendLs.SelectedIndex;
            switch (iIndex)
            {
                case 1:
                    this.message_command_sendBn_Click(null, null);
                    break;
                case 2:
                    this.message_command_sendBn2_Click(null, null);
                    break;
                case 3:
                    this.file_transfer_sendBn_Click(null, null);
                    break;
                case 4:
                    this.file_transfer_sendBn2_Click(null, null);
                    break;
                case 5:
                    this.file_transfer_sendBn3_Click(null, null);
                    break;
                default:
                    break;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            SdkNative.AddressInt32Vqt[] yxs = new SdkNative.AddressInt32Vqt[100];
            int iAddress = 0x01000000;
            for (int i = 0; i < yxs.Length; i++)
            {
                yxs[i].address = iAddress; ++iAddress;
                yxs[i].value = i;
                yxs[i].quality = 1;
                yxs[i].datetime = DateTime.Now;
            }

            int iResult = SdkNative.SendAddressInt32Vqts(yxs);
            doOutInfo("Sdk.SendAddressInt32Vqts : " + Convert.ToString(iResult));
        }

        private void button4_Click(object sender, EventArgs e)
        {
            SdkNative.AddressDoubleVqt[] ycs = new SdkNative.AddressDoubleVqt[100];
            int iAddress = 0x02000000;
            for (int i = 0; i < ycs.Length; i++)
            {
                ycs[i].address = iAddress; ++iAddress;
                ycs[i].value = i * 1.234;
                ycs[i].quality = 1;
                ycs[i].datetime = DateTime.Now;
            }

            int iResult = SdkNative.SendAddressDoubleVqts(ycs);
            doOutInfo("Sdk.SendAddressInt32Vqts : " + Convert.ToString(iResult));
        }

        private void sdk_test_win_FormClosed(object sender, FormClosedEventArgs e)
        {
            SdkNative.clear();
        }
    }
}
