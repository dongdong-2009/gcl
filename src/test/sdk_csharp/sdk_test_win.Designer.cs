namespace sdk_csharp
{
    partial class sdk_test_win
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.commandEd = new System.Windows.Forms.ComboBox();
            this.initBn = new System.Windows.Forms.Button();
            this.isconnectedBn = new System.Windows.Forms.Button();
            this.fileTansferEd = new System.Windows.Forms.TextBox();
            this.file_transfer_sendBn = new System.Windows.Forms.Button();
            this.message_command_sendBn = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.tabControl2 = new System.Windows.Forms.TabControl();
            this.tabPage8 = new System.Windows.Forms.TabPage();
            this.paramEd = new System.Windows.Forms.TextBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.tabPage9 = new System.Windows.Forms.TabPage();
            this.paramEd2 = new System.Windows.Forms.TextBox();
            this.panel3 = new System.Windows.Forms.Panel();
            this.commandEd2 = new System.Windows.Forms.ComboBox();
            this.message_command_sendBn2 = new System.Windows.Forms.Button();
            this.tabPage10 = new System.Windows.Forms.TabPage();
            this.timerSendCb = new System.Windows.Forms.CheckBox();
            this.timerSendEd = new System.Windows.Forms.NumericUpDown();
            this.timerSendLs = new System.Windows.Forms.ComboBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.button2 = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.fileTansferEd3 = new System.Windows.Forms.TextBox();
            this.file_transfer_sendBn3 = new System.Windows.Forms.Button();
            this.fileTansferEd2 = new System.Windows.Forms.TextBox();
            this.file_transfer_sendBn2 = new System.Windows.Forms.Button();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.button3 = new System.Windows.Forms.Button();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.button4 = new System.Windows.Forms.Button();
            this.tabPage6 = new System.Windows.Forms.TabPage();
            this.tabPage7 = new System.Windows.Forms.TabPage();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.InfoTextBox = new System.Windows.Forms.RichTextBox();
            this.timerSendTm = new System.Windows.Forms.Timer(this.components);
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabControl2.SuspendLayout();
            this.tabPage8.SuspendLayout();
            this.panel2.SuspendLayout();
            this.tabPage9.SuspendLayout();
            this.panel3.SuspendLayout();
            this.tabPage10.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.timerSendEd)).BeginInit();
            this.panel1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // commandEd
            // 
            this.commandEd.FormattingEnabled = true;
            this.commandEd.Items.AddRange(new object[] {
            "quest_user_info",
            "quest_user_power",
            "quest_user_check",
            "quest_test_result_commit_status",
            "post_error"});
            this.commandEd.Location = new System.Drawing.Point(6, 16);
            this.commandEd.Name = "commandEd";
            this.commandEd.Size = new System.Drawing.Size(458, 20);
            this.commandEd.TabIndex = 0;
            // 
            // initBn
            // 
            this.initBn.Dock = System.Windows.Forms.DockStyle.Top;
            this.initBn.Location = new System.Drawing.Point(0, 0);
            this.initBn.Name = "initBn";
            this.initBn.Size = new System.Drawing.Size(163, 50);
            this.initBn.TabIndex = 7;
            this.initBn.Text = "*** 初始化 ***";
            this.initBn.UseVisualStyleBackColor = true;
            this.initBn.Click += new System.EventHandler(this.initBn_Click);
            // 
            // isconnectedBn
            // 
            this.isconnectedBn.Dock = System.Windows.Forms.DockStyle.Top;
            this.isconnectedBn.Location = new System.Drawing.Point(0, 50);
            this.isconnectedBn.Name = "isconnectedBn";
            this.isconnectedBn.Size = new System.Drawing.Size(163, 51);
            this.isconnectedBn.TabIndex = 8;
            this.isconnectedBn.Text = "询问连接状态";
            this.isconnectedBn.UseVisualStyleBackColor = true;
            this.isconnectedBn.Click += new System.EventHandler(this.isconnectedBn_Click);
            // 
            // fileTansferEd
            // 
            this.fileTansferEd.Location = new System.Drawing.Point(11, 29);
            this.fileTansferEd.Name = "fileTansferEd";
            this.fileTansferEd.Size = new System.Drawing.Size(627, 21);
            this.fileTansferEd.TabIndex = 11;
            this.fileTansferEd.Text = "c:\\0.txt";
            // 
            // file_transfer_sendBn
            // 
            this.file_transfer_sendBn.Location = new System.Drawing.Point(643, 20);
            this.file_transfer_sendBn.Name = "file_transfer_sendBn";
            this.file_transfer_sendBn.Size = new System.Drawing.Size(164, 37);
            this.file_transfer_sendBn.TabIndex = 12;
            this.file_transfer_sendBn.Text = "file_transfer_send";
            this.file_transfer_sendBn.UseVisualStyleBackColor = true;
            this.file_transfer_sendBn.Click += new System.EventHandler(this.file_transfer_sendBn_Click);
            // 
            // message_command_sendBn
            // 
            this.message_command_sendBn.Location = new System.Drawing.Point(470, 8);
            this.message_command_sendBn.Name = "message_command_sendBn";
            this.message_command_sendBn.Size = new System.Drawing.Size(154, 34);
            this.message_command_sendBn.TabIndex = 14;
            this.message_command_sendBn.Text = "message command send 1";
            this.message_command_sendBn.UseVisualStyleBackColor = true;
            this.message_command_sendBn.Click += new System.EventHandler(this.message_command_sendBn_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Controls.Add(this.tabPage6);
            this.tabControl1.Controls.Add(this.tabPage7);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(824, 267);
            this.tabControl1.TabIndex = 15;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.tabControl2);
            this.tabPage1.Controls.Add(this.panel1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(816, 241);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "常规测试";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // tabControl2
            // 
            this.tabControl2.Controls.Add(this.tabPage8);
            this.tabControl2.Controls.Add(this.tabPage9);
            this.tabControl2.Controls.Add(this.tabPage10);
            this.tabControl2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl2.Location = new System.Drawing.Point(166, 3);
            this.tabControl2.Name = "tabControl2";
            this.tabControl2.SelectedIndex = 0;
            this.tabControl2.Size = new System.Drawing.Size(647, 235);
            this.tabControl2.TabIndex = 15;
            // 
            // tabPage8
            // 
            this.tabPage8.Controls.Add(this.paramEd);
            this.tabPage8.Controls.Add(this.panel2);
            this.tabPage8.Location = new System.Drawing.Point(4, 22);
            this.tabPage8.Name = "tabPage8";
            this.tabPage8.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage8.Size = new System.Drawing.Size(639, 209);
            this.tabPage8.TabIndex = 0;
            this.tabPage8.Text = "message1";
            this.tabPage8.UseVisualStyleBackColor = true;
            // 
            // paramEd
            // 
            this.paramEd.Dock = System.Windows.Forms.DockStyle.Fill;
            this.paramEd.Location = new System.Drawing.Point(3, 53);
            this.paramEd.Multiline = true;
            this.paramEd.Name = "paramEd";
            this.paramEd.Size = new System.Drawing.Size(633, 153);
            this.paramEd.TabIndex = 17;
            this.paramEd.Text = "AFB033CC88\r\n1";
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.commandEd);
            this.panel2.Controls.Add(this.message_command_sendBn);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(3, 3);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(633, 50);
            this.panel2.TabIndex = 16;
            // 
            // tabPage9
            // 
            this.tabPage9.Controls.Add(this.paramEd2);
            this.tabPage9.Controls.Add(this.panel3);
            this.tabPage9.Location = new System.Drawing.Point(4, 22);
            this.tabPage9.Name = "tabPage9";
            this.tabPage9.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage9.Size = new System.Drawing.Size(639, 209);
            this.tabPage9.TabIndex = 1;
            this.tabPage9.Text = "message2";
            this.tabPage9.UseVisualStyleBackColor = true;
            // 
            // paramEd2
            // 
            this.paramEd2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.paramEd2.Location = new System.Drawing.Point(3, 53);
            this.paramEd2.Multiline = true;
            this.paramEd2.Name = "paramEd2";
            this.paramEd2.Size = new System.Drawing.Size(633, 153);
            this.paramEd2.TabIndex = 18;
            this.paramEd2.Text = "AFB033CC88\r\n1";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.commandEd2);
            this.panel3.Controls.Add(this.message_command_sendBn2);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel3.Location = new System.Drawing.Point(3, 3);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(633, 50);
            this.panel3.TabIndex = 17;
            // 
            // commandEd2
            // 
            this.commandEd2.FormattingEnabled = true;
            this.commandEd2.Items.AddRange(new object[] {
            "quest_user_info",
            "quest_user_power",
            "quest_user_check",
            "quest_test_result_commit_status",
            "post_error"});
            this.commandEd2.Location = new System.Drawing.Point(6, 16);
            this.commandEd2.Name = "commandEd2";
            this.commandEd2.Size = new System.Drawing.Size(458, 20);
            this.commandEd2.TabIndex = 0;
            // 
            // message_command_sendBn2
            // 
            this.message_command_sendBn2.Location = new System.Drawing.Point(470, 8);
            this.message_command_sendBn2.Name = "message_command_sendBn2";
            this.message_command_sendBn2.Size = new System.Drawing.Size(154, 34);
            this.message_command_sendBn2.TabIndex = 14;
            this.message_command_sendBn2.Text = "message command send 2";
            this.message_command_sendBn2.UseVisualStyleBackColor = true;
            this.message_command_sendBn2.Click += new System.EventHandler(this.message_command_sendBn2_Click);
            // 
            // tabPage10
            // 
            this.tabPage10.Controls.Add(this.timerSendCb);
            this.tabPage10.Controls.Add(this.timerSendEd);
            this.tabPage10.Controls.Add(this.timerSendLs);
            this.tabPage10.Location = new System.Drawing.Point(4, 22);
            this.tabPage10.Name = "tabPage10";
            this.tabPage10.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage10.Size = new System.Drawing.Size(639, 209);
            this.tabPage10.TabIndex = 2;
            this.tabPage10.Text = "others";
            this.tabPage10.UseVisualStyleBackColor = true;
            // 
            // timerSendCb
            // 
            this.timerSendCb.AutoSize = true;
            this.timerSendCb.Location = new System.Drawing.Point(376, 32);
            this.timerSendCb.Name = "timerSendCb";
            this.timerSendCb.Size = new System.Drawing.Size(84, 16);
            this.timerSendCb.TabIndex = 3;
            this.timerSendCb.Text = "timer send";
            this.timerSendCb.UseVisualStyleBackColor = true;
            this.timerSendCb.CheckedChanged += new System.EventHandler(this.timerSendCb_CheckedChanged);
            // 
            // timerSendEd
            // 
            this.timerSendEd.Location = new System.Drawing.Point(195, 28);
            this.timerSendEd.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.timerSendEd.Minimum = new decimal(new int[] {
            30,
            0,
            0,
            0});
            this.timerSendEd.Name = "timerSendEd";
            this.timerSendEd.Size = new System.Drawing.Size(120, 21);
            this.timerSendEd.TabIndex = 2;
            this.timerSendEd.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            // 
            // timerSendLs
            // 
            this.timerSendLs.FormattingEnabled = true;
            this.timerSendLs.Items.AddRange(new object[] {
            "message1",
            "message2",
            "file1",
            "file2",
            "file3"});
            this.timerSendLs.Location = new System.Drawing.Point(48, 28);
            this.timerSendLs.Name = "timerSendLs";
            this.timerSendLs.Size = new System.Drawing.Size(121, 20);
            this.timerSendLs.TabIndex = 1;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.button2);
            this.panel1.Controls.Add(this.button1);
            this.panel1.Controls.Add(this.isconnectedBn);
            this.panel1.Controls.Add(this.initBn);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(3, 3);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(163, 235);
            this.panel1.TabIndex = 15;
            // 
            // button2
            // 
            this.button2.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.button2.Location = new System.Drawing.Point(0, 184);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(163, 51);
            this.button2.TabIndex = 10;
            this.button2.Text = "清除以下";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button1
            // 
            this.button1.Dock = System.Windows.Forms.DockStyle.Top;
            this.button1.Location = new System.Drawing.Point(0, 101);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(163, 51);
            this.button1.TabIndex = 9;
            this.button1.Text = "查询本终端信息";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.fileTansferEd3);
            this.tabPage2.Controls.Add(this.file_transfer_sendBn3);
            this.tabPage2.Controls.Add(this.fileTansferEd2);
            this.tabPage2.Controls.Add(this.file_transfer_sendBn2);
            this.tabPage2.Controls.Add(this.fileTansferEd);
            this.tabPage2.Controls.Add(this.file_transfer_sendBn);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(816, 241);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "文件发送";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // fileTansferEd3
            // 
            this.fileTansferEd3.Location = new System.Drawing.Point(11, 115);
            this.fileTansferEd3.Name = "fileTansferEd3";
            this.fileTansferEd3.Size = new System.Drawing.Size(627, 21);
            this.fileTansferEd3.TabIndex = 15;
            this.fileTansferEd3.Text = "c:\\0.txt";
            // 
            // file_transfer_sendBn3
            // 
            this.file_transfer_sendBn3.Location = new System.Drawing.Point(644, 106);
            this.file_transfer_sendBn3.Name = "file_transfer_sendBn3";
            this.file_transfer_sendBn3.Size = new System.Drawing.Size(164, 37);
            this.file_transfer_sendBn3.TabIndex = 16;
            this.file_transfer_sendBn3.Text = "file_transfer_send";
            this.file_transfer_sendBn3.UseVisualStyleBackColor = true;
            this.file_transfer_sendBn3.Click += new System.EventHandler(this.file_transfer_sendBn3_Click);
            // 
            // fileTansferEd2
            // 
            this.fileTansferEd2.Location = new System.Drawing.Point(11, 72);
            this.fileTansferEd2.Name = "fileTansferEd2";
            this.fileTansferEd2.Size = new System.Drawing.Size(627, 21);
            this.fileTansferEd2.TabIndex = 13;
            this.fileTansferEd2.Text = "c:\\0.txt";
            // 
            // file_transfer_sendBn2
            // 
            this.file_transfer_sendBn2.Location = new System.Drawing.Point(644, 63);
            this.file_transfer_sendBn2.Name = "file_transfer_sendBn2";
            this.file_transfer_sendBn2.Size = new System.Drawing.Size(164, 37);
            this.file_transfer_sendBn2.TabIndex = 14;
            this.file_transfer_sendBn2.Text = "file_transfer_send";
            this.file_transfer_sendBn2.UseVisualStyleBackColor = true;
            this.file_transfer_sendBn2.Click += new System.EventHandler(this.file_transfer_sendBn2_Click);
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.button3);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(816, 241);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "遥信发送";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(86, 6);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(109, 30);
            this.button3.TabIndex = 0;
            this.button3.Text = "简单遥信发送";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // tabPage4
            // 
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage4.Size = new System.Drawing.Size(816, 241);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "遥信接收";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.button4);
            this.tabPage5.Location = new System.Drawing.Point(4, 22);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage5.Size = new System.Drawing.Size(816, 241);
            this.tabPage5.TabIndex = 4;
            this.tabPage5.Text = "遥测发送";
            this.tabPage5.UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(213, 6);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(109, 30);
            this.button4.TabIndex = 1;
            this.button4.Text = "简单遥测发送";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // tabPage6
            // 
            this.tabPage6.Location = new System.Drawing.Point(4, 22);
            this.tabPage6.Name = "tabPage6";
            this.tabPage6.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage6.Size = new System.Drawing.Size(816, 241);
            this.tabPage6.TabIndex = 5;
            this.tabPage6.Text = "遥测接收";
            this.tabPage6.UseVisualStyleBackColor = true;
            // 
            // tabPage7
            // 
            this.tabPage7.Location = new System.Drawing.Point(4, 22);
            this.tabPage7.Name = "tabPage7";
            this.tabPage7.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage7.Size = new System.Drawing.Size(816, 241);
            this.tabPage7.TabIndex = 6;
            this.tabPage7.Text = "遥按任务";
            this.tabPage7.UseVisualStyleBackColor = true;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.tabControl1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.InfoTextBox);
            this.splitContainer1.Size = new System.Drawing.Size(824, 528);
            this.splitContainer1.SplitterDistance = 267;
            this.splitContainer1.TabIndex = 15;
            // 
            // InfoTextBox
            // 
            this.InfoTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.InfoTextBox.Location = new System.Drawing.Point(0, 0);
            this.InfoTextBox.Name = "InfoTextBox";
            this.InfoTextBox.Size = new System.Drawing.Size(824, 257);
            this.InfoTextBox.TabIndex = 1;
            this.InfoTextBox.Text = "";
            // 
            // timerSendTm
            // 
            this.timerSendTm.Tick += new System.EventHandler(this.timerSendTm_Tick);
            // 
            // sdk_test_win
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(824, 528);
            this.Controls.Add(this.splitContainer1);
            this.Name = "sdk_test_win";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "sdk test";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.sdk_test_win_FormClosed);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabControl2.ResumeLayout(false);
            this.tabPage8.ResumeLayout(false);
            this.tabPage8.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.tabPage9.ResumeLayout(false);
            this.tabPage9.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.tabPage10.ResumeLayout(false);
            this.tabPage10.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.timerSendEd)).EndInit();
            this.panel1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.tabPage5.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox commandEd;
        private System.Windows.Forms.Button initBn;
        private System.Windows.Forms.Button isconnectedBn;
        private System.Windows.Forms.TextBox fileTansferEd;
        private System.Windows.Forms.Button file_transfer_sendBn;
        private System.Windows.Forms.Button message_command_sendBn;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.TabPage tabPage6;
        private System.Windows.Forms.TabPage tabPage7;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.RichTextBox InfoTextBox;
        private System.Windows.Forms.TabControl tabControl2;
        private System.Windows.Forms.TabPage tabPage8;
        private System.Windows.Forms.TextBox paramEd;
        private System.Windows.Forms.TabPage tabPage9;
        private System.Windows.Forms.TextBox paramEd2;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.ComboBox commandEd2;
        private System.Windows.Forms.Button message_command_sendBn2;
        private System.Windows.Forms.TextBox fileTansferEd3;
        private System.Windows.Forms.Button file_transfer_sendBn3;
        private System.Windows.Forms.TextBox fileTansferEd2;
        private System.Windows.Forms.Button file_transfer_sendBn2;
        private System.Windows.Forms.Timer timerSendTm;
        private System.Windows.Forms.TabPage tabPage10;
        private System.Windows.Forms.NumericUpDown timerSendEd;
        private System.Windows.Forms.ComboBox timerSendLs;
        private System.Windows.Forms.CheckBox timerSendCb;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
    }
}

