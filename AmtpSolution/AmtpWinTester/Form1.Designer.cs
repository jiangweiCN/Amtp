namespace AmtpWinTester
{
    partial class AmtpcaMainForm
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
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AmtpcaMainForm));
            this.ulFileSelBtn = new System.Windows.Forms.Button();
            this.dataFileTextBox = new System.Windows.Forms.TextBox();
            this.loginBtn = new System.Windows.Forms.Button();
            this.tipTextBox = new System.Windows.Forms.RichTextBox();
            this.initBtn = new System.Windows.Forms.Button();
            this.clearBtn = new System.Windows.Forms.Button();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.logCheckbox = new System.Windows.Forms.CheckBox();
            this.licTextBox = new System.Windows.Forms.TextBox();
            this.selLicBtn = new System.Windows.Forms.Button();
            this.releaseBtn = new System.Windows.Forms.Button();
            this.logoutBtn = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // ulFileSelBtn
            // 
            this.ulFileSelBtn.Location = new System.Drawing.Point(12, 12);
            this.ulFileSelBtn.Name = "ulFileSelBtn";
            this.ulFileSelBtn.Size = new System.Drawing.Size(98, 23);
            this.ulFileSelBtn.TabIndex = 1;
            this.ulFileSelBtn.Text = "选择上传文件";
            this.ulFileSelBtn.UseVisualStyleBackColor = true;
            this.ulFileSelBtn.Click += new System.EventHandler(this.ulFileSelBtn_Click);
            // 
            // dataFileTextBox
            // 
            this.dataFileTextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::AmtpWinTester.Properties.Settings.Default, "ulDataFilePath", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.dataFileTextBox.Location = new System.Drawing.Point(126, 12);
            this.dataFileTextBox.Name = "dataFileTextBox";
            this.dataFileTextBox.Size = new System.Drawing.Size(535, 23);
            this.dataFileTextBox.TabIndex = 2;
            this.dataFileTextBox.Text = global::AmtpWinTester.Properties.Settings.Default.ulDataFilePath;
            // 
            // loginBtn
            // 
            this.loginBtn.Location = new System.Drawing.Point(12, 109);
            this.loginBtn.Name = "loginBtn";
            this.loginBtn.Size = new System.Drawing.Size(75, 23);
            this.loginBtn.TabIndex = 3;
            this.loginBtn.Text = "Login";
            this.loginBtn.UseVisualStyleBackColor = true;
            this.loginBtn.Click += new System.EventHandler(this.loginBtn_Click);
            // 
            // tipTextBox
            // 
            this.tipTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tipTextBox.Location = new System.Drawing.Point(0, 0);
            this.tipTextBox.Name = "tipTextBox";
            this.tipTextBox.Size = new System.Drawing.Size(673, 317);
            this.tipTextBox.TabIndex = 4;
            this.tipTextBox.Text = "";
            // 
            // initBtn
            // 
            this.initBtn.Location = new System.Drawing.Point(12, 79);
            this.initBtn.Name = "initBtn";
            this.initBtn.Size = new System.Drawing.Size(75, 23);
            this.initBtn.TabIndex = 5;
            this.initBtn.Text = "Init";
            this.initBtn.UseVisualStyleBackColor = true;
            this.initBtn.Click += new System.EventHandler(this.initBtn_Click);
            // 
            // clearBtn
            // 
            this.clearBtn.Location = new System.Drawing.Point(586, 79);
            this.clearBtn.Name = "clearBtn";
            this.clearBtn.Size = new System.Drawing.Size(75, 23);
            this.clearBtn.TabIndex = 6;
            this.clearBtn.Text = "Clear";
            this.clearBtn.UseVisualStyleBackColor = true;
            this.clearBtn.Click += new System.EventHandler(this.clearBtn_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.logoutBtn);
            this.splitContainer1.Panel1.Controls.Add(this.logCheckbox);
            this.splitContainer1.Panel1.Controls.Add(this.licTextBox);
            this.splitContainer1.Panel1.Controls.Add(this.selLicBtn);
            this.splitContainer1.Panel1.Controls.Add(this.releaseBtn);
            this.splitContainer1.Panel1.Controls.Add(this.loginBtn);
            this.splitContainer1.Panel1.Controls.Add(this.dataFileTextBox);
            this.splitContainer1.Panel1.Controls.Add(this.clearBtn);
            this.splitContainer1.Panel1.Controls.Add(this.ulFileSelBtn);
            this.splitContainer1.Panel1.Controls.Add(this.initBtn);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tipTextBox);
            this.splitContainer1.Size = new System.Drawing.Size(673, 458);
            this.splitContainer1.SplitterDistance = 137;
            this.splitContainer1.TabIndex = 7;
            // 
            // logCheckbox
            // 
            this.logCheckbox.AutoSize = true;
            this.logCheckbox.Checked = global::AmtpWinTester.Properties.Settings.Default.outputLog;
            this.logCheckbox.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::AmtpWinTester.Properties.Settings.Default, "outputLog", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.logCheckbox.Location = new System.Drawing.Point(222, 79);
            this.logCheckbox.Name = "logCheckbox";
            this.logCheckbox.Size = new System.Drawing.Size(90, 21);
            this.logCheckbox.TabIndex = 10;
            this.logCheckbox.Text = "Output log";
            this.logCheckbox.UseVisualStyleBackColor = true;
            // 
            // licTextBox
            // 
            this.licTextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::AmtpWinTester.Properties.Settings.Default, "licFilePath", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.licTextBox.Location = new System.Drawing.Point(126, 42);
            this.licTextBox.Name = "licTextBox";
            this.licTextBox.Size = new System.Drawing.Size(535, 23);
            this.licTextBox.TabIndex = 9;
            this.licTextBox.Text = global::AmtpWinTester.Properties.Settings.Default.licFilePath;
            // 
            // selLicBtn
            // 
            this.selLicBtn.Location = new System.Drawing.Point(13, 42);
            this.selLicBtn.Name = "selLicBtn";
            this.selLicBtn.Size = new System.Drawing.Size(97, 23);
            this.selLicBtn.TabIndex = 8;
            this.selLicBtn.Text = "选择许可文件";
            this.selLicBtn.UseVisualStyleBackColor = true;
            this.selLicBtn.Click += new System.EventHandler(this.selLicBtn_Click);
            // 
            // releaseBtn
            // 
            this.releaseBtn.Location = new System.Drawing.Point(102, 79);
            this.releaseBtn.Name = "releaseBtn";
            this.releaseBtn.Size = new System.Drawing.Size(75, 23);
            this.releaseBtn.TabIndex = 7;
            this.releaseBtn.Text = "Release";
            this.releaseBtn.UseVisualStyleBackColor = true;
            this.releaseBtn.Click += new System.EventHandler(this.releaseBtn_Click);
            // 
            // logoutBtn
            // 
            this.logoutBtn.Location = new System.Drawing.Point(102, 108);
            this.logoutBtn.Name = "logoutBtn";
            this.logoutBtn.Size = new System.Drawing.Size(75, 23);
            this.logoutBtn.TabIndex = 11;
            this.logoutBtn.Text = "Logout";
            this.logoutBtn.UseVisualStyleBackColor = true;
            this.logoutBtn.Click += new System.EventHandler(this.logoutBtn_Click);
            // 
            // AmtpcaMainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 17F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(673, 458);
            this.Controls.Add(this.splitContainer1);
            this.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(5);
            this.Name = "AmtpcaMainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Amtpca测试程序";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.AmtpcaMainForm_FormClosed);
            this.Load += new System.EventHandler(this.AmtpcaMainForm_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button ulFileSelBtn;
        private System.Windows.Forms.TextBox dataFileTextBox;
        private System.Windows.Forms.Button loginBtn;
        private System.Windows.Forms.RichTextBox tipTextBox;
        private System.Windows.Forms.Button initBtn;
        private System.Windows.Forms.Button clearBtn;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button releaseBtn;
        private System.Windows.Forms.TextBox licTextBox;
        private System.Windows.Forms.Button selLicBtn;
        private System.Windows.Forms.CheckBox logCheckbox;
        private System.Windows.Forms.Button logoutBtn;
    }
}

